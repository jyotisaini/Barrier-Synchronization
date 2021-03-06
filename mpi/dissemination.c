#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <mpi.h>
#include <unistd.h>
#include <sys/time.h>

#include "../nthPrime.c"


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
		
		//Send message to corresponding partner based on the round
		MPI_Isend(&sendBuf, 1, MPI_INT, (rank + (int)pow (2, round)) % numProcessors, 0, MPI_COMM_WORLD, &sendRequest);
		MPI_Irecv(&recvBuf, 1, MPI_INT, recepient, 0, MPI_COMM_WORLD, &recvRequest);

		//wait to receive message from the corresponding partner
		MPI_Wait (&recvRequest, &status);
		round ++;
	}
}

int main (int argc, char ** argv) {
	MPI_Init (NULL, NULL);
	int numBarriers;
	int nPrime;
	struct timeval start, end;
	double elapsedTime = 0.0;

	if (argc == 3) {
		numBarriers = atoi (argv[1]);
		nPrime = atoi (argv[2]);
	} else {
		printf ("Syntax Error: Correct syntax is ./dissemination <num_barriers> <prime_count>\n");
		return -1;
	}

	int worldSize;
	MPI_Comm_size (MPI_COMM_WORLD, &worldSize);

	int worldRank;
	MPI_Comm_rank (MPI_COMM_WORLD, &worldRank);

	
	int i;
	for (i = 0; i < numBarriers; i ++) {
		long a = getPrime (nPrime);
		printf ("Hello world from processor %d\n", worldRank);
		printf ("Processor %d entering barrier %d\n", worldRank, i);

		gettimeofday (&start, NULL);
		disseminationBarrier (worldRank, worldSize);
		gettimeofday (&end, NULL);

		printf ("Processor %d out of barrier %d\n", worldRank, i);

		elapsedTime += (end.tv_sec - start.tv_sec) * 1000.0;
		elapsedTime += (end.tv_usec - start.tv_usec) / 1000.0;
	}

	printf ("Average time spent by processor %d is %f\n", worldRank, elapsedTime / (numBarriers * 1.0));
	MPI_Finalize();
}
