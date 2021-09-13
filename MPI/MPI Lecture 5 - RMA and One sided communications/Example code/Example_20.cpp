#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <time.h>

using namespace std;

int id, p;

const int max_count = 100;

unsigned long long fibonacci(int num)
{
	unsigned long long next = 1;
	unsigned long long val[2] = { 1, 1 };
	for (int cnt = 0; cnt < num - 2; cnt++)
	{
		next = val[0] + val[1];
		val[0] = val[1];
		val[1] = next;
	}

	return next;
}

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	srand(time(NULL) + id * 10);

	int* counter = nullptr;
	MPI_Win window;

	int current_count= -1;

	if (id == 0)
		MPI_Win_allocate(sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &counter, &window);
	else
		MPI_Win_allocate(0, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &counter, &window);

	if (id == 0)
		*counter = 0;

	//ensures that the system is set before the task counting commences
	MPI_Win_fence(0, window);

	do
	{
		MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, window);
		//read the current count off the memory on process zero
		MPI_Get(&current_count, 1, MPI_INT, 0, 0, 1, MPI_INT, window);
		MPI_Win_flush(0, window);
		//Increment the counter
		int increment = 1;
		MPI_Accumulate(&increment, 1, MPI_INT, 0, 0, 1, MPI_INT, MPI_SUM, window);
		MPI_Win_unlock(0, window);

		if (current_count < max_count)
		{
			int num = rand() % 100;
			cout << "Process " << id << " is did task " << current_count << " and the " << num << "th Fibonacci number is " << fibonacci(num) << endl;
		}
	} while (current_count < max_count);

	//fence to ensure that the other process get to read the counter before process 0 exits
	MPI_Win_fence(0, window);
	MPI_Win_free(&window);

	MPI_Finalize();
}