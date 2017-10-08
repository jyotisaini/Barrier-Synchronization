#include<stdio.h>
#include<omp.h>
#include<stdlib.h>
#include<math.h>
#define MAX_THREADS 8
#define NUM_BARRIERS 100 /* take these from command line later */

typedef struct Flags
{
	int myflags[2][MAX_THREADS];
	int *partnerFlags[2][MAX_THREADS];
	
}Flags;

void disseminationBarrier(Flags *localFlags, int *sense, int *rounds, int *parity);

int main(int argc, char** argv) {

    int NUM_THREADS;
    if(argc==2)
        NUM_THREADS =  atoi(argv[2]);
    else 
    {
        printf("Syntax : ./Dissemination <#number of threads>");
        exit(-1);
    }

    Flags totalProcessors[NUM_THREADS];

    int rounds = ceil((log(NUM_THREADS)/log(2)));
    omp_set_num_threads(NUM_THREADS);
    int parity =0;
    int sense = 0;
    int i,p,j,k;
    double startTime, endTime;
    int totalThreads = get_omp_num_threads();
    int threadNum = get_omp_thread_num();

    #pragma omp parallel {
        Flags *localFlags = &totalProcessors[threadNum];
         printf("Hello from Inside Thread %d", threadNum);

    
        for(i=0;i<NUM_THREADS;i++){
            for(p=0;p<2;p++) {
                for (j=0;j<rounds;j++)
                    #pragma omp critical {
                    totalProcessors[i].myflags[p][j]= 0;
                    totalProcessors[i].partnerFlags[p][j]= 0;
                    }
                 }
             }

       for (i=0;i<NUM_THREADS; i++)
            for(k=0;k<rounds; k++) {
                notifyNodes = pow(2,k);
                if(i==(threadNum+notifyNodes)%NUM_THREADS) {
                    #pragma omp critical {
                    totalProcessors[threadNum].partnerFlags[0][k] = totalProcessors[i].myflags[0][k];
                    totalProcessors[threadNum].partnerFlags[1][k] = totalProcessors[i].myflags[1][k];
                }
            }
        }
        int startTime= omp_get_wtime();
        disseminationBarrier(localFlags,&snese, &rounds, &parity);
        int endTime = omp_get_wtime();

        printf("Hello from thread %d . outside barrier. ", threadNum);

    }
    printf(" total time take by DisseminationBarrier id %d ",endTime - startTime );
          
}

void disseminationBarrier(Flags *localFlags, int *sense, int *rounds, int *parity) {
   int i;

   for(i=0; i<*rounds;i++){
   #pragma omp critical {
      *localFlags->partnerFlags[parity][i]= *sense;
    }
       while(*localFlags->myflags[parity][i]!= *sense);
   }

   if(*parity) {
    *sense = !*sense;
   }
  *parity = 1-*parity;

}


