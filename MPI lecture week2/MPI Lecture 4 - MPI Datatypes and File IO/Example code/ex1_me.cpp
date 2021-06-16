#include <mpi.h>
#include <iostream>
#include <locale>
#include <time.h>
#include <stdlib.h>
#include <vector>

using namespace std;

int id, p;

const int max_particles = 10000;
vector<particle> temp_list;

/*void send_data()
{
    particle temp;
    //add to particle

    MPI_Bcast(&temp, sizeof(particle), MPI_BYTE, num, MPI_COMM_WORLD);
}*/

class particle{

      public:
      double x[2];
      double v[2];

      particle()
      {
          v[0] = 0;
          v[1] = 0;
      }

      particle *prev, *next;

      static MPI_Datatype MPI_type;
      static void create_type();

};

class particle_linked: public particle
{
public:
   particle_linked *next, *prev;

   particle_linked()
   {
       next = nullptr;
       prev = nullptr;
   }
};

MPI_Datatype particle::MPI_type;

particle_linked *start_list = nullptr;

void particle::create_type()
{
	int block_lengths[2];
	MPI_Aint displacements[2];
	MPI_Aint addresses[2], add_start;
	MPI_Datatype typelist[2];

	particle temp;

	typelist[0] = MPI_DOUBLE;
	block_lengths[0] = 2;
	MPI_Get_address(&temp.x, &addresses[0]);

	typelist[1] = MPI_DOUBLE;
	block_lengths[1] = 2;
	MPI_Get_address(&temp.v, &addresses[1]);

	MPI_Get_address(&temp, &add_start);
	for (int i = 0; i < 2; i++) displacements[i] = addresses[i] - add_start;

	MPI_Type_create_struct(2, block_lengths, displacements, typelist, &MPI_type);
	MPI_Type_commit(&MPI_type);
}

const double x_max[2] = {1.0 , 1.0};

double random_dart()
{
    return ((double) rand()) / ((double) RAND_MAX + 1.0);
}

int proc_from_x(double *x)
{
    return (int)((x[0] * p / x_max[0]));
}


void create_and_send_particles()
{   
    particle_linked *temp = new particle_linked;
    for (int i = 0; i < max_particles; ++i)
    {   
        for (int i=0; i<2; i++)
        {temp->x[i] = random_dart() * x_max[i];}
        
        int dest = proc_from_x(temp->x);

        if(dest == 0)
        {
            if (start_list!= nullptr)
               start_list->prev = temp;
            temp->next = start_list;
            start_list = temp;
        }

        else
        {
            MPI_Send(&temp, 1, particle::MPI_type, dest, 0, MPI_COMM_WORLD);
        }

        for (int i = 1; i < p; ++i)
        {
            MPI_Send(nullptr, 0, particle::MPI_type, i , 0 ,MPI_COMM_WORLD);
        }
        
    }
}

void receive_particles()
{
    MPI_Status status;
    do
    {
       particle_linked* temp = new particle_linked;
       MPI_
    } while (true);
    
}


int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	particle::create_type();
   
    if (id == 0)
       create_and_send_particles();
    
    else
       recieve_particles();

    int count = 0;
    particle_linked* current = start_list;

    while (current!=nullptr)
    {
        count++;
        current = current->next;
    }

    while (current!=nullptr)
    {
        particle_linked* current = start_list;
        delete current;
        current = temp;
    }

    
    MPI_Type_free(&particle::MPI_type);
    MPI_Finalize();

}