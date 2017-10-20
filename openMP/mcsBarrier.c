#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>
#include <omp.h>

#include "../nthPrime.c"


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

int main (int argc, char ** argv) {

	int num_threads, num_barriers;
	double timeTaken = 0.0;

	if (argc == 3) {
		num_threads = atoi (argv[1]);
		num_barriers = atoi (argv[2]);
	} else {
		printf ("Syntax error: Corect syntax is ./mcsBarrier <num_processors> <num_barriers>\n");
		return -1;
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
		struct timeval start, end;
		double elapsedTime = 0.0;
		for (k = 0; k < num_barriers; k++) {
			long n = getPrime (10000);
			printf ("Thread %d entering MCS Barrier %d\n", ID, k);
			gettimeofday (&start, NULL);
			treeBarrier (sense, nodes[ID]);
			gettimeofday (&end, NULL);
			printf ("Thread %d exiting MCS Barrier %d\n", ID, k);
			elapsedTime += (end.tv_sec - start.tv_sec) * 1000.0;
			elapsedTime += (end.tv_usec - start.tv_usec) / 1000.0;
		}
		
		printf ("Average time spent by thread %d in MCS Barrier is %f\n", ID ,elapsedTime/(num_barriers * 1.0));
		free (sense);
		#pragma omp critical
		{
			timeTaken += elapsedTime;
		}
	}

	for (i = 0; i < num_threads; i++) {
		free (nodes[i]);
	}

	printf ("Average time spent by a thread in MCS Barrier is %f\n", timeTaken / (num_barriers * num_threads * 1.0));
	return 0;
}
