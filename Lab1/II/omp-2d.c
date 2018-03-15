/*
DESCRIPTION:
 		Parallelizing an inner loop with dependences

		for (iter=0; iter<numiter; iter++) {
 			for (i=0; i<size-1; i++) {
 				V[i] = f( V[i], V[i+1] );
 			}
 		}

**************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include <omp.h>

#define TOTALSIZE 1000
#define NUMITER 200

/*
 * * DUMMY FUNCTION
 * */
#define f(x,y)	((x+y)/2.0)


/* MAIN: PROCESS PARAMETERS */
int main(int argc, char *argv[]) {

  /* VARIABLES */
  int i, iter;

  /* DECLARE VECTOR AND AUX DATA STRUCTURES */
  double *V = (double *) malloc(TOTALSIZE * sizeof(double));

  /* 1. INITIALIZE VECTOR */
  for(i = 0; i < TOTALSIZE; i++) {
      V[i]= 0.0 + i;
  }

#pragma omp parallel private(i,iter)
  {
    int tid = omp_get_thread_num();
    int nt = omp_get_num_threads();
    int border = (tid + 1 == nt) ? TOTALSIZE - 1 : (TOTALSIZE / nt) * (tid + 1);
    int start = TOTALSIZE / nt * tid;

        /* 2. ITERATIONS LOOP */
        for(iter = 0; iter < NUMITER; iter++) {
            double tmp = V[border];

            #pragma omp barrier
            /* 2.1. PROCESS ELEMENTS */
            for(i = start; i < border; i++) {
                V[i] = (i+1 == border) ? f(V[i], tmp) : f(V[i], V[i+1]);
            }
            /* 2.2. END ITERATIONS LOOP */
             #pragma omp barrier /* Garantir que ninguém trabalha com valores antigos */
        }


    }

    /* 3. OUTPUT FINAL VALUES */
    printf("Output:\n");
    for(i = 0; i < TOTALSIZE; i++) {
      printf("%4d %f\n", i, V[i]);
    }
}
