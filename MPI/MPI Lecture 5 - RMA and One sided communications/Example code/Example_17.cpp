#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <time.h>

using namespace std;

//This example is equivalent to a scatter
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

	//MPI_Win_allocate both allocates the memory associated with this data and creates the associated window
	//Doing a Scatter with puts the shared data is the receive. It is the size of a single chunk, but is on every process
	MPI_Win_allocate(sizeof(int) * chunk_size, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &recv_data, &window);

	//Start by storing data in the send_data memory on processor zero - no need to do any special communications even if shared as this is on the same process
	if (id == 0)
	{
		//The send data need not be shared
		send_data = new int[p * chunk_size];
		//while I could have used a single loop, I have split it between an outer loop for each process and an inner loop for the data beloning to a given process
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

	//Fence starts/stops a set of communications - nearly like an MPI_Barrier, but can let processes through if they are no longer involved in communications
	MPI_Win_fence(0, window);

	if (id == 0)
	{
		//Note that these communications are safe to do together as each MPI_Put is addressing memory on a different process 
		for (int i = 0; i < p; i++)
			MPI_Put(&send_data[i * chunk_size], chunk_size, MPI_INT, i, 0, chunk_size, MPI_INT, window);
	}

	//Ensures that all the previous Puts have completed
	MPI_Win_fence(0, window);

	cout << "Data on process " << id << ": ";
	for (int n = 0; n < chunk_size; n++)
		cout << recv_data[n] << "\t";
	cout << endl;

	//This also frees the attached memory if it was created using MPI_Win_allocate
	MPI_Win_free(&window);		

	//Free the memory not created with the window
	delete[] send_data;
	MPI_Finalize();
}
