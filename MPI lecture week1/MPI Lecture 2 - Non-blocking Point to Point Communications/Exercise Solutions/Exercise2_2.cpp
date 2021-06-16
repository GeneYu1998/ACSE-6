#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <time.h>

using namespace std;

int id, p;

void Do_Work(void)	 //Some (not very useful) work
{
	int sum = 0;
	for (int i = 0; i < 100; i++) sum+= 100;
}


int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL) + id * 10);

	int tag_num = 1;

	int sent_num = 100;

	MPI_Request* request = new MPI_Request[(p - 1) * 2];
	double **send_data = new double*[p];
	double **recv_data = new double*[p];

	for (int i = 0; i < p; i++)
	{
		send_data[i] = new double[sent_num];
		recv_data[i] = new double[sent_num];
		for (int j = 0; j < sent_num; j++)
			send_data[i][j] = id;
	}

	int flag = 0;
	int cnt = 0;

	for (int i = 0; i<p; i++)
		if (i != id)
		{
			MPI_Irecv(recv_data[i], sent_num, MPI_DOUBLE, i, tag_num, MPI_COMM_WORLD, &request[cnt]);
			cnt++;

			MPI_Isend(send_data[i], sent_num, MPI_DOUBLE, i, tag_num, MPI_COMM_WORLD, &request[cnt]);
			cnt++;
		}
		//else
		//{
			//for (int j = 0; j < sent_num; j++)
				//recv_data[i][j] = send_data[i][j];
		//}


	int work_cnt = 0;
	while (MPI_Testall(10, request, &flag, MPI_STATUS_IGNORE) == MPI_SUCCESS && flag == 0)
	{
		Do_Work();
		work_cnt++;
	}

	cout << "Process " << id << " did " << work_cnt << " cycles of work while waiting " << endl;
	cout.flush();

	for (int i = 0; i < p; i++)
	{   
		if (i !=id)
		{
			cout << "Process " << id << " recieve ";
		    for (int j = 0; j < sent_num; ++j)
		         cout << recv_data[i][j] << " ";

		    cout << "from processer " << i << endl;
	     }
	}

	for (int i = 0; i < p; i++)
	{
		delete[] send_data[i];
		delete[] recv_data[i];
	}

	delete[] request;
	delete[] recv_data;
	delete[] send_data;

	MPI_Finalize();
}