OMPLIBS = -lgomp

CC = gcc
CPPFLAGS = -g 
LDFLAGS = -g 
LDLIBS = $(OMPLIBS)

MPICC = mpicc
MPICH = /usr/lib/openmpi
CFLAGS = -I$(MPICH)/include

all: tournament

tournament: tournament.o
		$(MPICC) -o $@ $(LDFLAGS) $^ $(LDLIBS) -lm
clean:
	rm -f *.o tournament

