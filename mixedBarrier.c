#include <stdio.h>
#include <stdbool.h>
#include <omp.h>
#include <mpi.h>

#define NUM_THREADS 5

typedef struct treenode{
	bool parentSense;
	bool * parentPointer;
	bool * childPointers[2];
	bool haveChild[4];
	bool childNotReady[4];
	bool dummy;
} treenode;

treenode nodes[NUM_THREADS];

void treeBarrier (bool sense) {
	int threadID = omp_get_thread_num();

	while (memcmp (nodes[threadID].childNotReady, 
					(bool[]) {false, false, false, false}, 
					sizeof (nodes[threadID].childNotReady)) != 0);
	for (int i = 0; i <  4; i++) {
		nodes[threadID].childNotReady[i] = nodes[threadID].haveChild[i];
	}
	*nodes[threadID].parentPointer = false;
	if (threadID != 0) {
		while (nodes[threadID].parentSense != sense);
	}

	*nodes[threadID].childPointers[0] = sense;
	*nodes[threadID].childPointers[1] = sense;

	sense = !sense;
}

void disseminationBarrier (int rank, int numProcessors) {
	int round = 0;
	int sendBuf, recvBuf;
	MPI_Request sendRequest, recvRequest;
	MPI_Status status;
	while (round < (int)ceil(log(numProcessors * 1.0) / log(2.0))) {
		// if(sendRequest) {
		// 	MPI_Wait(&sendRequest, &status);
		// }
		int recepient = (rank - (int)pow (2, round)) % numProcessors;
		if (recepient < 0) {
			recepient = recepient + numProcessors;
		}
		
		MPI_Isend(&sendBuf, 1, MPI_INT, (rank + (int)pow (2, round)) % numProcessors, 0, MPI_COMM_WORLD, &sendRequest);
		printf ("sent message from %d to %d\n", rank, (rank + (int)pow (2, round)) % numProcessors);
		MPI_Irecv(&recvBuf, 1, MPI_INT, recepient, 0, MPI_COMM_WORLD, &recvRequest);

		MPI_Wait (&recvRequest, &status);
		printf ("Received message sent from %d to %d\n", recepient, rank);
		round ++;
	}
}

int main () {
	MPI_Init (NULL, NULL);

	int worldSize;
	MPI_Comm_size (MPI_COMM_WORLD, &worldSize);

	int worldRank;
	MPI_Comm_rank (MPI_COMM_WORLD, &worldRank);

	for (int i = 0; i < NUM_THREADS; i++) {
		for (int j = 0; j <  4; j++) {
			nodes[i].haveChild[j] = 4 * i + j < NUM_THREADS - 1 ? true : false;
			nodes[i].childNotReady[j] = nodes[i].haveChild[j];
		}

		nodes[i].parentPointer = i == 0 ? &nodes[i].dummy : &nodes[(i-1) / 4].childNotReady[(i-1) % 4];
		nodes[i].childPointers[0] = 2 * i + 1 < NUM_THREADS ? &nodes[2 * i + 1].parentSense : &nodes[i].dummy;
		nodes[i].childPointers[1] = 2 * i + 2 < NUM_THREADS ? &nodes[2 * i + 2].parentSense : &nodes[i].dummy;

		nodes[i].parentSense = false;
		nodes[i].dummy = false;
	}

	omp_set_num_threads(NUM_THREADS);

	#pragma omp parallel
	{
		bool sense = true;
		int ID = omp_get_thread_num();
		printf ("Thread %d entering MCS Barrier\n", ID);
		treeBarrier (sense);
		printf ("Thread %d exiting MCS Barrier\n", ID);

	}

	if (worldRank == 0) {
		sleep (10);
	}

	for (int i = 0; i < 2; i ++) {
		printf ("Hello world from processor %d\n", worldRank);
		printf ("Processor %d entering barrier %d\n", worldRank, i);
		disseminationBarrier (worldRank, worldSize);
		printf ("Processor %d out of barrier %d\n", worldRank, i);
	}

	MPI_Finalize();
}
