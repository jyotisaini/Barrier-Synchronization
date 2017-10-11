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

	while (memcmp (nodes[threadID].childNotReady, 
					(bool[]) {false, false, false, false}, 
					sizeof (nodes[threadID].childNotReady)) != 0) {
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
}

int main (int argc, char ** argv) {
	
	for (int i = 0; i < NUM_THREADS; i++) {
		for (int j = 0; j <  4; j++) {
			nodes[i].haveChild[j] = 4 * i + j < NUM_THREADS ? true : false;
			nodes[i].childNotReady[j] = nodes[i].haveChild[j];
		}

		nodes[i].parentPointer = i == 0 ? &nodes[i].dummy : &nodes[(i-1) / 4].childNotReady[(i-1) % 4];
		nodes[i].childPointers[0] = 2 * i + 1 >= NUM_THREADS ? &nodes[i].dummy : &nodes[2 * i + 1].parentSense;
		nodes[i].childPointers[1] = 2 * i + 2 >= NUM_THREADS ? &nodes[i].dummy : &nodes[2 * i + 2].parentSense;

		nodes[i].parentSense = false;
	}

	#pragma omp parallel
	{
		bool sense = true;

	}
}