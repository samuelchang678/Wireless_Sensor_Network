C = mpicc


all: main

main: asgn2_1.o 

asgn2_1.o: asgn2_1.c
	$(C) asgn2_1.c slave.c master.c -lm -lpthread

slave.o: slave.c
	$(C) slave.c -lm

master.o: master.c
	$(C) master.c -lm -lpthread
	
