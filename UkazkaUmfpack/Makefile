all: poisson

CPP = g++
CPPFLAGS = -O2
CC = gcc
CCFLAGS = -O2

%.o: %.c
	$(CC) $(CCFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ $<

poisson: main_umfpack.o solver_umfpack.o
	$(CPP) -o poisson main_umfpack.o solver_umfpack.o -lumfpack

clean:
	rm -f *.o poisson
