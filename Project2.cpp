#include<iostream>
#include<vector>
#include<queue>
#include<stack>
#include <chrono>
using namespace std;
using namespace std::chrono;

# define Task 10 // no. of tasks
# define Core 3 //no. of cores
# define Send_Power 0.5 //Power of sending task to cloud
# define C_P_1 1 // Core 1 power consumption
# define C_P_2 2 // Core 2 power consumption
# define C_P_3 4 // Core 3 power consumption
# define Send_Time 3 //Time taken to send task to the cloud
# define Compute_Time 1 //Time taekn to compute the task on cloud
# define Receive_Time 1 //Time taken to receive task from cloud

float T_i(0); //intial time
float E_i(0);  // initial energy
queue<int> execution_order;
vector<float> priority(Task);

int least(vector<int> p)
{
	int min=p[0];
	for (int i = 1; i < p.size(); i++)
		if (p[i] < min)
			min = p[i];
	return min;
}
template<class P>
P highest(vector<P> p)
{
	P max = p[0];
	for (int i = 1; i < p.size(); i++)
		if (p[i] > max)
			max = p[i];
	return max;
}
float avg(vector<int> p)
{
	int sum=0;
	for (int i = 0; i < p.size(); i++)
		sum += p[i];
	return (float)sum/(float)p.size();
}
int index1(vector<int> p)
{
	int min = 0;
	int tmp = p[0];
	for (int i = 1; i < p.size(); i++)
		if (p[i] < tmp)
		{
			tmp = p[i];
			min = i;
		}
	return min;
}
template<class P>
int index2(vector<P> p)
{
	int max = 0;
	P tmp=p[0];
	for (int i = 1; i < p.size(); i++)
		if (p[i] > tmp)
		{
			tmp = p[i];
			max = i;
		}
	return max;
}

void print(vector<int> p)
{
	cout << "vector: ";
	for (int i = 0; i < p.size(); i++)
		cout << " " << p[i];
	cout << endl;
}


vector<int> Initial_Scheduling(vector<vector <int> >matrix_adj, vector<vector <int> >Local_Task)
{
	vector<int> T_Local_min(Task),T_re(Task),flag_L_R(Task),entry(Task);
	entry[0] = 1;
	vector<float> W(Task);
	
    //Primary Assignment begins
	for (int i = 0; i < Task; i++)
	{
		T_Local_min[i] = least(Local_Task[i]);
		T_re[i] = Send_Time + Compute_Time + Receive_Time;

        if(T_re[i] < T_Local_min[i])
            flag_L_R[i] = 0;
        else
            flag_L_R[i] = 1;

		if (entry[i] == 1)
			flag_L_R[i] = 1;
        
        if(flag_L_R[i] == 0)
            W[i] = T_re[i];
        else
            W[i] = avg(Local_Task[i]);
	}

	//Task Prioritizing begins
	for (int x = Task - 1; x >= 0; x--)
	{
		vector<float>successor(Task);
		for (int y = 0; y < Task; y++)
		{
			if (matrix_adj[x][y] == 1)
				successor[y] = priority[y];
		}
		priority[x] = W[x] + highest(successor);
	}

	//Execution unit selection begins
    queue<int>exec;
	float energy(0);
	vector<float>temp_priority = priority;
	
	for (int i = 0; i < Task; i++)
	{
		int order = index2(temp_priority);
		exec.push(order);
		temp_priority[order] = 0;
	}
    
	vector<int> Task_end(Task);
    vector<int> Task_begin(Task);
	execution_order = exec;
	vector<int> S(Task);
    vector<int> channel(Core+1);
	vector<int> C_1_time = {0}, C_2_time = { 0 }, C_3_time = { 0 }, W_S_time = { 0 }, Cloud_time = { 0 }, W_R_time = { 0 };
	int current = 0;
	for (int i = 0; i < Task; i++)
	{
		current = exec.front();
		channel[0] = T_re[current];
		channel[1] = Local_Task[current][0];
		channel[2] = Local_Task[current][1];
		channel[3] = Local_Task[current][2];
		if (flag_L_R[current] == 0) // cloud task
		{
			vector<int> ws_predecessor = { 0 }, c_predecessor = { 0 };
			for (int j = 0; j < Task; j++)
			{
				if (matrix_adj[j][current] == 1)
				{
					if (S[j] == 0)
					{
						ws_predecessor.push_back(W_S_time.back());
						c_predecessor.push_back(Cloud_time.back());
					}
					else ws_predecessor.push_back(Task_end[j]);
				}
			}
			int RT_ws = highest(ws_predecessor);
			W_S_time.push_back(RT_ws + Send_Time);
			c_predecessor.push_back(RT_ws + Send_Time);
			int RT_c = highest(c_predecessor);
			Cloud_time.push_back(RT_c + Compute_Time);
			W_R_time.push_back(Cloud_time.back() + Receive_Time);
			S[current] = 0;
			Task_end[current] = W_R_time.back();
            Task_begin[current] = W_S_time.back();
			energy += (float)Send_Power*(float)Send_Time;
		}
		else// not a cloud task
		{
			vector<int> predecessor = { 0 }, FT(Core + 1), ws_predecessor = { 0 };
			for (int j = 0; j < Task; j++)
				if (matrix_adj[j][current] == 1)
				{
					if (S[j] != 0)
					{
						predecessor.push_back(Task_end[j]);
						ws_predecessor.push_back(Task_end[j]);
					}
					else
					{
						ws_predecessor.push_back(W_S_time.back());
						predecessor.push_back(Task_end[j]);
					}
				}
			int RT_l = highest(predecessor),RT_c=highest(ws_predecessor);

            if(RT_c > W_S_time.back())
                FT[0] = RT_c + channel[0];
            else
                FT[0] = W_S_time.back() + channel[0];
            
            if(RT_l > C_1_time.back())
                FT[1] = RT_l + channel[1];
            else
                FT[1] = C_1_time.back() + channel[1];
            
            if(RT_l > C_2_time.back())
                FT[2] = RT_l + channel[2];
            else
                FT[2] = C_2_time.back() + channel[2];

            if(RT_l > C_3_time.back())
                FT[3] = RT_l + channel[3];
            else
                FT[3] = C_3_time.back() + channel[3];


			if (entry[current] == 1)
				FT[0] = FT[1] + FT[2] + FT[3];
			S[current] = index1(FT);
			switch (S[current])
			{case 0:
			{
				W_R_time.push_back(FT[0]);
				Cloud_time.push_back(FT[0] - Receive_Time);
				W_S_time.push_back(FT[0] - Receive_Time - Compute_Time);
				Task_end[current] = W_R_time.back();
				energy += (float)Send_Power*(float)Send_Time;
				break;
			}
			case 1:
			{	
				C_1_time.push_back(FT[1]);
				Task_end[current] = C_1_time.back();
				energy += (float)C_P_1*(float)channel[1];
				break;
			}
			case 2:
			{
				C_2_time.push_back(FT[2]);
				Task_end[current] = C_2_time.back();
				energy += (float)C_P_2*(float)channel[2];
				break;
			}
			case 3:
			{
				C_3_time.push_back(FT[3]);
				Task_end[current] = C_3_time.back();
				energy += (float)C_P_3*(float)channel[3];
				break;
			}
			default:
				break;
			}
            Task_begin[current]=RT_l;
		}
		exec.pop();//clear the exec queue
	}
    
    T_i = highest(Task_end);//or use the last time of the exit task finished
	cout << "Total time of Initial Scheduling: " << T_i << endl;
	cout << "Total energy of Initial Scheduling: " << energy << endl;
    cout << "Start Time of Initial Scheduling: ";
    print(Task_begin);
    cout << "End Time of Initial Scheduling: " ;
    print(Task_end);
	
	E_i = energy;
	return S;
}
vector<int> Task_Migration(vector<vector <int> >matrix_adj, vector<vector <int> >Local_Task, vector<int> S, int T_max)
{   
   	int tmp_s, temp_time(0);
	vector<int> S_new(S);
	vector<int> result(Task);
	float optimal_E((float)E_i), optimal_T((float)T_i);//my sample float optimal_E(100.5), optimal_T(18);
    vector<int> channel(Core + 1);
    vector<int> Task_end(Task);
    vector<int> Task_begin(Task);
    vector<int> C_1_time = { 0 }, C_2_time = { 0 }, C_3_time = { 0 }, W_S_time = { 0 }, Cloud_time = { 0 }, W_R_time = { 0 };
	//int FT(0);				
	for (int m = 0; m < Task; m++)
	{
		if (S_new[m] != 0)//not cloud
		{
			vector<float>E(Core+1), T(Core+1);
			vector<vector <int> >S_choice;
			S_choice.resize(Core+1, vector<int>(Task));
			for (int n = 0; n < Core + 1; n++)
			{
				float energy(0);
				if (n != S_new[m])
				{                    
					int current = 0,T_total(0);
					queue<int> exec = execution_order;
					tmp_s = S_new[m];
					S_new[m] = n;
					for (int i = 0; i < Task; i++)
					{
						current = exec.front();
						channel[0] = Send_Time + Compute_Time + Receive_Time;
						channel[1] = Local_Task[current][0];
						channel[2] = Local_Task[current][1];
						channel[3] = Local_Task[current][2];
						if (S_new[current] == 0)// cloud task
						{
							vector<int> ws_predecessor = { 0 }, c_predecessor = { 0 };
							for (int j = 0; j < Task; j++)
							{
								if (matrix_adj[j][current] == 1)
								{
									if (S_new[j] == 0)
									{
										ws_predecessor.push_back(W_S_time.back());
										c_predecessor.push_back(Cloud_time.back());
									}
									else ws_predecessor.push_back(Task_end[j]);
								}
							}
							int RT_ws = highest(ws_predecessor);
							W_S_time.push_back(RT_ws + Send_Time);
							c_predecessor.push_back(RT_ws + Send_Time);
							int RT_c = highest(c_predecessor);
							Cloud_time.push_back(RT_c + Compute_Time);
							W_R_time.push_back(Cloud_time.back() + Receive_Time);
							Task_end[current] = W_R_time.back();
                            Task_begin[current] = W_S_time.back();
							energy += (float)Send_Power*(float)Send_Time;
						}
						else if(S_new[current] == 1)// set task to core1
						{
							vector<int> predecessor = { 0 };
							int FT(0);
							for (int j = 0; j < Task; j++)
								if (matrix_adj[j][current] == 1)
									predecessor.push_back(Task_end[j]);
							int RT_l = highest(predecessor);
							RT_l > C_1_time.back() ? FT = RT_l + channel[1] : FT = C_1_time.back() + channel[1];
							C_1_time.push_back(FT);
							Task_end[current] = C_1_time.back();
                            Task_begin[current]=RT_l;
							energy += (float)C_P_1*(float)channel[1];
						}
						else if (S_new[current] == 2)// set task to core2
						{
							vector<int> predecessor = { 0 };
							int FT(0);
							for (int j = 0; j < Task; j++)
								if (matrix_adj[j][current] == 1)
									predecessor.push_back(Task_end[j]);
							int RT_l = highest(predecessor);
							RT_l > C_2_time.back() ? FT = RT_l + channel[2] : FT = C_2_time.back() + channel[2];
							C_2_time.push_back(FT);
							Task_end[current] = C_2_time.back();
                            Task_begin[current]=RT_l;
							energy += (float)C_P_2*(float)channel[2];
						}
						else if (S_new[current] == 3)// set task to core3
						{
							vector<int> predecessor = { 0 };
							int FT(0);
							for (int j = 0; j < Task; j++)
								if (matrix_adj[j][current] == 1)
									predecessor.push_back(Task_end[j]);
							int RT_l = highest(predecessor);
							RT_l > C_3_time.back() ? FT = RT_l + channel[3] : FT = C_3_time.back() + channel[3];
							C_3_time.push_back(FT);
							Task_end[current] = C_3_time.back();
                            Task_begin[current]=RT_l;
							energy += (float)C_P_3*(float)channel[3];
						}
                        
						temp_time=highest(Task_end);
						if (temp_time > T_max)
							break;
						exec.pop();//clear the exec queue
					}
					int test(0);

                     
				}
				T[n] = temp_time;
                //cout << temp_time;
				E[n] = energy;
				S_choice[n] = S_new;
			}

			int selected(0);
			for (int k = 0; k < Core+1; k++)
			{
				if (T[k] <= optimal_T&& E[k] < optimal_E)
				{	
					S_new= S_choice[k];
					result = S_choice[k];
					optimal_T = T[k];
                    optimal_E = E[k];
					selected = 1;
				}
			}
			if (selected == 0)
			{
				for (int k = 0; k < Core+1; k++)
				{
					if (T[k] <= T_max&& E[k] < optimal_E)
					{
						S_new = S_choice[k];
						result = S_choice[k];
						optimal_T = T[k];
                        optimal_E = E[k];
					}
				}
			}
        } 
         
	}
	cout << "Total Time of Task Migration: " << optimal_T << endl;
    cout << "Total Energy of Task Migration: " << optimal_E << endl;
    cout << "Start Time of Task Migration: ";
    print(Task_begin);
    cout << "End Time of Task Migration: " ;
    print(Task_end);
	return result;
}
int main()
{
	vector<vector <int> >matrix_adj = { { 0, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
	                                    { 0, 0, 0, 0, 0, 0, 0, 1, 1, 0 },
	                                    { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
	                                    { 0, 0, 0, 0, 0, 0, 0, 1, 1, 0 },
	                                    { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
	                                    { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
	                                    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	                                    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	                                    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	                                    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
	/*
	vector<vector <int> >matrix_adj = {
		{ 0, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 1, 1, 0 },
		{ 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
		{ 0, 0, 0, 0, 0, 0, 1, 1, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
		*/
	vector<vector <int> >Local_Task = {
		{ 9,7,5 },
		{ 8,6,5 },
		{ 6,5,4 },
		{ 7,5,3 },
		{ 5,4,2 },
		{ 7,6,4 },
		{ 8,5,3 },
		{ 6,4,2 },
		{ 5,3,2 },  
		{ 7,4,2 },
	}; 
	
    auto start = high_resolution_clock::now();
	vector<int> x = Initial_Scheduling(matrix_adj, Local_Task);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
	cout << "After initial scheduling: ";
	print(x);
	cout<<"Run time of Initial Scheduling: "<< duration.count() << " microseconds" << endl;
    auto start1 = high_resolution_clock::now();
	vector<int> y = Task_Migration(matrix_adj, Local_Task, x, 25);
    auto stop1 = high_resolution_clock::now();
    auto duration1 = duration_cast<microseconds>(stop1 - start1);
	cout << "After Task Migration: ";
	print(y);
	cout << "Running Time of Task Migration: " << duration1.count() << " microseconds" << endl;
}
