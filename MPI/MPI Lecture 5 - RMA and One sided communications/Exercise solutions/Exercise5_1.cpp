#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <time.h>

using namespace std;

//This example is equivalent to a gather
int id, p;

const int chunk_size = 5;

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL) + id * 10);

	int* send_data = nullptr;			
	int* recv_data = nullptr;			
	MPI_Win window;	

	MPI_Win_allocate(sizeof(int) * chunk_size, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &send_data, &window);

	if (id == 0)
	{
		recv_data = new int[p * chunk_size];
	}

	cout << "Data on process " << id << ": ";
	for (int n = 0; n < chunk_size; n++)
	{
		send_data[n] = rand() % 1000;
		cout << send_data[n] << "\t";
	}
	cout.flush();

	MPI_Win_fence(0, window);

	if (id == 0)
	{
		for (int i = 0; i < p; i++)
			MPI_Get(&recv_data[i * chunk_size], chunk_size, MPI_INT, i, 0, chunk_size, MPI_INT, window);
	}

	MPI_Win_fence(0, window);

	if (id == 0)
	{
		for (int i = 0; i < p; i++)
		{
			cout << "Data from process " << i << ": ";
			for (int n = 0; n < chunk_size; n++)
				cout << recv_data[i * chunk_size + n] << "\t";
			cout << endl;
		}
	}

	MPI_Win_free(&window);		

	delete[] recv_data;
	MPI_Finalize();
}
