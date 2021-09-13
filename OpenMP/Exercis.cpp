#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <vector>

using namespace std;

int id, p;

class particle
{
public:
	double x[2];
	double v[2];

	particle()
	{
		v[0] = 0.0;
		v[1] = 0.0;
	}

	static MPI_Datatype MPI_Type;
	static void create_type(void);
};

class particle_linked : public particle
{
public:
	particle_linked* next, * prev;

	particle_linked()
	{
		next = nullptr;
		prev = nullptr;
	}
};

MPI_Datatype particle::MPI_Type;


void particle::create_type(void)
{
	vector<int> block_lengths;
	vector<MPI_Aint> addresses;
	MPI_Aint add_start, temp_add;
	vector<MPI_Datatype> typelist;

	particle temp;

	typelist.push_back(MPI_DOUBLE);
	block_lengths.push_back(2);
	MPI_Get_address(&temp.x, &temp_add);
	addresses.push_back(temp_add);

	typelist.push_back(MPI_DOUBLE);
	block_lengths.push_back(2);
	MPI_Get_address(&temp.v, &temp_add);
	addresses.push_back(temp_add);
	
	MPI_Get_address(&temp, &add_start);
	for (int i = 0; i < addresses.size(); i++) addresses[i] = addresses[i] - add_start;

	MPI_Type_create_struct(addresses.size(), block_lengths.data(), addresses.data(), typelist.data(), &MPI_Type);
	MPI_Type_commit(&MPI_Type);
}

const double x_max[2] = { 1.0, 1.0 };

double random()
{
	return ((double)rand()) / (((double)RAND_MAX) + 1.0);
}

int proc_from_x(double* x)
{
	return (int)((x[0] * p) / x_max[0]);
}

particle_linked* start_list = nullptr;
const int max_particles = 10000;


void create_and_send_particles(void)				//on proc_zero
{
	for (int i = 0; i < max_particles; i++)
	{
		particle_linked *temp_particle = new particle_linked;

		for (int cnt = 0; cnt < 2; cnt++)
			temp_particle->x[cnt] = random() * x_max[cnt];

		int destination = proc_from_x(temp_particle->x);

		if (destination == 0)
		{
			if (start_list != nullptr)
				start_list->prev = temp_particle;
			temp_particle->next = start_list;
			start_list = temp_particle;
		}
		else
			MPI_Send(temp_particle, 1, particle::MPI_Type, destination, 0, MPI_COMM_WORLD);
	}

	for (int i = 1; i < p; i++)
		MPI_Send(nullptr, 0, particle::MPI_Type, i, 0, MPI_COMM_WORLD);
}

void receive_particles(void)
{
	MPI_Status status;

	do
	{
		particle_linked* temp_particle = new particle_linked;
		MPI_Recv(temp_particle, 1, particle::MPI_Type, 0, 0, MPI_COMM_WORLD, &status);
		int count;
		MPI_Get_count(&status, particle::MPI_Type, &count);

		if (count == 1)
		{
			if (start_list != nullptr)
				start_list->prev = temp_particle;
			temp_particle->next = start_list;
			start_list = temp_particle;
		}
		else if (count == 0)
		{
			delete temp_particle;
			break;
		}
		else
		{
			cout << "Unexpected number of particles received!" << endl;
			break;
		}

	} while (true);
}


int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL) + id * 1000);

	particle::create_type();

	if (id == 0)
		create_and_send_particles();
	else
		receive_particles();

	int count = 0;
	particle_linked* current = start_list;

	while (current != nullptr)
	{
		count++;
		current = current->next;
	}

	cout << id << ": is responsible for " << count << " particles" << endl;

	current = start_list;

	while (current != nullptr)
	{
		particle_linked* temp = current->next;
		delete current;
		current = temp;
	}

	MPI_Type_free(&particle::MPI_Type);
	MPI_Finalize();
}