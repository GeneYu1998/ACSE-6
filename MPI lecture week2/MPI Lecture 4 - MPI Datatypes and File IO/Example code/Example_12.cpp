#include <mpi.h>
#include <iostream>
#include <locale>

using namespace std;

int id, p;

class my_class
{
public:
	int I1, I2;
	int var_not_to_send;
	double D1;
	char S1[50];

    void buildMPIType();
	//MPI_Datatype MPI_type;
};

MPI_Datatype MPI_type;

// MPI_Datatype my_class::MPI_type;

void my_class::buildMPIType()
{
	int block_lengths[4];
	MPI_Aint displacements[4];
	MPI_Aint addresses[4], add_start;
	MPI_Datatype typelist[4];

	my_class temp;

	typelist[0] = MPI_INT;
	block_lengths[0] = 1;
	MPI_Get_address(&temp.I1, &addresses[0]);

	typelist[1] = MPI_INT;
	block_lengths[1] = 1;
	MPI_Get_address(&temp.I2, &addresses[1]);

	typelist[2] = MPI_DOUBLE;
	block_lengths[2] = 1;
	MPI_Get_address(&temp.D1, &addresses[2]);

	typelist[3] = MPI_CHAR;
	block_lengths[3] = 50;
	MPI_Get_address(&temp.S1, &addresses[3]);

	MPI_Get_address(&temp, &add_start);
	for (int i = 0; i < 4; i++) displacements[i] = addresses[i] - add_start;

	MPI_Type_create_struct(4, block_lengths, displacements, typelist, &MPI_type);
	MPI_Type_commit(&MPI_type);
}

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);


	my_class *data=new my_class[10];
    
	data[0].buildMPIType();

	if (id == 0)
	{
		for (int i = 0; i < 10; i++)
		{
			data[i].I1 = 6;
			data[i].I2 = 3;
			data[i].D1 = 10.0;
			data[i].var_not_to_send = 25;
			strncpy(data[i].S1, "My test string", 50);
		}
	}


	MPI_Bcast(data, 1, MPI_type, 0, MPI_COMM_WORLD);

	my_class data_1;
	data_1.buildMPIType();

	MPI_File mpi_file;

	string fname("test.txt");	

	if (MPI_File_open(MPI_COMM_WORLD, fname.c_str(), MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &mpi_file) != MPI_SUCCESS)
	{
		cout << "Error opening file!" << endl;
		exit(0);
	}
	
	string string_out;

	string_out = string("This data has been written by ") + to_string(id) + string("\n");

    int a[2] = {1, 1};

	MPI_File_write_ordered(mpi_file, a, 5, MPI_type, MPI_STATUS_IGNORE);
	MPI_File_close(&mpi_file);
    
	MPI_Request request;
	MPI_Isend(data, 1000, MPI_type, 1, 1, MPI_COMM_WORLD, &request);

	cout << "On process " << id << endl;
	for (int i = 0; i < 10; i++)
		cout<<"\t"<< i << ": I1=" << data[i].I1 << " I2=" << data[i].I2 << " D1= " << data[i].D1 << " S1=" << data[i].S1 << ". The unsent variable is " << data[i].var_not_to_send << endl;

	//MPI_Type_free(&my_class::MPI_type);
	MPI_Finalize();
}