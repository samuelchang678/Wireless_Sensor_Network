#include "slave.h"

void sensor_io(MPI_Comm world, MPI_Comm comm,int nrows,int ncols){
    int ndims=2 ;
    int size, my_rank, reorder, my_cart_rank, ierr;
    int worldsize;
	int dims[ndims],coord[ndims];
	int wrap_around[ndims];
	time_t now;
	struct tm ts;
	char buf[80];
	
	char buffer[140];
	int position = 0 ;
	
	dims[0] = nrows; /* number of rows */
	dims[1] = ncols; /* number of columns */
	
	MPI_Comm_size(world,&worldsize);
	MPI_Comm_size(comm, &size);
	MPI_Comm_rank(comm, &my_rank);
	MPI_Dims_create(size, ndims, dims);
	MPI_Comm comm2D;
	MPI_Status status;
	MPI_Status status2;
	
	wrap_around[0] = 0;
	wrap_around[1] = 0; /* periodic shift is .false. */
	reorder = 0;
	ierr =0;
	ierr = MPI_Cart_create(comm, ndims, dims, wrap_around, reorder, &comm2D);
	if(ierr != 0) printf("ERROR[%d] creating CART\n",ierr);

    MPI_Cart_coords(comm2D, my_rank, ndims, coord); 
    MPI_Cart_rank(comm2D, coord, &my_cart_rank);
    int rank_array[4];
    
    //storing the adjacent nodes into an array 
    MPI_Cart_shift( comm2D, SHIFT_ROW, DISP, &rank_array[0], &rank_array[1] );
    MPI_Cart_shift( comm2D, SHIFT_COL, DISP, &rank_array[2], &rank_array[3] );
    
    //int term;
    MPI_Status stat;
    MPI_Status stat2;
    int stop=0;
    int term=0;
    double t1;

    
    do{
            int counter=0;
            int flag =0;

            MPI_Bcast(&term,1,MPI_INT,worldsize-1,world);
            unsigned int seed = rand()*my_rank;
            int temp = (rand_r(&seed) % (90 - 70 + 1)) + 70;
            //printf("value %d rank %d \n", temp, my_rank);
            //int temp =80 + rand() / (RAND_MAX / (90 - 80 + 1) + 1);
            //int temp=90;
            
            MPI_Request send_request[4];
            MPI_Request receive_request[4];
            MPI_Status send_status[4];
            MPI_Status receive_status[4];

            
            for (int i =0; i <4;i++){
                
	            MPI_Isend(&temp, 1, MPI_INT, rank_array[i], 0, comm2D, &send_request[i]);
	        }
	
	
	        //array for the received values
	        int recv_array [4];
	        //loop to received from each neighbour 
	        for (int i =0; i <4;i++){
                MPI_Irecv(&recv_array[i], 1, MPI_INT, rank_array[i], 0, comm2D, &receive_request[i]);
	        }
	        MPI_Waitall(4, send_request, send_status);
	        MPI_Waitall(4, receive_request, receive_status);

	        //printf("Rank %d has Temperature: %d. Recv Top: %d. Recv Bottom: %d. Recv Left: %d. Recv Right: %d.\n", my_rank,temp, recv_array[0], recv_array[1], recv_array[2], recv_array[3]);
	        if (temp>THRESHOLD){
	            for (int i =0; i <4;i++){
	                if (recv_array[i]>0){
	                    if(abs(temp-recv_array[i])>=ACCEPTABLE){
	                        counter++;
	                    }
	                }
	            }
	        }


            /*printf("rank: %d has temperatue of value: %d \n", my_rank, temp);
            if (temp>THRESHOLD){
                MPI_Request request;
                MPI_Status status;
                int no_of_negh;
                for (int i =0; i <4 ; i++){
                    if(rank_array[i] >0){
                        no_of_negh ++;
                    }
                    //printf("%d has rank %d \n", my_rank,rank_array[i]);
                    //printf("Rank %d requesting value from rank: %d \n", my_rank,rank_array[i]);
                    //MPI_Isend(&my_rank,1,MPI_INT, rank_array[i],NEIGHBOUR,comm2D,&request);
                    //MPI_Wait(request,status); 
                    MPI_Send(&my_rank, 1 , MPI_INT, rank_array[i], NEIGHBOUR, comm2D);
                    //MPI_Recv(&recv_array[i], 1, MPI_INT,rank_array[i],SENDING,comm2D,&status);    
                }
                int buffer;
                for( int i = 0; i<no_of_negh*2 ; i++){
                    //MPI_Send(&my_rank, 1 , MPI_INT, rank_array[i], NO_REQUEST, comm2D);
                    //printf( "rank %d has %d neighbours \n", my_rank,no_of_negh);
                    MPI_Recv(&buffer, 1, MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,comm2D,&status);
                    //printf("receiving");
                    //printf("rank %d receiving faker %d\n",my_rank, no_of_negh);
                    if (status.MPI_TAG==NEIGHBOUR){
                        //printf( "rank %d sending to %d  \n", my_rank,status.MPI_SOURCE);
                        MPI_Send(&temp,1,MPI_INT,status.MPI_SOURCE,SENDING,comm2D);
                    }
                    
                    else if(status.MPI_TAG==SENDING){
                     //printf( "rank %d receiving from %d with value %d \n", my_rank,status.MPI_SOURCE, buffer);
                        if(temp>0){
                           if (abs(temp-buffer)>=ACCEPTABLE){
                                counter ++;
                            } 
                        }  
                    }
                }
                //printf("exiting %d \n", my_rank);
            }  
            else{
                int buffer;
                int no_of_negh;
                for (int i =0; i <4 ; i++){
                    if(rank_array[i] >0){
                        no_of_negh ++;
                    }
                    //printf("requesting value from rank: %d \n", rank_array[i]);
                    MPI_Send(&my_rank, 1 , MPI_INT, rank_array[i], NO_REQUEST, comm2D);
                    //MPI_Recv(&recv_array[i], 1, MPI_INT,rank_array[i],SENDING,comm2D,&status);    
                } 
                for( int i = 0; i< no_of_negh; i++){
                    MPI_Recv(&buffer, 1, MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,comm2D,&status);
                    
                    if (status.MPI_TAG==REQUEST){
                        MPI_Send(&temp,1,MPI_INT,status.MPI_SOURCE,SENDING,comm2D);
                    }
                }
            }*/
            if (counter>2){  
               int send_array[2]; 
               send_array[0] = temp;
               send_array[1] = time(NULL);
               MPI_Request req;
               MPI_Status result_status;
               
               time(&now);
               
               ts = *localtime(&now);
               strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &ts);
               t1 = MPI_Wtime();
               position =0;
               MPI_Pack(&temp,1,MPI_INT,buffer,140,&position,MPI_COMM_WORLD);
               MPI_Pack(&t1,1,MPI_DOUBLE,buffer,140,&position,MPI_COMM_WORLD);
               MPI_Pack(&coord[0],1,MPI_INT,buffer,140,&position,MPI_COMM_WORLD);
               MPI_Pack(&coord[1],1,MPI_INT,buffer,140,&position,MPI_COMM_WORLD);
               MPI_Pack(buf,80,MPI_CHAR,buffer,140,&position,MPI_COMM_WORLD);
               MPI_Send(buffer,position,MPI_PACKED,worldsize-1,TAG_FOR_RESULT,world);
               //MPI_Send(&send_array,2,MPI_INT,worldsize-1,TAG_FOR_RESULT,world);
               //printf("Sent to master node %d from process %d\n",send_array[0],my_rank);
               counter =0;
            } 
        
        MPI_Barrier(comm2D);
     }while(term==0);
    //printf("TERMINATED loop\n");
    /*
    MPI_Send(&term,1,MPI_INT,worldsize-1,TAG_ASK_FOR_JOB,world);
    int count =0;
    MPI_Request req;
    MPI_Status result_status;
    while (!flag){
        MPI_Iprobe(worldsize-1,MPI_ANY_TAG,world,&flag,&result_status);
    }
    MPI_Recv(&term,1,MPI_INT,worldsize-1,STOP,world,&stat2);
    printf("TERMINATED\n");
    */
     MPI_Comm_free( &comm2D );
 }

