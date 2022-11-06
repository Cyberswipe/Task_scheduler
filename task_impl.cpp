########

/*Author : Aditya Mukundan
Created : 11/05/2021
*/
###########

#include <iostream>
using namespace std;
 
int **minElement = NULL;
int **minVal = NULL;
int *groupArray = NULL;
int *grpSumArray = NULL;
 
//Generate the initial row
void initRowGen(int array[], int **minElement, int length)
{
   int presentSum = 0;
   for(int i = 0; i < length; i++)
   {
       minElement[0][i] = presentSum + array[i];
       presentSum = minElement[0][i];
   }
}
 
//Calculate the sum of row elements
int sumGenerator(int *pArr, int low , int high)
{
   int sum = 0;
   for (int z = low; z <= high; z++)
   {
       sum += pArr[z];
   }
   return sum;
}
 
//Initialize memory to elements
int initMemory(int *pArr, int rowNum, int colNum)
{
    minElement = new int*[rowNum];
    minVal = new int*[rowNum];
    groupArray = new int [rowNum];
    grpSumArray = new int [rowNum];

   for (int l = 0; l < rowNum; l++)
       {
           minElement[l] = new int [colNum];
           minVal[l] = new int [colNum];
       }
      
       for (int j = 0; j < rowNum; j++)
       {
           for (int i = 0; i < colNum; i++)
           {
               minElement[j][i] = 0;
               minVal[j][i] = 0;
           }
       }
      
       groupArray[rowNum] = {0};
       grpSumArray[rowNum] = {0};
       
       initRowGen(pArr, minElement, colNum);
 
       return 0;
   }
 

void sumArrayGen(int *mainArray, int rowNum)
{
   int lowBond = 0;
   for(int pos = 0; pos < rowNum; pos++)
   {
       grpSumArray[pos] = sumGenerator(mainArray, lowBond, (lowBond + groupArray[pos] - 1));
       lowBond += groupArray[pos];
   }
}
 
//Main method to generate the algorithm using Dynamic programming concept
int* Max_Min_grouping(int *mainArray, int colNum, int rowNum)
{
   int sum = 0;
   int gSum = 0;
   int min = 0;
   int col = colNum-1;
   int i,j;
    
   for (j = rowNum-1; j >=0; j--)
   {
       for (i = colNum-1; i >= j; i--)
       {
           if (j > 0)
           {
               for (int k = j-1; k < i; k++)
               {
                   groupArray = Max_Min_grouping(mainArray, colNum-groupArray[rowNum], rowNum-1);
                   sum = sumGenerator(mainArray, (k+1), i);
                   min = minElement[j-1][k] < sum ? minElement[j-1][k] : sum;
                   if (min > minElement[j][i])
                   {
                       minElement[j][i] = min;
                       minVal[j][i] = k;
                   }
               }
           }
       }
       if(j != 0)
       {
           groupArray[j] =  col - minVal[j][col];
           col = minVal[j][col];
           gSum += groupArray[j];
       }
       else
       {
           groupArray[j] =  colNum - gSum;
       }
    }
 
   return groupArray;
}

void print2dArray(int **array, int row, int column)
{
   for (int p = 0; p < row; p++)
   {
       for (int q = 0; q < column; q++)
       {
           cout << array[p][q] << " ";
       }
       cout <<  "\n" << endl;
   }
}

 void printArray(int array[], int arrLen)
{
   for (int i = 0; i < arrLen; i++)
       cout << array[i] << " ";
   cout << "\n" << endl;
}

//Driver 
int main()
{
   //int mainArray[] = {3, 9, 7, 8, 2, 6, 5, 10, 1, 7, 6, 4};
   int colNum = 0;
   int rowNum = 0;
   int length = 0;
  
   cout << "\nEnter Length of the input array(A) : " << endl;
   cin >> colNum;
    
   int *mainArray = new int [colNum];
    
   cout << "\nEnter the input array(A) : " << endl;
   while(length < colNum)
   {
       cin >> mainArray[length];
       length ++;
   }
  
   cout << "\nEnter Length of the grouping & summation array : " << endl;
   cin >> rowNum;
    
   initMemory(mainArray, rowNum, colNum);   
   Max_Min_grouping(mainArray, colNum, rowNum);
  
   //Minimum element array of min array
   cout << "\nMinimimum Array elements: " << endl;
   print2dArray(minElement, rowNum, colNum);
 
   cout << "\nMin Array : " << endl;
   print2dArray(minVal, rowNum, colNum);
 
   //Optimal Solution Array
   cout << "\nOptimal Solution Array : " << endl;
   printArray(groupArray, rowNum);
 
   // Group Summation Array
   sumArrayGen(mainArray, rowNum);
   cout << "\nGroup Summation Array : " << endl;
   printArray(grpSumArray, rowNum);
 
   return 0;
}