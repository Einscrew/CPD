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
#include<omp.h>

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

  int tid , nt;
  double acc;

    /* 2. ITERATIONS LOOP */
  for(iter = 0; iter < NUMITER; iter++) {
    #pragma omp parallel private(i, tid, acc)
    {
     nt = omp_get_num_threads();
     tid = omp_get_thread_num();

     for( i = 0; i < TOTALSIZE-1 ; i+=nt) {
       if(i+tid < TOTALSIZE - 1){
         acc = f(V[i+tid],V[i+1+tid]);
       }
       #pragma omp barrier
       if(i+tid < TOTALSIZE - 1 ){
         V[i+tid]= acc;
       }
      }
    }
   }

  /* 2.2. END ITERATIONS LOOP */
  }


  /* 3. OUTPUT FINAL VALUES */
  printf("Output:\n");
  for(i = 0; i < TOTALSIZE; i++) {
    printf("%4d %f\n", i, V[i]);
  }      

}
