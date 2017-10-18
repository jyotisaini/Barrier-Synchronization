#include<stdio.h>
#include<omp.h>
#include<stdlib.h>
#include<math.h>
#define MAX_THREADS 8

typedef struct Flags
{
	int myflags[2][MAX_THREADS];
	int *partnerFlags[2][MAX_THREADS];
	
}Flags;

void disseminationBarrier(Flags *localFlags, int *sense, int *rounds, int *parity);

int main(int argc, char *argv[]) {

     int NUM_THREADS,NUM_BARRIERS;
    if(argc==3) {

        NUM_THREADS =  atoi(argv[1]);
        NUM_BARRIERS = atoi(argv[2]);
        //printf(" num threads %d\n", NUM_THREADS);
        //printf(" num barrier %d\n", NUM_BARRIERS);
      }
    else 
    {
        printf("Syntax : ./Dissemination <#number of threads>  <#num of barriers>");
        exit(-1);
    }

    Flags totalProcessors[NUM_THREADS];
    int rounds = ceil(log(NUM_THREADS)/log(2));;
    //printf("rounds = %d", rounds);
    omp_set_num_threads(NUM_THREADS);
    double startTime, endTime, totalTime;
   

    #pragma omp parallel
   {    
        int totalThreads = omp_get_num_threads();
        int threadNum = omp_get_thread_num();

        int parity = 0;
        int sense = 1;
        int i,p,j,k, notifyNodes;
        
        Flags *localFlags = &totalProcessors[threadNum];
       
        
        #pragma omp critical 
        for(i=0;i<NUM_THREADS;i++)
            for(p=0;p<2;p++) 
                for (j=0;j<rounds;j++) 
                     totalProcessors[i].myflags[p][j]= 0;

       for(j=0;j<NUM_BARRIERS; j++){
         printf("Hello from Inside Thread %d Inside barrier %d \n  ", threadNum, j);
         #pragma omp critical
         {
         for (i=0;i<NUM_THREADS; i++) {
            for(k=0;k<rounds; k++) {
                 notifyNodes = ceil(pow(2,k));
                 if( i==(threadNum+notifyNodes)%NUM_THREADS) {
                       totalProcessors[threadNum].partnerFlags[0][k] = &totalProcessors[i].myflags[0][k];
                       totalProcessors[threadNum].partnerFlags[1][k] = &totalProcessors[i].myflags[1][k];
                 }
              }
            }
          }
          startTime= omp_get_wtime();
          disseminationBarrier(localFlags, &sense, &rounds, &parity);
          endTime = omp_get_wtime();
          printf("Bye from thread %d outside barrier - %d.\n ", threadNum,j);
      }
      printf(" total time spent inside  DisseminationBarrier id : %d is %f \n",j, endTime - startTime );
      totalTime+=(endTime-startTime);
    
    } 
    printf("total time spent per barrier averaged over NUM_BARRIERS is %f", totalTime/NUM_BARRIERS*1.0);       
}

void disseminationBarrier(Flags *localFlags, int *sense, int *rounds, int *parity) {
   int p=*parity, i;

   for(i=0; i<*rounds;i++){
   #pragma omp critical 
    {
      *localFlags->partnerFlags[p][i]= *sense;
    }
       while(localFlags->myflags[p][i]!= *sense){}
   }

   if(*parity) 
    *sense = !*sense;
  *parity = 1 - *parity;
}


