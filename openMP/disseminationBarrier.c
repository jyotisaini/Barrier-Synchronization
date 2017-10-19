#include<stdio.h>
#include<omp.h>
#include<stdlib.h>
#include<math.h>
#include<sys/time.h>
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
      }
    else 
    {
        printf("Syntax : ./Dissemination <#number of threads>  <#num of barriers>");
        exit(-1);
    }

    Flags totalProcessors[NUM_THREADS];
    int rounds = ceil(log(NUM_THREADS)/log(2));;
    omp_set_num_threads(NUM_THREADS);
    double totalTime=0;
    struct timeval startTime, endTime;
   int i,p,j;
   for(i=0;i<NUM_THREADS;i++)
      for(p=0;p<2;p++) 
        for(j=0;j<rounds;j++) 
            totalProcessors[i].myflags[p][j]= 0;

    #pragma omp parallel
   {    
        int totalThreads = omp_get_num_threads();
        int threadNum = omp_get_thread_num();

        int parity = 0;
        int sense = 1;
        int i,p,j,k, notifyNodes;
        
        Flags *localFlags = &totalProcessors[threadNum];
        
       for(j=0;j<NUM_BARRIERS; j++){
         printf("Thread %d- entering barrier %d \n  ", threadNum, j);
            for(k=0;k<rounds; k++) {
                 notifyNodes = ceil(pow(2,k));
                 int i =(threadNum+notifyNodes)%totalThreads;
                 totalProcessors[threadNum].partnerFlags[0][k] = &totalProcessors[i].myflags[0][k];
                 totalProcessors[threadNum].partnerFlags[1][k] = &totalProcessors[i].myflags[1][k];   
              }
           gettimeofday(&startTime, NULL);
           disseminationBarrier(localFlags, &sense, &rounds, &parity);
           gettimeofday(&endTime, NULL);
           double elapsedTime = (endTime.tv_sec - startTime.tv_sec)*1000.0;
           elapsedTime+=(endTime.tv_usec -startTime.tv_usec)/1000.0;
           printf("Thread %d exiting barrier - %d.\n ", threadNum,j);
           //printf("Total time spent by thread id : %d inside DisseminationBarrier id : %d is %f \n",threadNum, j, elapsedTime);
           #pragma omp critical
           {
            totalTime+=elapsedTime;
           }
          }   
      }
      printf("Total time spent per barrier averaged over NUM_BARRIERS is %f \n", totalTime/(NUM_BARRIERS*NUM_THREADS*1.0));
} 
           

void disseminationBarrier(Flags *localFlags, int *sense, int *rounds, int *parity) {
   int p=*parity, i;
   for(i=0; i<*rounds;i++){
      *localFlags->partnerFlags[p][i]= *sense;
       while(localFlags->myflags[p][i]!= *sense){}
   }
   if(*parity) 
    *sense = !*sense;
  *parity = 1 - *parity;
}


