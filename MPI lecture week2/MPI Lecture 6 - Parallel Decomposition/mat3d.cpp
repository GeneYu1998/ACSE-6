#include <iostream>
#include <cstdlib>
#include <time.h>

using namespace std;



void setup_continuous_array_3d(double***& array_3d, double*& array_1d, int m, int n, int o)
{
    array_1d = new double[m * n * o];
    array_3d = new double** [m];

    for (int i = 0; i < m; i++)
    {
    array_3d[i] = new double* [n]; 
    
    for (int j = 0; j < n; j++)    
        array_3d[i][j] = &array_1d[i * n * o + j * o]; //index = i*n*o + j*o + k
    }

}

//from index to ijk
//i = index / (j_max * k_max);
//j = (index % (j_max * k_max)) / k_max;
//k = index % k_max;
