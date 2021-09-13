#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <time.h>

using namespace std;

//This example is again equivalent to a scatter
int id, p;

const int chunk_size = 5;

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL) + id * 10);

	int* send_data = nullptr;			//pointer which will contain the data to send
	int* recv_data = nullptr;			//pointer which will contain the data to receive
	MPI_Win window;				//struct in which the infow for a given memory window will be stored

	//only process zero needs remotely accessible memory in this example
	if (id == 0)
		MPI_Win_allocate(sizeof(int) * p * chunk_size, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &send_data, &window);
	else
		//We still need a window on each of the other processes to control access (set fences, specify the displacement size etc.), but it can be empty
		MPI_Win_allocate(0, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &send_data, &window);

	recv_data = new int[chunk_size];

	
	if (id == 0)
	{
		for (int i = 0; i < p; i++)
		{
			cout << "Data for process " << i << ": ";
			for (int n = 0; n < chunk_size; n++)
			{
				send_data[i * chunk_size + n] = rand() % 1000;
				cout << send_data[i * chunk_size + n] << "\t";
			}
			cout << endl;
		}
	}

	MPI_Win_fence(MPI_MODE_NOPRECEDE, window);

	//Each process uses an MPI_Get to read data off process 0 - Note that it is fine to do this on process zero itself
	MPI_Get(recv_data, chunk_size, MPI_INT, 0, id * chunk_size, chunk_size, MPI_INT, window);

	MPI_Win_fence(MPI_MODE_NOSUCCEED, window);

	cout << "Data on process " << id << ": ";
	for (int n = 0; n < chunk_size; n++)
		cout << recv_data[n] << "\t";
	cout << endl;

	MPI_Win_free(&window);		

	delete[] recv_data;

	MPI_Finalize();
}
