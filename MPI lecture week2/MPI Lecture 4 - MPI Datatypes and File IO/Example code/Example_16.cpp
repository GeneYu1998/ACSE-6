#include <mpi.h>
#include <iostream>
#include <string>

using namespace std;

int id, p;
int tag_num = 1;

int main(int argc, char* argv[])
{
	int recv_cnt = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	MPI_File mpi_file;

	string fname("test.txt");


	if (MPI_File_open(MPI_COMM_WORLD, fname.c_str(), MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &mpi_file) != MPI_SUCCESS)
	{
		cout << "Error opening file!" << endl;
		exit(0);
	}
	
	string string_out;

	string_out = string("This data has been written by ") + to_string(id) + string("\n");


	MPI_File_write_ordered(mpi_file, string_out.c_str(), (int)string_out.length(), MPI_CHAR, MPI_STATUS_IGNORE);
	MPI_File_close(&mpi_file);

	MPI_Finalize();
}