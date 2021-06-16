#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <time.h>

using namespace std;

//This example is again equivalent to a scatter - PSWC version of Example 17
int id, p;

MPI_Group comm_group;			//A base group
MPI_Group group;				//Group for the specific communication/s
int *group_ids = nullptr;		//Ids of the processes involved in the communication

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

	if (id == 0)
		MPI_Win_allocate(sizeof(int) * p * chunk_size, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &send_data, &window);
	else
		MPI_Win_create(nullptr, 0, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &window);

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

	MPI_Comm_group(MPI_COMM_WORLD, &comm_group);		//Setups up a base group

	if (id != 0)
	{
		//These are the origin processes
		group_ids = new int;
		group_ids[0] = 0;
		MPI_Group_incl(comm_group, 1, group_ids, &group);
		MPI_Win_start(group, 0, window);
		MPI_Get(recv_data, chunk_size, MPI_INT, 0, id * chunk_size, chunk_size, MPI_INT, window);
		MPI_Win_complete(window);
	}
	else
	{
		//This is the target process
		group_ids = new int[p];
		for (int n = 1; n < p; n++)
			group_ids[n - 1] = n;
		MPI_Group_incl(comm_group, p - 1, group_ids, &group);

		MPI_Win_post(group, 0, window);
		//doing a simple copy on process zero - I am doing this inside the post - wait to give process zero at least a little bit to do while the communications are completing
		for (int n = 0; n < chunk_size; n++)
			recv_data[n] = send_data[n];
		MPI_Win_wait(window);
	}

	cout << "Data on process " << id << ": ";
	for (int n = 0; n < chunk_size; n++)
		cout << recv_data[n] << "\t";
	cout << endl;

	MPI_Win_free(&window);		

	delete[] recv_data;
	delete[] group_ids;

	MPI_Finalize();
}
