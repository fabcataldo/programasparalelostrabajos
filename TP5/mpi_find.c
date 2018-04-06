#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

int find(int value, int *vector, int size){
	int i;
	for(i=0;i<size;i++){
		if(value==vector[i]){
			return i;
		}
	}
	return -1;
}

void ver_vector(int *vector, int size){
	int i;
	for(i=0;i<size;i++){
		printf("\n%d",vector[i]);
	}
}

int obtener_tamanio(char* nomb_file){
	FILE *archivo = fopen(nomb_file, "r");
	int tamanio_vector=0;
	int i=0;
	
	if (archivo == NULL)
		return 1;
	do{
		fscanf(archivo, "%d", &(tamanio_vector));
		i++;		
	}while((fgetc(archivo)) != '\n');
	fclose(archivo);
	return tamanio_vector;
}

void read_file(char* nomb_file, int* vector, int size){
	FILE *fp = fopen(nomb_file, "r");
	int i;
	if (fp == NULL)
		return;
	fseek(fp, 3, SEEK_SET);
	do{
		fscanf(fp, "%d", &(vector[i]));
		i++;		
	}while((fgetc(fp)) != EOF && i<size);
	
	fclose(fp);
}

int main(int argc, char* argv[]){
	if(argv[1] == NULL && argv[2] == NULL){
		fprintf(stderr, "Argumentos inválidos\n");
		return 1;
	}
	if(argv[1] == NULL){
		fprintf(stderr, "Numero de busqueda inválido\n");
		return 1;
	}	
	if(argv[2] == NULL){
		fprintf(stderr, "Nombre de archivo inválido\n");
		return 1;
	}	
	else{
		int valor_a_buscar = atoi(argv[1]);
		char* nombre_archivo = argv[2];
		int tamanio_vec = obtener_tamanio(nombre_archivo);
		int* valores = (int*)malloc(tamanio_vec*sizeof(int));
		read_file(nombre_archivo, valores, tamanio_vec);

		int rank, p, src, dst, tag=0, srcmain;
		int mensaje=0; //inicializo el lugar a donde va a el resultado del find()	
		double start1,end1,t1;

		MPI_Status status;
		MPI_Request *request;
		MPI_Init(&argc, &argv);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Comm_size(MPI_COMM_WORLD, &p);

		if(rank==0){
			start1 = MPI_Wtime(); 
			for(src = 1; src<p; src++){
				dst=src; //el numero del rango del primer proceso, por lo menos, es 1, los demás son >1
					
				MPI_Isend(&valor_a_buscar, 1, MPI_INT, dst, tag, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);			
				MPI_Irecv(&mensaje, 1, MPI_INT, src, tag, MPI_COMM_WORLD, &status);		
				end1 = MPI_Wtime();
				t1=end1-start1;
				printf("índice del elemento buscado: %d\n", mensaje);
				fprintf(stderr, "Tiempo transcurrido: %lf\n", t1);		
			}
			
		}	
		else{//rango del main
			//rangos de los otros procesos
			srcmain=0;
			dst=srcmain;

			MPI_Irecv(&valor_a_buscar, 1, MPI_INT, srcmain, tag, MPI_COMM_WORLD, &status);
			mensaje=find(valor_a_buscar, valores, tamanio_vec);

			MPI_Isend(&mensaje, 1, MPI_INT, dst, tag, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);
		}

		MPI_Finalize();
		free(valores);
		return 0;
	}
	
}
