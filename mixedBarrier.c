#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <omp.h>
#include <mpi.h>
#include <string.h>
#include <math.h>
#include <unistd.h>


typedef struct treenode{
	bool parentSense;
	bool * parentPointer;
	bool * childPointers[2];
	bool haveChild[4];
	bool childNotReady[4];
	bool dummy;
} treenode;

void treeBarrier (bool* sense, treenode * node) {
	int threadID = omp_get_thread_num();
	int i;

	while (memcmp (node -> childNotReady,
					(bool[]){false, false, false, false},
					sizeof (node -> childNotReady)) != 0);
	
	for (i = 0; i <  4; i++) {
		node -> childNotReady[i] = node -> haveChild[i];
	}

	*node -> parentPointer = false;
	if (threadID != 0) {
		while (node -> parentSense != *sense);
	}
	bool * childPointer0 = node -> childPointers[0];
	bool * childPointer1 = node -> childPointers[1];
	*childPointer0 = *sense;
	*childPointer1 = *sense;
	*sense = !(*sense);
}


void disseminationBarrier (int rank, int numProcessors) {
	int round = 0;
	int sendBuf, recvBuf;
	MPI_Request sendRequest, recvRequest;
	MPI_Status status;
	while (round < (int)ceil(log(numProcessors * 1.0) / log(2.0))) {
		int recepient = (rank - (int)pow (2, round)) % numProcessors;
		if (recepient < 0) {
			recepient = recepient + numProcessors;
		}
		
		MPI_Isend(&sendBuf, 1, MPI_INT, (rank + (int)pow (2, round)) % numProcessors, 0, MPI_COMM_WORLD, &sendRequest);
		MPI_Irecv(&recvBuf, 1, MPI_INT, recepient, 0, MPI_COMM_WORLD, &recvRequest);

		MPI_Wait (&recvRequest, &status);
		round ++;
	}
}

int main (int argc, char ** argv) {
	MPI_Init (NULL, NULL);

	int worldSize;
	MPI_Comm_size (MPI_COMM_WORLD, &worldSize);

	int worldRank;
	MPI_Comm_rank (MPI_COMM_WORLD, &worldRank);

	int num_threads, num_barriers;

	if (argc == 3) {
		num_threads = atoi (argv[1]);
		num_barriers = atoi (argv[2]);
	}
	omp_set_num_threads(num_threads);
	treenode * nodes[num_threads];
	int i, j;
	for (i = 0; i < num_threads; i++) {
		nodes[i] = (treenode *) malloc (sizeof(treenode));
	}
	for (i = 0; i < num_threads; i++) {
		
		bool dummy = nodes[i] -> dummy;
		for (j = 0; j <  4; j++) {
			nodes[i] -> haveChild[j] = 4 * i + j < num_threads - 1 ? true : false;
			nodes[i] -> childNotReady[j] = nodes[i] -> haveChild[j];
		}

		nodes[i] -> parentPointer = i == 0 ? &dummy : &(nodes[(i-1) / 4] -> childNotReady[(i-1) % 4]);
		nodes[i] -> childPointers[0] = 2 * i + 1 < num_threads ? &(nodes[2 * i + 1] -> parentSense) : &dummy;
		nodes[i] -> childPointers[1] = 2 * i + 2 < num_threads ? &(nodes[2 * i + 2] -> parentSense) : &dummy;

		nodes[i] -> parentSense = false;
		nodes[i] -> dummy = false;
	}

	// printf ("%c\n", nodes[0] -> childNotReady[0]);
	#pragma omp parallel
	{
		bool * sense = (bool*) malloc(sizeof(bool));
		*sense = true;
		int ID = omp_get_thread_num();
		int k;
		for (k = 0; k < num_barriers; k++) {
			printf ("Thread %d entering MCS Barrier %d\n", ID, k);
			treeBarrier (sense, nodes[ID]);
			printf ("Thread %d exiting MCS Barrier %d\n", ID, k);
		}
		
		free (sense);
	}

	for (i = 0; i < num_threads; i++) {
		free (nodes[i]);
	}

	if (worldRank == 0) {
		sleep (10);
	}

	for (int i = 0; i < num_barriers; i ++) {
		printf ("Hello world from processor %d\n", worldRank);
		printf ("Processor %d entering barrier %d\n", worldRank, i);
		disseminationBarrier (worldRank, worldSize);
		printf ("Processor %d out of barrier %d\n", worldRank, i);
	}

	MPI_Finalize();
}
