#include "slave.h"
#include "master.h"

int main(int argc, char *argv[])
{
    int nrows, ncols;
    int size, my_rank;
	
    MPI_Comm new_comm;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
    if (argc == 3) {
		nrows = atoi(argv[1]);
		ncols = atoi(argv[2]);
		if( (nrows*ncols) != size-1) {
			if( my_rank ==0) printf("ERROR: nrows*ncols)=%d * %d = %d != %d\n", nrows, ncols, nrows*ncols,size-1);
			
			MPI_Finalize(); 
			return 0;
		}
	} else {
		nrows=ncols=(int)sqrt(size-1);
	}
    MPI_Comm_split( MPI_COMM_WORLD,my_rank == size-1, 0, &new_comm);
    if (my_rank == size-1){
	    base_station_async( MPI_COMM_WORLD, new_comm );
	}
    else{
	    sensor_io( MPI_COMM_WORLD, new_comm ,nrows,ncols);
    }
   MPI_Barrier(MPI_COMM_WORLD);
    MPI_Comm_free(&new_comm);
    MPI_Finalize();
    return 0;
}

