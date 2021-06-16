#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <vector>

using namespace std;

int id, p;

double **array_data;
const int i_max = 200, j_max = 200;

MPI_Datatype top_type, bottom_type, left_type, right_type;

void create_array(int m, int n)
{
    array_data = new double* [m];
    for (int i = 0; i < m; ++i)
       array_data[i] = new double[n];   
}

void free_array(int m, int n)
{
    for (int i = 0; i < m; ++i)
        delete[] array_data[i];
    delete array_data;
}

void create_left_type(int m, int n)
{
    vector<int> block_lengths;
    vector<MPI_Aint> addresses;
    MPI_Aint add_start, temp_add;
    vector<MPI_Datatype> typelist;

    for (int i = 0; i < m; i++)
    {
        block_lengths.push_back(1);
        typelist.push_back(MPI_DOUBLE);
        MPI_Get_address(&array_data[i][0], &temp_add);
        addresses.push_back(temp_add);

    }

    MPI_Get_address(array_data, &add_start);
    for (int i = 0; i < m; i++)
        addresses[i] = addresses[i] - add_start;

    MPI_Type_create_struct(m, block_lengths.data(), addresses.data(), typelist.data(), &left_type);
    MPI_Type_commit(&left_type);
     
}

void create_right_type(int m, int n)
{
    vector<int> block_lengths;
    vector<MPI_Aint> addresses;
    MPI_Aint add_start, temp_add;
    vector<MPI_Datatype> typelist;

    for (int i = 0; i < m; i++)
    {
        block_lengths.push_back(1);
        typelist.push_back(MPI_DOUBLE);
        MPI_Get_address(&array_data[i][n-1], &temp_add);
        addresses.push_back(temp_add);

    }

    MPI_Get_address(array_data, &add_start);
    for (int i = 0; i < m; i++)
        addresses[i] = addresses[i] - add_start;

    MPI_Type_create_struct(m, block_lengths.data(), addresses.data(), typelist.data(), &right_type);
    MPI_Type_commit(&right_type);
     
}

void create_top_type(int m, int n)
{
    int block_length;
    MPI_Aint address, add_start;
    MPI_Datatype typeval;

    block_length = n;
    typeval = MPI_DOUBLE;
    MPI_Get_address(&array_data[0][0], &address);
    MPI_Get_address(array_data, &add_start);

    MPI_Type_create_struct(1, &block_length, &address, &typeval, &top_type);
    MPI_Type_commit(&top_type);
     
}

void create_bottom_type(int m, int n)
{
    int block_length;
    MPI_Aint address, add_start;
    MPI_Datatype typeval;

    block_length = n;
    typeval = MPI_DOUBLE;
    MPI_Get_address(&array_data[m-1][0], &address);
    MPI_Get_address(array_data, &add_start);


    MPI_Type_create_struct(1, &block_length, &address, &typeval, &bottom_type);
    MPI_Type_commit(&bottom_type);
     
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

    create_array(i_max, j_max);
    create_left_type(i_max, j_max);
    create_right_type(i_max, j_max);
    create_top_type(i_max, j_max);
    create_bottom_type(i_max, j_max);
    
    if(id == 0)
    {
        for (int i = 0; i < i_max; i++)
         for (int j = 0; j < j_max; j++)
           array_data[i][j] = i *j_max + j;
    }

    MPI_Bcast(array_data, 1, left_type, 0, MPI_COMM_WORLD);
    MPI_Bcast(array_data, 1, right_type, 0, MPI_COMM_WORLD);  
    MPI_Bcast(array_data, 1, top_type, 0, MPI_COMM_WORLD);
    MPI_Bcast(array_data, 1, bottom_type, 0, MPI_COMM_WORLD);    

    MPI_Type_free(&left_type);
    MPI_Type_free(&right_type);
    MPI_Type_free(&top_type);
    MPI_Type_free(&bottom_type);    
    MPI_Finalize();
}
