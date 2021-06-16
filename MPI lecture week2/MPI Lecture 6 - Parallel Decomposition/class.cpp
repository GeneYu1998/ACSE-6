#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <time.h>


using namespace std;

int id, p;

//A * B = C			//A -> i_max by k_max   B -> k_max by j_max    C -> i_max by j_max

int i_max = 100, j_max = 200, k_max = 150;
double** A_2d, * A_1d;
double** B_2d, * B_1d;
double** C_2d, * C_1d;


void setup_continuous_array(double**& array_2d, double*& array_1d, int m, int n)
{
	array_1d = new double[m * n];
	array_2d = new double* [m];

	for (int i = 0; i < m; i++)
		array_2d[i] = &array_1d[i * n];						//index = i*n + j
}

void free_continuous_array(double**& array_2d, double*& array_1d)
{
	delete[] array_1d;
	delete[] array_2d;
}



int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL) + id * 10);


	setup_continuous_array(A_2d, A_1d, i_max, k_max);
	setup_continuous_array(B_2d, B_1d, k_max, j_max);
	setup_continuous_array(C_2d, C_1d, i_max, j_max);

	for (int i = 0; i < i_max * k_max; i++)
		A_1d[i] = ((double)rand()) / ((double)RAND_MAX) * 10.0;
	for (int i = 0; i < k_max * j_max; i++)
		B_1d[i] = ((double)rand()) / ((double)RAND_MAX) * 10.0;

	for (int i = 0; i<i_max;i++)
		for (int j = 0; j < j_max; j++)
		{
			C_2d[i][j] = 0.0;
			for (int k = 0; k < k_max; k++)
				C_2d[i][j] += A_2d[i][k] * B_2d[k][j];
				//C_1d[i * j_max + j] += A_1d[i * k_max + k] * B_1d[k * j_max + j];
		}

	free_continuous_array(A_2d, A_1d);
	free_continuous_array(B_2d, B_1d);
	free_continuous_array(C_2d, C_1d);

	MPI_Finalize();
}