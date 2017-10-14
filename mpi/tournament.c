#include <stdio.h>
#include <math.h>
#include "mpi.h"
#include <omp.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#define NUM_BARRIERS 3

typedef struct roundStruct {
	int role;
	int opponent;
}roundStruct;

int numProcessors;

enum winStatus {WINNER, LOSER, BYE, CHAMPION, DROPOUT};
void tounementBarrier(roundStruct array[numProcessors][10], int rank, int rounds, int barrier );

int main( int argc, char *argv[] ) {
	double t0, t1;
	int rank;
	int tag = 1;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numProcessors);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int counter = 0;
	int rounds = ceil(log(numProcessors)/log(2));
    /*Testing 
	printf("rounds : %d\n", rounds);
	printf("numProcessors  %d", numProcessors);
    */
	roundStruct array[numProcessors][10];
	double time1, time2;
	int i, j, k, l;
	j = rank;

	for( k=0; k<=rounds; k++ ) {
		array[j][k].role = -1;
		array[j][k].opponent = -1;
	}

	i=0;
	int g=0,compFirst,compSecond=0;
	l = rank;

	for( k=0; k<=rounds; k++ ) {
		compFirst = ceil(pow(2,k));
		compSecond = ceil(pow(2,k-1));

		//printf("compFirst : %d compSecond : %d\n", compFirst, compSecond);

		if((k > 0) && (l%compFirst==0) && ((l + (compSecond))< numProcessors) && (compFirst < numProcessors))
			array[l][k].role = WINNER;

		if((k > 0) && (l%compFirst == 0) && ((l + compSecond)) >= numProcessors)
			array[l][k].role = BYE;
		
		if((k > 0) && ((l%compFirst == compSecond)))
			array[l][k].role = LOSER;

		if((k > 0) && (l==0) && (compFirst >= numProcessors))
			array[l][k].role = CHAMPION;

        if(k==0) 
			array[l][k].role = DROPOUT;
		
		if(k==0 && numProcessors == 1) 
		{
			array[l][k].role = CHAMPION;
			break;
		}	
		
		if( array[l][k].role == LOSER )
			array[l][k].opponent = l - compSecond;

		if(array[l][k].role == WINNER || array[l][k].role == CHAMPION)
			array[l][k].opponent = l+compSecond;
		
	}
   
	/* Debugging 

	   printf("reached here %d", WINNER);

	  for(i=0;i<numProcessors; i++)
		for(j=0;j<=rounds;j++)
			printf("array[%d][%d] => role => %d, opponent =>%d \n", i, j, array[i][j].role, array[i][j].opponent);
     */

	i = rank;
	int f; double time=0;

	MPI_Barrier(MPI_COMM_WORLD);

    for( f=0; f<NUM_BARRIERS; f++ ){
    	t0 = MPI_Wtime();
    	tounementBarrier(array,rank,rounds,f);
    	t1 = MPI_Wtime();
        time+=t1-t0;
    }

	printf("time taken by one processor%d is %f\n\n",rank,time/NUM_BARRIERS);
	MPI_Finalize();
	return 0;
}

void tounementBarrier( roundStruct array[numProcessors][10], int rank, int rounds , int barrier) {
	int round=1, tag=1, my_msg=1;
	int i;
	MPI_Status *status;

	if(numProcessors ==1)
		return;

	printf("Proc %d waiting at barrier %d\n",rank, barrier);

	while(1) {
		if( array[rank][round].role == WINNER ) {
			int opponent_rank_loser_to_wait_on = array[rank][round].opponent;
			MPI_Recv( &my_msg, 1, MPI_INT, opponent_rank_loser_to_wait_on, tag, MPI_COMM_WORLD,NULL );
		}

		if( array[rank][round].role == LOSER ) {
			int opponent_winner = array[rank][round].opponent;
			MPI_Send(&my_msg,1,MPI_INT,opponent_winner,tag,MPI_COMM_WORLD);
			MPI_Recv(&my_msg,1,MPI_INT,opponent_winner,tag,MPI_COMM_WORLD,NULL);
			break;
		}

		if(array[rank][round].role == CHAMPION) {
			int opponent_loser_winner = array[rank][round].opponent;
			MPI_Recv(&my_msg,1,MPI_INT,opponent_loser_winner,tag,MPI_COMM_WORLD,NULL);
			MPI_Send(&my_msg,1,MPI_INT,opponent_loser_winner,tag,MPI_COMM_WORLD);
			break;
		}

		if( round <= rounds )
			round = round +1;
	}

	while(1) {
		if( round > 0 )
			round = round - 1;

		if( array[rank][round].role == WINNER ) {
			int opponent_loser_waken = array[rank][round].opponent;
			MPI_Send(&my_msg,1,MPI_INT,opponent_loser_waken,tag,MPI_COMM_WORLD);
		}

		if( array[rank][round].role == DROPOUT ){
			break;
		}
	}
}
