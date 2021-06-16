#include <mpi.h>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <time.h>
#include <chrono>

#define DO_TIMING

using namespace std;
using chrono::duration;

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
    //row_end = new int[p];
    
	int rows_left = i_max;
	int rows_assigned;

	row_start[0] = 0;

	for (int n = 0; n < p - 1; ++n)
	{   

        rows_assigned = rows_left / (p - n);
		rows_left -= rows_assigned;
		row_start[n + 1] = row_start[n] + rows_assigned;
		process_chunk[n] = rows_assigned;
	}	

	process_chunk[p - 1] = i_max - row_start[p - 1];
    
	/*if (id == 0)
    {
	for (int i = 0; i < p; ++i)
	    cout << "process " << i << ": start " << row_start[i] << " " << process_chunk[i] << endl;
	}*/
}

void send_matrix(double *data, int m, int n, int row_start, int num_rows, int dest, MPI_Request *request, int &cnt)
{
     MPI_Isend(&data[row_start * n], num_rows * n, MPI_DOUBLE, dest, tag_num, MPI_COMM_WORLD, &request[cnt]);
     cnt++;
}

void recv_matrix(double *data, int m, int n, int row_start, int num_rows, int source, MPI_Request *request, int &cnt)
{
     MPI_Irecv(&data[row_start * n], num_rows * n, MPI_DOUBLE, source, tag_num, MPI_COMM_WORLD, &request[cnt]);
     cnt++;
}

int main(int argc, char* argv[])
{   
#ifdef DO_TIMING
    auto start = chrono::high_resolution_clock::now();
#endif

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL) + id * 10);

    setup_partition();

	MPI_Request* requests;
	int cnt = 0;

    if (id == 0)
	{
	setup_continuous_array(A_2d, A_1d, i_max, k_max);
	setup_continuous_array(B_2d, B_1d, k_max, j_max);
	setup_continuous_array(C_2d, C_1d, i_max, j_max);

	for (int i = 0; i < i_max * k_max; i++)
		A_1d[i] = (double)i;
	for (int i = 0; i < k_max * j_max; i++)
		B_1d[i] = (double)(2 * i);

		/*cout << "A: " << endl;
		for (int i = 0; i < i_max; ++i)
		{
			for (int k = 0; k < k_max; ++k)
			{
				cout << A_2d[i][k] << " ";
			}
			cout << endl;
		}
		cout << endl;

		cout << "B: " << endl;
		for (int k = 0; k < k_max; ++k)
		{
			for (int j = 0; j < j_max; ++j)
			{
				cout << B_2d[k][j] << " ";
			}
			cout << endl;
		}
		cout << endl;*/

	requests = new MPI_Request[2 * p - 1];

	for (int i = 1; i < p; ++i)
    {
        send_matrix(A_1d, i_max, k_max, row_start[i], process_chunk[i], i ,requests, cnt);
        recv_matrix(C_1d, i_max, j_max, row_start[i], process_chunk[i], i ,requests, cnt);
	}

	MPI_Ibcast(B_1d, k_max * j_max, MPI_DOUBLE, 0, MPI_COMM_WORLD, &requests[cnt]);
	cnt++;
	//cout << "process " << id << "cnt: " << cnt;
	}

	else
	{
	setup_continuous_array(A_2d, A_1d, process_chunk[id], k_max);
	setup_continuous_array(B_2d, B_1d, k_max, j_max);
	setup_continuous_array(C_2d, C_1d, process_chunk[id], j_max);

	requests = new MPI_Request[2];

	recv_matrix(A_1d, process_chunk[id], k_max, 0, process_chunk[id], 0, requests, cnt);

	MPI_Ibcast(B_1d, k_max * j_max, MPI_DOUBLE, 0, MPI_COMM_WORLD, &requests[cnt]);
	cnt++;

	MPI_Waitall(cnt, requests, MPI_STATUS_IGNORE);

    /*cout << "processer " << id << " get " << "B: ";
	for (int k = 0; k< k_max;k++)
	{
		for (int j = 0; j < j_max; j++)
		{
		    cout << B_2d[k][j] << " ";
		}
		cout << endl;
	}*/

    //cout << "processer " << id << " get " << "A: ";
	/*for (int k = 0; k< process_chunk[id];k++)
	{
		for (int j = 0; j < k_max; j++)
		{
		    cout << A_2d[k][j] << " ";
		}
		cout << endl;
	}*/

    
	
	}



	for (int i = 0; i< process_chunk[id];i++)
	{
		for (int j = 0; j < j_max; j++)
		{
			C_2d[i][j] = 0.0;
			for (int k = 0; k < k_max; k++)
			{	
			//cout << "processer " << id << " get " << "A[" << row_start[id] + i <<"][" << k <<"] is " << A_2d[k][j] << endl;			
			//cout << "processer " << id << " get " << "B[" << k <<"][" << j <<"] is " << B_2d[k][j] << endl;
			C_2d[i][j] += A_2d[i][k] * B_2d[k][j];
				//C_1d[i * j_max + j] += A_1d[i * k_max + k] * B_1d[k * j_max + j];
			}
			
			//cout << "processer " << id << " computed " << "C[" << row_start[id] + i <<"][" << j <<"]: " <<  C_2d[i][j] << endl;
		}
	}

	if (id == 0)
	{
		MPI_Waitall(cnt, requests, MPI_STATUS_IGNORE);
        
		/*cout << "C: " << endl;
		for (int i = 0; i < i_max; ++i)
		{
			for (int j = 0; j < j_max; ++j)
			{
				cout << C_2d[i][j] << " ";
				//cout << C_1d[i * j_max + j] << " ";
			}
			cout << endl;
		}
		cout << endl;*/

	}
	else{
		//cnt = 0;
		//MPI_Send(C_1d, process_chunk[id]*j_max, MPI_DOUBLE, 0, tag_num, MPI_COMM_WORLD);
		cnt = 0;
        send_matrix(C_1d, process_chunk[id], j_max, 0, process_chunk[id], 0 ,requests, cnt);				
	}

#ifdef DO_TIMING
      MPI_Barrier(MPI_COMM_WORLD);
	  auto finish = chrono::high_resolution_clock::now();
	  if(id ==0)
	  {   
		  //chrono::
		  //cout << setprecision(6)
		  duration<double> time_span = chrono::duration_cast<duration<double>>(finish - start);  
		  cout << "The process takes " << time_span.count() << " sec with " << p << " processer" << endl;
	  }    
#endif


	free_continuous_array(A_2d, A_1d);
	free_continuous_array(B_2d, B_1d);
	free_continuous_array(C_2d, C_1d);

	MPI_Finalize();
}