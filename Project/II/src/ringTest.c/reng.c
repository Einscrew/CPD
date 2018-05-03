#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include <unistd.h>

#define FINISH 123


/****************************************************************************
*    Allocs a board, fills that board and find a solution if there's one    *
*                                        			   						*
* Returns: 0 at exit     		   											*        								    
****************************************************************************/

int main(int argc, char *argv[]) {

    int id, p;
    // MPI_INIT & MPI_COMM
    MPI_Init (&argc, &argv);
    
    MPI_Comm_rank (MPI_COMM_WORLD, &id);
    MPI_Comm_size (MPI_COMM_WORLD, &p);

    int f = 0, flags = 0, flagr = 0, send = 0, recv = 0;

    for (int i = 1; i < argc; ++i)
    {
        if(atoi(argv[i]) == id){
            f=1;
        }
    }
    MPI_Request rr;
    MPI_Request rs;
    MPI_Status s;
    char fid, idcopy;
    char pid = -1, finish = 0;
    int from = (id==0)?(p-1):(id-1);
    
    if(f==0){
        //printf("[%d]NOT FINISH\n", id );
        while(finish == 0){
            MPI_Irecv(&fid, 1, MPI_CHAR, from, FINISH, MPI_COMM_WORLD, &rr );
            while(!flagr){
                MPI_Test(&rr,&flagr,&s);
            }
            recv++;
            printf("[%d]recebeu fid: %d\n", id, fid);
            MPI_Send(&fid, 1, MPI_CHAR, (id+1)%p, FINISH, MPI_COMM_WORLD);
            printf("[%d]enviou fid: %d para: %d\n", id, fid, (id+1)%p);
            send++;
            if(pid == fid)
                finish++;

            if(pid == -1)
                pid=fid;
        }
    }
    else
    {
        idcopy = id;
        while(finish == 0){
            MPI_Irecv(&fid, 1, MPI_CHAR, from, FINISH, MPI_COMM_WORLD, &rr );
            MPI_Send(&idcopy, 1, MPI_CHAR, (id+1)%p, FINISH, MPI_COMM_WORLD, &rs);
            while(recv == 0 || send == 0){
                
                MPI_Test(&rr ,&flagr ,&s);
                recv = (flagr==0)?recv:1;
                MPI_Test(&rs ,&flags ,&s);
                send = (flagr==0)?send:1;
            }
            printf("[%d]recebeu fid: %d\n", id, fid);
            printf("[%d]enviou idcopy: %d para: %d\n", id, idcopy, (id+1)%p);
            if(pid == fid)
                finish++;

            if(pid == -1)
                pid=fid;
            if (fid > idcopy)
            {
                idcopy = fid;
            }
        }
        if(fid == id){
            printf("[%d] sou eu fid: %d\n", id, fid);
        }

    }
    MPI_Barrier(MPI_COMM_WORLD); 



    MPI_Finalize();
    
    return 0;
}


