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
    char fid, copyid;
    char pid = -1, finish = 0;
    int from = (id==0)?(p-1):(id-1);
    
    
    copyid = id;

    if(f==1) MPI_Isend(&copyid, 1, MPI_CHAR, (id+1)%p, FINISH, MPI_COMM_WORLD, &rs);
    MPI_Irecv(&fid, 1, MPI_CHAR, from, FINISH, MPI_COMM_WORLD, &rr );
    while(recv == 0){
        //trabalhjo.amawokdawd
        //awidampwd
        MPI_Test(&rr ,&flagr ,&s);
        recv = (flagr==0)?recv:1;
    }
    //printf("[%d] recebeu fid: %d\n", id, fid );
    if(f==1){
        while(send == 0){
            //trabalhjo.amawokdawd
            //awidampwd
            MPI_Test(&rs ,&flags ,&s);
            send = (flags==0)?send:1;
        }
    }

    if(fid >= id || f == 0){
        if(fid == id) printf("1st msg by [%d]\n", id );
        if(fid > pid)
            pid = fid;
    
        //printf("[%d] vai tentar enviar fid: %d\n", id, fid );
        MPI_Send(&fid, 1, MPI_CHAR, (id+1)%p, FINISH, MPI_COMM_WORLD);
        //printf("[%d] enviou  fid: %d\n", id, fid );
    }
    
    do{
        MPI_Recv(&fid, 1,MPI_CHAR, from, FINISH, MPI_COMM_WORLD, &s);
        //printf("[%d] recebeu fid: %d ---\n", id, fid );
        if(fid == pid || fid == id){
            if(fid == pid) finish = 1;
            if(pid != id){
                MPI_Send(&fid, 1, MPI_CHAR, (id+1)%p, FINISH, MPI_COMM_WORLD);
                //printf("[%d] enviou  fid: %d ---\n", id, fid );
            }
            pid = fid;
        }else if(fid > pid){
            printf("[%d]dawdaw fid:%d pid:%d\n",  id, fid, pid);
            pid = fid;
            //passas o que recebeste e espertas por outra igual pid > id = fid
            if(fid >= id){
                MPI_Send(&fid, 1, MPI_CHAR, (id+1)%p, FINISH, MPI_COMM_WORLD);
                //printf("[%d] enviou  fid: %d ---\n", id, fid );
            }
        }
    }while(finish == 0);

    if(fid == id){
        printf("\n[%d]IHNAWDIAWND\n\n", id);
    }
    MPI_Barrier(MPI_COMM_WORLD); 
    MPI_Finalize();
    
    return 0;
}


