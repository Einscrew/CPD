#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char * argv [] ){

        int i, p, name_len;
        char name[MPI_MAX_PROCESSOR_NAME];

        MPI_Init(&argc, &argv);

        MPI_Comm_rank(MPI_COMM_WORLD, &i);
        MPI_Comm_size(MPI_COMM_WORLD, &p);


        MPI_Get_processor_name(name, &name_len);

        printf("Process %d send greetings from machine %s!\n", i, name);
        fflush(stdout);

        MPI_Finalize();
        return 0;
}
