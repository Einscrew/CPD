#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {

    MPI_Status status;
    int id, p, i, j, rounds;
    double secsO, secsB;
    int * arr = NULL;
    
    MPI_Init (&argc, &argv);

    MPI_Comm_rank (MPI_COMM_WORLD, &id);
    MPI_Comm_size (MPI_COMM_WORLD, &p);

    if(argc != 3){
	if (!id) printf ("Command line: %s <n-rounds> <array size>\n", argv[0]);
	MPI_Finalize();
	exit (1);
    }
    rounds = atoi(argv[1]);
    arr = malloc(sizeof(int)*atoi(argv[2]));
    
    MPI_Barrier (MPI_COMM_WORLD);
    secsO = - MPI_Wtime();

    for(i = 0; i < rounds; i++){
	if(!id){
	    for( j = 1; j < p ; j++){
	    	 MPI_Send(arr, 10000, MPI_INT, j, i, MPI_COMM_WORLD);
/*		 MPI_Recv(arr, 10000, MPI_INT, j, i, MPI_COMM_WORLD, &status);*/
	    }
	}
	else{
	    MPI_Recv(arr, 10000, MPI_INT, 0, i, MPI_COMM_WORLD, &status);
/*	    MPI_Send(arr, 10000, MPI_INT, 0, i, MPI_COMM_WORLD);*/
	}
    }

    MPI_Barrier (MPI_COMM_WORLD);
    secsO += MPI_Wtime();
    secsB = -MPI_Wtime();
    for(i = 0; i < rounds; i++){
        MPI_Bcast(arr, 10000, MPI_INT, 0, MPI_COMM_WORLD);
    }

    MPI_Barrier (MPI_COMM_WORLD);
    secsB +=MPI_Wtime();
    if(!id){
        printf("Time: %6.2fs VS %6.2f\nRatio: %6.2f\n", secsO, secsB, secsO/secsB);
    }
    MPI_Finalize();
    return 0;
}
