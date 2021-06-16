#include <omp.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <iostream>

int main (int argc, char *argv[]) {
int maxt;
int tid;
int nthreads;
// Fork a team of threads giving them their own copies of variables 
omp_set_num_threads(32);
#pragma omp parallel private(nthreads, tid)
{

tid = omp_get_thread_num();
printf("Hello from thread %d\n", tid); // Obtain thread number //print Hello World from thread tid – COMPLETE HERE
if (tid == 0)
{ 
nthreads = omp_get_num_threads();

printf("number of threads %d\n", nthreads);
}

// All threads join master thread and disband
}


return(0); 
}