#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#define SIZEPROC 4
#define SIZEMAT 8

void mostrarmatriz(int* matriz, int nrm, int ncm){
	int i,j;
	for(i=0;i<nrm;i++){
		for(j=0;j<ncm;j++){
			printf("%d ",matriz[i*ncm+j]);		
		}
		printf("\n");
	}
}

void cargar_matriz(int* matriz, int nrm, int ncm){
	int i,j;
	for(i=0;i<nrm;i++){
		for(j=0;j<ncm;j++){
			matriz[i*ncm+j]=(10*i)+j;
		}
	}
}

void cargar_b(int *v, int n){
	int i, j;
	for(i=0;i<n;i++){
		for(j=0;j<n;j++){
			v[i*n+j]=0;
		}
	}
}

int main(int argc, char *argv[]){
	int p, rank, source=0, dest,tag=1, i,j,k;
	int* a = NULL;

	MPI_Status stat;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	if (p == SIZEPROC) {
		a = (int*)malloc(SIZEMAT*SIZEMAT*sizeof(int));
		if(rank==0){
			printf("\nProceso 0\n");
			printf("\nMatriz inicial\n");
			cargar_matriz(a, SIZEMAT, SIZEMAT);
			mostrarmatriz(a, SIZEMAT, SIZEMAT);
		}
		//mando la matriz a todos los procesos, y éstos van a llamar a MPI_Bcast para recibir
		MPI_Bcast(a, SIZEMAT*SIZEMAT, MPI_INT, 0, MPI_COMM_WORLD);  
		
		printf("\nContigua\n");
		mostrarmatriz(a, SIZEMAT, SIZEMAT);

		MPI_Finalize();
	}
	else{
		printf("Must specify %d processors. Terminating.\n",SIZEPROC);
		MPI_Finalize();
	}
	
	free(a);
	return 0;
}
