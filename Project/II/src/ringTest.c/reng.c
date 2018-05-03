#include <mpi.h>
#include <omp.h>
#include <unistd.h>

#define FINISH 123
#define PRINT 231
char found = FALSE;
Board * finalBoard = NULL;
char print = FALSE;


/****************************************************************************
*    Allocs a board, fills that board and find a solution if there's one    *
*                                        			   						*
* Returns: 0 at exit     		   											*        								    
****************************************************************************/

int main(int argc, char *argv[]) {


    // MPI_INIT & MPI_COMM
    MPI_Init (&argc, &argv);
    
    MPI_Comm_rank (MPI_COMM_WORLD, &id);
    MPI_Comm_size (MPI_COMM_WORLD, &p);

    int f = 0, flag = 0;

    for (int i = 1; i < argc; ++i)
    {
        if(atoi(argv[i]) == id){
            f=1;
        }
    }
    MPI_Request r;
    MPI_Status s;
    char fid;

    MPI_Irecv(&fid, 1, MPI_CHAR, (id==0)?p-1:id-1, FINISH, MPI_COMM_WORLD, &r );
    
    if(f==0){
        while(!flag){
            MPI_Test(&r,&flag,&s);
        }
        MPI_Send(&trash, 1, MPI_CHAR, fid, PRINT, MPI_COMM_WORLD);
    }
    else
    {
        sleep(random()%5);
        while(!flag){
            MPI_Test(&r,&flag,&s);
        }
        
        MPI_Send(&trash, 1, MPI_CHAR, fid, PRINT, MPI_COMM_WORLD);

    }

    MPI_Finalize();
    
    return 0;
}


