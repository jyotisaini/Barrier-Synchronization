#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "omp.h"

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
	inr i;

	while (memcmp (nodes[threadID].childNotReady, 
					(bool[]) {false, false, false, false}, 
					sizeof (nodes[threadID].childNotReady)) != 0);
	for (i = 0; i <  4; i++) {
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

int main (int argc, char ** argv) {
	int i, j;
	for (i = 0; i < NUM_THREADS; i++) {
		for (j = 0; j <  4; j++) {
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
}