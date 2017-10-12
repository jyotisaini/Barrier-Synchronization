#include <stdio.h>
#include <math.h>
#include "mpi.h"
#include <omp.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#define rounds_first ceil(log(numprocessors)/log(2))
#define rounds1 rounds_first
#define NUM_BARRIERS 1

typedef struct roundStruct {
	int role;
	int vpid;
	int tb_round;
	int opponent;
}roundStruct;

int numprocessors;

enum winStatus {WINNER, LOSER, BYE, CHAMPION, DROPOUT};
void tounementBarrier(roundStruct array[numprocessors][10], int rank, int rounds );

int main( int argc, char *argv[] ) {
	double t0, t1, t_code;
	int rank, my_dst, my_src;
	int tag = 1;

	char filename[20];
	MPI_Status mpi_result;
	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD,&numprocessors);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int counter = 0;
	int rounds = ceil(log(numprocessors)/log(2) );

	roundStruct array[numprocessors][10];
	double time1, time2;
	int i, j, k, l;
	j = rank;

	for( k=0; k<=rounds; k++ ) {
		array[j][k].role = -1;
		array[j][k].opponent = -1;
	}

	i=0;
	int temp=0,temp2,g=0,comp,comp_second=0;
	l = rank;

	for( k=0; k<=rounds; k++ ) {
		temp = k;
		temp2= l;
		comp = ceil(pow(2,k));
		comp_second = ceil(pow(2,k-1));

		if((k > 0) && (l%comp==0) && ((l + (comp_second))< numprocessors) && (comp < numprocessors))
			array[l][k].role = WINNER;

		if((k > 0) && (l%comp == 0) && ((l + comp_second)) >= numprocessors)
			array[l][k].role = BYE;
		
		if((k > 0) && ((l%comp == comp_second)))
			array[l][k].role = LOSER;

		if((k > 0) && (l==0) && (comp >= numprocessors))
			array[l][k].role = CHAMPION;

		if(k==0 ) 
			array[l][k].role = DROPOUT;
		
		if( array[l][k].role == LOSER )
			array[l][k].opponent = l - comp_second;

		if(array[l][k].role == WINNER || array[l][k].role == CHAMPION)
			array[l][k].opponent = l+comp_second;
		
	}

	i = rank;
	int f; double time=0;

	MPI_Barrier(MPI_COMM_WORLD);
    for( f=0; f<NUM_BARRIERS; f++ ){
    	printf("Proc %d waiting at barrier %d\n",rank, f);

    	t0 = MPI_Wtime();
    	tounementBarrier(array,rank,rounds);
    	t1 = MPI_Wtime();
        time+=t1-t0;

    }

	printf("time taken by one processor%d is %f\n\n",rank,time/NUM_BARRIERS);
	MPI_Finalize();
	return 0;
}

void tounementBarrier( roundStruct array[numprocessors][10], int rank, int rounds ) {
	int round=1, tag=1, my_msg=1;
	int i;
	MPI_Status *status;

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
