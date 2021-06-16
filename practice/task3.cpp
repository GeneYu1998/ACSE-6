#include <omp.h> 
#include <stdio.h>
#include <stdlib.h>
#define N 50
#include <iostream>

int main (int argc, char *argv[]) { 
int i, nthreads, tid;
double a[N], b[N], c[N], d[N];
for (i=0; i<N; i++) { // Some initializations, arbitrary values 
a[i] = i * 1.5; 
b[i] = i + 22.35;
c[i] = d[i] = 0.0;
}

#pragma omp parallel shared(a,b,c,d,nthreads) private(i,tid) 
{
   tid = omp_get_thread_num(); 
   if (tid == 0) {
   nthreads = omp_get_num_threads(); 
   printf("Threads: %d\n", nthreads);
   }

   printf("Thread %d starting ...\n", tid);

   #pragma omp sections nowait 
   {
       #pragma omp section 
       {
       for (i=0; i< 10; i++) {
       c[i] = a[i] + b[i];
       printf("tid, i, c[i]:  %d, %d, %2f\n", tid, i, c[i]);// – what are these variables?
       } 
       }

       #pragma omp section 
       {
       for (i=0; i< 10; i++) {
        d[i] = a[i] * b[i];
        printf("tid, i, d[i]:  %d, %d, %2f\n", tid, i, d[i]);
        } 
        }

    } // end of sections
    printf("tid: %d\n", tid);
}

}