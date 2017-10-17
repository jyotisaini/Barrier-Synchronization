OMPLIBS = -lgomp

CC = gcc
CPPFLAGS = -g 
LDFLAGS = -g 
LDLIBS = $(OMPLIBS)

MPICC = mpicc
MPICH = /opt/openmpi-1.4.3-gcc44
CFLAGS = -I$(MPICH)/include

all: tournament dissemination

tournament: tournament.o
		$(MPICC) -o $@ $(LDFLAGS) $^ $(LDLIBS) -lm
dissemination: dissemination.o
		$(MPICC) -o $@ $(LDFLAGS) $^ $(LDLIBS) -lm
clean:
	rm -f *.o tournament
	rm -f *.o dissemination

