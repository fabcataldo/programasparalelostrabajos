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
	//int* b = NULL;

	MPI_Status stat;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	/*
	MPI_Datatype porfilas;
	//defino el nuevo tipo de datos "porfilas"
	MPI_Type_contiguous(SIZEMAT, MPI_DOUBLE, &porfilas);
	MPI_Type_commit(&porfilas);

	//lleno el buffer de recepción
	b = (int*)malloc(SIZEMAT*SIZEMAT*sizeof(int));
	cargar_b(b, SIZEMAT);
	*/

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
		
		/*
		for(k=0;k<2;k++){
			for(i=0;i<9;i+2){
				for(j=0;j<9;j++){
					b[k*SIZEMAT+j]=a[i*SIZEMAT+j];


			//mando 2 filas de 8 elementos a cada proceso, y cada proceso va a recibirlos en el buffer b
			MPI_Scatter(&b, 1, MPI_DOUBLE, b, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);	
		}
		*/
		printf("\nContigua\n");
		mostrarmatriz(a, SIZEMAT, SIZEMAT);

		/*	printf("\nPor filas\n");
			printf("\n%lf %lf \n",b[0], b[1]); //primer elemento, la fila i, y el otro elemento, la fila i+1				
		*/
	}
	else{
		printf("Must specify %d processors. Terminating.\n",SIZEPROC);
	}
	
	free(a);
	/*	
	free(b);
	MPI_Type_free(&porfilas);
	*/
	MPI_Finalize();
	return 0;
}
