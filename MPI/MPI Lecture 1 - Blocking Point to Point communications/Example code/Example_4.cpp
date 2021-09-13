#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <time.h>

using namespace std;

int id, p;
int tag_num = 1;


//Function sends random data to a random other processor 
void Send_Random_Data(void)
{
	int to_proc;
	while ((to_proc = rand() % p) == id);	//Stop code sending to itself
	//create a random number between 0 and 1000
	int send_data = (rand()%1000);

	MPI_Send(&send_data, 1, MPI_INT, to_proc, tag_num, MPI_COMM_WORLD);

	cout << "Processor " << id << " sent data to processor " << to_proc << endl;
	cout.flush();
}


//Function probes to receive data from any process. Function will return -1 if a negative number is received
bool Recv_Data(void)
{
	int from_proc;
	int recv_data;
	MPI_Status status;

	while (MPI_Probe(MPI_ANY_SOURCE, tag_num, MPI_COMM_WORLD, &status) != MPI_SUCCESS)
		cout << "Communication timed out or failed! " << id << endl;

	from_proc = status.MPI_SOURCE;

	MPI_Recv(&recv_data, 1, MPI_INT, from_proc, tag_num, MPI_COMM_WORLD, &status);

	//if a negative number is received the code needs to exit
	if (recv_data == -1)
		return false;

	cout << "Processor " << id << " received data from processor " << from_proc << endl;
	cout.flush();
	
	return true;
}

int main(int argc, char *argv[])
{
	int recv_cnt = 0;
	bool status = true;
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL) + id * 10);

	if (id == 0) Send_Random_Data();			//Starts the ping-pong

	while (status)
	{
		status = Recv_Data();					//Waits for and receives data
		recv_cnt++;
		if (recv_cnt<10 && status) 
			Send_Random_Data();					//Sends it on to a new processor
		else break;								//Exits if 10 communications are received or a negative number is received
	}

	if (status)			//process has left by breaking after 10 communications
	{
		cout << endl << "Process " << id << " has received 10 communications and is exiting" << endl;
		cout.flush();
		for (int i = 0; i < p; i++)
			if (i!=id)
			{
				int send_data = -1;			//send a negative numbers to each of the other processors to tell them to exit
				MPI_Send(&send_data, 1, MPI_INT, i, tag_num, MPI_COMM_WORLD);
			}		
	}

	MPI_Finalize();
	return 0;
}