OMPFLAGS = -fopenmp 
OMPLIBS = -lgomp -lm

CC = gcc
CPPFLAGS = -g $(OMPFLAGS)
LDFLAGS = -g $(OMPFLAGS)
LDLIBS = $(OMPLIBS)

all: disseminationBarrier mcsBarrier

disseminationBarrier: disseminationBarrier.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

mcsBarrier: mcsBarrier.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)


clean:
	rm -f *.o disseminationBarrier mcsBarrier 
