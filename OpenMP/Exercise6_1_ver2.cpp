#include <mpi.h>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <chrono>

#define DO_TIMING

using namespace std;

int id, p;

//A * B = C			//A -> i_max by k_max   B -> k_max by j_max    C -> i_max by j_max

int i_max = 1000, j_max = 2000, k_max = 1500;
double** A_2d, * A_1d;
double** B_2d, * B_1d;
double** C_2d, * C_1d;

int* row_start, *process_chunk;

int tag_num = 1;


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


void setup_partition()
{
	row_start = new int[p];
	process_chunk = new int[p];

	int rows_left = i_max;
	row_start[0] = 0;
	for (int n = 0; n < p-1; n++)
	{
		int rows_assigned = rows_left / (p - n);
		rows_left -= rows_assigned;
		row_start[n + 1] = row_start[n] + rows_assigned;
		process_chunk[n] = rows_assigned;
	}
	process_chunk[p - 1] = i_max - row_start[p - 1];

#ifndef DO_TIMING
	if (id == 0)
	{
		for (int i = 0; i < p; i++)
			cout << "process " << i << ": start " << row_start[i] << " " << process_chunk[i] << endl;
	}
#endif
}


void send_matrix(double* data, int m, int n, int row_start, int num_rows, int dest, MPI_Request* requests, int& cnt)
{
	MPI_Isend(&data[row_start * n], num_rows * n, MPI_DOUBLE, dest, tag_num, MPI_COMM_WORLD, &requests[cnt]);
	cnt++;
}

void recv_matrix(double* data, int m, int n, int row_start, int num_rows, int source, MPI_Request* requests, int& cnt)
{
	MPI_Irecv(&data[row_start * n], num_rows * n, MPI_DOUBLE, source, tag_num, MPI_COMM_WORLD, &requests[cnt]);
	cnt++;
}

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

#ifdef DO_TIMING
	MPI_Barrier(MPI_COMM_WORLD);
	auto start = chrono::high_resolution_clock::now();
#endif

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL) + id * 10);

	setup_partition();

	MPI_Request* requests = nullptr;
	int cnt = 0;

	if (id == 0)
	{
		setup_continuous_array(A_2d, A_1d, i_max, k_max);
		setup_continuous_array(B_2d, B_1d, k_max, j_max);
		setup_continuous_array(C_2d, C_1d, i_max, j_max);

		for (int i = 0; i < i_max * k_max; i++)
			A_1d[i] = ((double)rand()) / ((double)RAND_MAX) * 10.0;
		for (int i = 0; i < k_max * j_max; i++)
			B_1d[i] = ((double)rand()) / ((double)RAND_MAX) * 10.0;

		requests = new MPI_Request[3 * (p - 1)];`

		for (int i = 1; i < p; i++)
		{
			send_matrix(A_1d, i_max, k_max, row_start[i], process_chunk[i], i, requests, cnt);
			send_matrix(B_1d, k_max, j_max, 0, k_max, i, requests, cnt);
			recv_matrix(C_1d, i_max, j_max, row_start[i], process_chunk[i], i, requests, cnt);
		}

		cnt++;
	}
	else
	{
		setup_continuous_array(A_2d, A_1d, process_chunk[id], k_max);
		setup_continuous_array(B_2d, B_1d, k_max, j_max);
		setup_continuous_array(C_2d, C_1d, process_chunk[id], j_max);

		requests = new MPI_Request[2];
		int cnt = 0;

		recv_matrix(A_1d, process_chunk[id], k_max, 0, process_chunk[id], 0, requests, cnt);
		recv_matrix(B_1d, k_max, j_max, 0, k_max, 0, requests, cnt);
		cnt++;
		MPI_Waitall(cnt, requests, MPI_STATUSES_IGNORE);
	}


	for (int i = 0; i< process_chunk[id]; i++)
		for (int j = 0; j < j_max; j++)
		{
			C_2d[i][j] = 0.0;
			for (int k = 0; k < k_max; k++)
				C_2d[i][j] += A_2d[i][k] * B_2d[k][j];
		}

	if (id == 0)
	{
		MPI_Waitall(cnt, requests, MPI_STATUSES_IGNORE);
	}
	else
	{
		cnt = 0;
		MPI_Send(C_1d, process_chunk[id]*j_max, MPI_DOUBLE, 0, tag_num, MPI_COMM_WORLD);
	}

#ifdef DO_TIMING
	MPI_Barrier(MPI_COMM_WORLD);
	auto finish = chrono::high_resolution_clock::now();
	if (id == 0)
	{
		std::chrono::duration<double> elapsed = finish - start;
		cout << setprecision(5);
		cout << "The code took " << elapsed.count() << "s to run" << endl;
	}
#endif

	free_continuous_array(A_2d, A_1d);
	free_continuous_array(B_2d, B_1d);
	free_continuous_array(C_2d, C_1d);

	MPI_Finalize();
}