#include<stdio.h>

int main(int argc,char** argv){
  #pragma omp parallel
  printf("Hello World!\n");
  return 0;
}