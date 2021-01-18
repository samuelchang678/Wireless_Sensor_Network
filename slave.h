#pragma once
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define THRESHOLD 80
#define ACCEPTABLE 0
#define REQUEST 1
#define NO_REQUEST 2
#define SENDING 4
#define NEIGHBOUR 0
#define TO_MASTER 3
#define SHIFT_ROW 0
#define SHIFT_COL 1
#define DISP 1
#define CONTINUE 5
#define TAG_ASK_FOR_JOB 1
#define TAG_FOR_RESULT 2
#define STOP 6

void sensor_io(MPI_Comm world, MPI_Comm comm,int row,int col);
