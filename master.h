#pragma once
#include <mpi.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CONTINUE 5
#define STOP 6
#define ITERATION 500
#define TO_MASTER 3
#define TAG_ASK_FOR_JOB 1
#define TAG_FOR_RESULT 2
#define THRESHOLD 80
#define ACCEPTABLE_RANGE 10

void base_station_async(MPI_Comm world, MPI_Comm comm);
void *gen_infared(void *pArg);
