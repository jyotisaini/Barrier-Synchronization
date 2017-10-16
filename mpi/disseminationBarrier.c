#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <mpi.h>
#include <unistd.h>

#define NUM_NODES 8

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