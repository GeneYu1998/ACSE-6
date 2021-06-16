#include <omp.h> 
#include <stdio.h> 
#include <stdlib.h> 

int N = 100;
int tid;

int main(int argc, char *argv[]) {
omp_set_num_threads(12);//set number of threads here 
double A[N][N], B[N][N], C[N][N];
//initialization
for (int i = 0; i < N; i++) { 
    for(int j=0;j<N;j++) {
A[i][j] = j*1; B[i][j] = i * 2;
C[i][j] = 0; }
}

double start = omp_get_wtime(); //start time measurement
double sum(0);
//#pragma omp parallel num_threads(12)
//#pragma omp for
for (int i = 0; i < N; i++) {
for (int j = 0; j < N; j++) {
{
  for (int k=0; k < N; k++) 
  { 
  C[i][j] += A[i][k]*B[k][j];
  //printf("tid, C[%d],[%d], i, k, j: %d, %f, %d, %d, %d: \n", tid, i, j, C[i][j], i, k, j);

  }
}
}
}
double end = omp_get_wtime(); //end time measurement
printf("time used: %f", end - start);

}