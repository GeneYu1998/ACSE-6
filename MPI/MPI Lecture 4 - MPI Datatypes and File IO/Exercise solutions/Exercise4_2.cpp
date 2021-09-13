#include <mpi.h>
#include <iostream>
#include <vector>

using namespace std;

int id, p;

MPI_Datatype Datatype_left, Datatype_right, Datatype_top, Datatype_bottom;

void createdatatypes(double** data, int m, int n)
{
	vector<int> block_lengths;
	vector<MPI_Datatype> typelist;
	vector<MPI_Aint> addresses;
	MPI_Aint add_start;

	//left
	for (int i = 0; i < m; i++)
	{
		block_lengths.push_back(1);
		typelist.push_back(MPI_DOUBLE);
		MPI_Aint temp_address;
		MPI_Get_address(&data[i][0], &temp_address);
		addresses.push_back(temp_address);
	}
	MPI_Get_address(data, &add_start);
	for (int i = 0; i < m; i++) addresses[i] = addresses[i] - add_start;
	MPI_Type_create_struct(m, block_lengths.data(), addresses.data(), typelist.data(), &Datatype_left);
	MPI_Type_commit(&Datatype_left);

	//right
	block_lengths.resize(0);
	typelist.resize(0);
	addresses.resize(0);
	for (int i = 0; i < m; i++)
	{
		block_lengths.push_back(1);
		typelist.push_back(MPI_DOUBLE);
		MPI_Aint temp_address;
		MPI_Get_address(&data[i][n - 1], &temp_address);
		addresses.push_back(temp_address);
	}
	for (int i = 0; i < m; i++) addresses[i] = addresses[i] - add_start;
	MPI_Type_create_struct(m, block_lengths.data(), addresses.data(), typelist.data(), &Datatype_right);
	MPI_Type_commit(&Datatype_right);

	//top - only need one value
	int block_length = n;
	MPI_Datatype typeval = MPI_DOUBLE;
	MPI_Aint address;
	MPI_Get_address(data[0], &address);
	address = address - add_start;
	MPI_Type_create_struct(1, &block_length, &address, &typeval, &Datatype_top);
	MPI_Type_commit(&Datatype_top);

	//bottom - only need one value
	MPI_Get_address(data[m - 1], &address);
	address = address - add_start;
	MPI_Type_create_struct(1, &block_length, &address, &typeval, &Datatype_bottom);
	MPI_Type_commit(&Datatype_bottom);
}


double** data_array;
const int i_max = 200;
const int j_max = 200;

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	data_array = new double*[i_max];
	for (int i = 0; i < i_max; i++)
		data_array[i] = new double[j_max];

	createdatatypes(data_array, i_max, j_max);


	if (id == 0)
	{
		for (int i = 0; i < i_max; i++)
			for (int j = 0; j < j_max; j++)
				data_array[i][j] =(double) (i * j_max + j);
	}

	MPI_Bcast(data_array, 1, Datatype_left, 0, MPI_COMM_WORLD);
	MPI_Bcast(data_array, 1, Datatype_right, 0, MPI_COMM_WORLD);
	MPI_Bcast(data_array, 1, Datatype_top, 0, MPI_COMM_WORLD);
	MPI_Bcast(data_array, 1, Datatype_bottom, 0, MPI_COMM_WORLD);

	MPI_Finalize();
}
