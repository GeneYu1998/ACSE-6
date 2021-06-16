#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <stdlib.h>
#include <vector>
#include <cmath>

using namespace std;

vector<double> list;

const int size = 10;

int partition_into_2(double* list, int bottom, int top)
{
    double  pivot = list[top];
    int  pivot_loc = bottom;
    double temp;

    for (int j = bottom; j < top; j++)
    {
        if(list[j] <= pivot)
        {
            temp = list[pivot_loc];
            list[pivot_loc] = list[j];
            list[j] = temp;
            ++pivot_loc;
        }
    }
    
    temp = list[pivot_loc];
    list[pivot_loc] = list[top];
    list[top] = temp;

    return pivot_loc;
}

void quick_sort(double* list, int bottom, int top)
{    
     int pivot;

     if(top > bottom)
     {
        pivot = partition_into_2(list, bottom, top);

        quick_sort(list, bottom, pivot - 1);
        quick_sort(list, pivot + 1, top);         

     }
}


int main(int argc, char* argv[])
{
    srand(time(NULL));
    list.resize(size);
    cout << "List in : ";
    for (int i = 0; i < size; ++i)
    {
         list[i] = ((double) rand() / (double)RAND_MAX) * 100;
         cout << list[i] << " ";
    }

    cout << endl;

    quick_sort(list.data(), 0, size - 1);

    cout << "List out : ";
    for (int i = 0; i < size; ++i)
    {
         cout << list[i] << " ";
    }

    cout << endl;    

    
    

    
}