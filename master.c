#include "master.h"
int * infared = NULL;
void *gen_infared(void *pArg)
{
    int size =*((int*)pArg);
    for (int i=0; i<size-1;i++ )
    {
        unsigned int seed = time(NULL)*rand();
        int temperature = (rand_r(&seed) % (90 - 70 + 1)) + 70;
        infared[i] = temperature;
    }
    
    return NULL;
} 
void base_station_async(MPI_Comm world, MPI_Comm comm){
    pthread_t tid[1];
    int ms =0;
    int size;
    int true_alert=0;
    int false_alert=0;
    int i,flag ;
    MPI_Comm_size(world, &size);
    infared = malloc(sizeof(int)*(size-1));
    int tmp[2];
    int term = 1;
    int send_array[1];
    MPI_Status stat,stat2;
    time_t now;
    struct tm ts;
    char buf[80];
    int buffer[2];
    int slave;
    
    struct timespec start, end, startComp, endComp;
    double time_taken; 
    clock_gettime(CLOCK_MONOTONIC, &start); 
    
    FILE *fp = fopen("log.txt","a");
  
    int stop =0;
    // BASE STATION
    do{
        tid[0] = 0;
        pthread_create(&tid[0], NULL, gen_infared,&size);
        pthread_join(tid[0], NULL);
        int flag = 0;
        int count =0;
        MPI_Bcast(&stop,1,MPI_INT,size-1,world);
        MPI_Request req;
        MPI_Status result_status;
        while (!flag){
            MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,world,&flag,&result_status);
        }
        MPI_Get_count( &result_status, MPI_INT, &count );
        //printf("count : %d source : %d \n",count,result_status.MPI_SOURCE);
        int slave_rank = result_status.MPI_SOURCE;
        if (ms >= ITERATION){
            //MPI_Send(&term,1,MPI_INT,slave_rank,STOP,world);
            stop=1;
        }
        else if (result_status.MPI_TAG == TAG_FOR_RESULT)
        {
            int source = result_status.MPI_SOURCE;
            int value ;
            int x, y;
            double t2,t1;
            char recv_time[80];
            char buffer_recv [140];
            int position =0 ;
            MPI_Recv(buffer_recv,140,MPI_PACKED,source,TAG_FOR_RESULT,world,&stat2);
            t2= MPI_Wtime();
            MPI_Unpack(buffer_recv, 140, &position, &value, 1, MPI_INT, MPI_COMM_WORLD);
            MPI_Unpack(buffer_recv, 140, &position, &t1, 1, MPI_DOUBLE, MPI_COMM_WORLD);
            MPI_Unpack(buffer_recv, 140, &position, &x, 1, MPI_INT, MPI_COMM_WORLD);
            MPI_Unpack(buffer_recv, 140, &position, &y, 1, MPI_INT, MPI_COMM_WORLD);
            MPI_Unpack(buffer_recv, 140, &position, recv_time, 80, MPI_CHAR, world);
            //MPI_Recv(&buffer,2,MPI_INT,source,TAG_FOR_RESULT,world,&stat2); 
            if (infared[source]>THRESHOLD) {
                if (abs(infared[source]-value)<=ACCEPTABLE_RANGE){
                    time(&now);
                    ts = *localtime(&now);
                    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &ts);
                    //printf("%d\n", ts);
                    //fprintf(fp, "%lun", (unsigned long)time(NULL)); 
                    //printf("time faks u  %s",recv_time);
                    fprintf(fp,"Time of Node:%s ,Current time:%s,Infared temp: %d Node temp: %d Source: %d Coordinate: (%d,%d) Alert: True , time : %3fs \n",recv_time,buf,infared[source],value,source,x,y,t2-t1);
                    true_alert++;
                }
            }
            else{
                time(&now);
                ts = *localtime(&now);
                strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &ts);
                //printf("time fak u %s",recv_time);
                fprintf(fp,"Time of Node:%s ,Current time:%s,Infared temp: %d Node temp: %d Source: %d Coordinate: (%d,%d) Alert: False , time : %3fs \n",recv_time,buf,infared[source],value,source,x,y,t2-t1);
                false_alert++;
            }
            ms++;
        }
        
    }while(stop==0);
    
    MPI_Bcast(&stop,1,MPI_INT,size-1,world);
    clock_gettime(CLOCK_MONOTONIC, &end); 
	time_taken = (end.tv_sec - start.tv_sec) * 1e9; 
    time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9; 

    fprintf(fp,"Total alerts received: %d , True: %d , False %d \n", true_alert+false_alert, true_alert,false_alert);
    fprintf(fp, "Total communication time %lf s\n", time_taken);
    fclose(fp);
    //printf("MASTER END\n");
}
