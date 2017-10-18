#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "omp.h"


typedef struct treenode{
	bool parentSense;
	bool * parentPointer;
	bool * childPointers[2];
	bool haveChild[4];
	bool childNotReady[4];
	bool dummy;
} treenode;


void treeBarrier (bool* sense, treenode * node) {
	// printf("%c\n", node -> childNotReady[0]? 't' : 'f');
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
}