OMPLIBS = -lgomp

CC = mpicc
CPPFLAGS = -g 
LDFLAGS = -g 
LDLIBS = $(OMPLIBS)

MPICC = mpicc

all: tournament dissemination

tournament: tournament.o
		$(MPICC) -o $@ $(LDFLAGS) $^ $(LDLIBS) -lm
dissemination: dissemination.o
		$(MPICC) -o $@ $(LDFLAGS) $^ $(LDLIBS) -lm
clean:
	rm -f *.o tournament
	rm -f *.o dissemination

