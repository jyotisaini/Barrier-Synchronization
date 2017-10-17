OMPFLAGS = -fopenmp 
OMPLIBS = -lgomp

CC = gcc
CPPFLAGS = -g $(OMPFLAGS)
LDFLAGS = -g $(OMPFLAGS)
LDLIBS = $(OMPLIBS)

MPICC = mpicc
MPICH = /usr/lib/openmpi
CFLAGS = -I$(MPICH)/include

all: mixedBarrier

mixedBarrier: mixedBarrier.o
		$(MPICC) -o $@ $(LDFLAGS) $^ $(LDLIBS) -lm

clean:
	rm -f *.o mixedBarrier

