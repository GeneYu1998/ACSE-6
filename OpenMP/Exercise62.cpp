#include <mpi.h>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <vector>

using namespace std;

vector<double> list;
int list_size = 20;

int partition_into_2(double* list, int bottom, int top)
{
	double pivot = list[top];
	int pivot_loc = bottom;

	for (int j = bottom; j < top; j++)
	{
		if (list[j] <= pivot)
		{
			double temp = list[pivot_loc];
			list[pivot_loc] = list[j];
			list[j] = temp;
			pivot_loc++;
		}
	}
	double temp = list[pivot_loc];
	list[pivot_loc] = list[top];
	list[top] = temp;

	return pivot_loc;
}


void quick_sort(double* list, int bottom, int top)
{
	if (top > bottom)
	{
		int pivot = partition_into_2(list, bottom, top);

		quick_sort(list, bottom, pivot - 1);
		quick_sort(list, pivot + 1, top);
	}
}



int main(int argc, char* argv[])
{
	srand(time(NULL));
	list.resize(list_size);
	cout << "list in: ";
	for (int i = 0; i < list_size; i++)
	{
		list[i] = ((double)rand()) / ((double)RAND_MAX) * 100.0;
		cout << list[i] << "\t";
	}
	cout << endl;

	quick_sort(list.data(), 0, list_size - 1);

	cout << "list out: ";
	for (int i = 0; i < list_size; i++)
		cout << list[i] << "\t";

	cout << endl;
}