/*

COL380. assignment 1.

Deadline: 12.03.2021

Problem: 

Compute the sum of the First N natural numbers. 

The sequential version is provided below.

1. Develop two parallel versions of the given program using OpenMP `parallel for' with maximum 8 threads.

Compilation command: gcc -O0 ...

2. The speedup and efficiency of both parallel versions for 2, 4, 8 threads for N=10^3, N=10^5 and N=10^7 numbers.

3. Does the speedup follow Amdahl's law?

Hint: Use a time function in the program (e.g. omp_get_wtime) to measure the time.

*/

#include <omp.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <math.h> 

//#define N 10000000 // vary N as required
//#define THREADS 8
//#define SERIAL_TIME 0.122026

int main(int argc, char* argv[]) 
{ 
   
   int strategy = atoi(argv[1]);
   int N = atoi(argv[2]);
   int THREADS = atoi(argv[3]);

   //printf("%d %d %d\n",strategy, N, THREADS);

   double start = omp_get_wtime();

   unsigned long long sum=0;
   // int *a;
   // a = (int *)malloc(sizeof(int)*N);

   long long int *a;
   a = (long long int *)malloc(sizeof(long long int)*N);

   omp_set_num_threads(THREADS);

   a[0] = 1;
   
   for(int i=1;i<N;i++) 
      a[i] = a[i-1]+1;

   // Serial Version

   // for(int i=0;i<N;i++) {
   //       sum += a[i];
   // }

   // 0.62 - 0.64

   //Parallelized version - 1      
   // #pragma omp parallel for
   // for(int i=0;i<N;i++) {
   //    #pragma omp critical
   //    {
   //       sum += a[i];
   //    }
   // }

   // 8.32s

   // Parallelized version - 2

   // long long int *shared;
   // shared = (long long int*)calloc(THREADS,sizeof(long long int));

   // #pragma omp parallel for shared(shared)
   // for(int i=0;i<N;i++) {
   //    int tid = omp_get_thread_num();
   //    shared[tid] = shared[tid] + a[i];
   // }
   // for(int i=0;i<THREADS;i++) sum += shared[i];
   // free(shared);

   // 0.7415s

   // Parallelized version - 3

   // long long int *shared;
   // shared = (long long int*)calloc(THREADS,sizeof(long long int));

   // #pragma omp parallel for shared(shared) schedule(dynamic)
   // for(int i=0;i<N;i++) {
   //    int tid = omp_get_thread_num();
   //    shared[tid] = shared[tid] + a[i];
   // }
   // for(int i=0;i<THREADS;i++) sum += shared[i];
   // free(shared);

   // 3.09s

   // Parallelized version - 4, approach - 1

   if(strategy == 1){
      //printf("Strategy1\n");
      #pragma omp parallel
      {
         long long int local = 0;
         #pragma omp for
         for(int i=0;i<N;i++) {
            local += a[i];
         }
         #pragma omp critical
         {
            sum += local;
         }
      }
   }
   //0.44s

   // Parallelized Version - 5

   // #pragma omp parallel
   // {
   //    long long int local = 0;
   //    #pragma omp for schedule(dynamic)
   //    for(int i=0;i<N;i++) {
   //       local += a[i];
   //    }
   //    #pragma omp critical
   //    {
   //       sum += local;
   //    }
   // }

   //3.35s

   // Parallelized Version - 6

   // #pragma omp parallel
   // {
   //    long long int local = 0;
   //    #pragma omp for schedule(dynamic,1000)
   //    for(int i=0;i<N;i++) {
   //       local += a[i];
   //    }
   //    #pragma omp critical
   //    {
   //       sum += local;
   //    }
   // }

   //0.476s

   // Parallelized Version - 7

   // int size = N;
   // while(size > 1){
   //    int curr = size/2;
   //    #pragma omp for
   //    for(int i=0;i<curr;i++) {
   //       a[i] += a[size-i-1];
   //    }
   //    size = ceil((double)size/2);
   // }
   // sum = a[0];

   // //0.882s

   //Parallelized Version - 8, Approach - 2
   else{
      //printf("Strategy0\n");
      int size = N;
      while(size > 1){
         int curr = size/2;
         #pragma omp for schedule(dynamic, 100)
         for(int i=0;i<curr;i++) {
            a[i] += a[size-i-1];
         }
         size = ceil((double)size/2);
      }
      sum = a[0];
   }

   //0.874s

   // Parallelized version - 9

   // #pragma omp parallel
   // {
   //    long long int local = 0;
   //    #pragma omp for nowait
   //    for(int i=0;i<N;i++) {
   //       local += a[i];
   //    }
   //    #pragma omp critical
   //    {
   //       sum += local;
   //    }
   // }

   //0.62s


   // for(int i=0;i<THREADS;i++) sum += shared[i];
   // free(shared);

   printf("%llu\n", sum);
   free(a);

   //double total = omp_get_wtime() - start;
   //printf("Time: %f\n",total);
   //printf("Threads: %d\n",THREADS);
   //printf("Size: %d\n",N);
   //printf("SpeedUp: %f\n",(double)SERIAL_TIME/total);
   //printf("Efficiency: %f\n",(double) SERIAL_TIME / (total * THREADS));     
} 