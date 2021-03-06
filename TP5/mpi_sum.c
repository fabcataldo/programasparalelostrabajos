#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

int sum(int *v, int size){
	int i;
	int suma=0;
		for(i=0;i<size;i++){	
			suma+=v[i];	
		}
	return suma;
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
		if(fgetc(archivo) == '\n')
			break;	
	}while((fgetc(archivo)) != '\n');
	fclose(archivo);
	return tamanio_vector;
}

void read_file(char* nomb_file, int* vector, int size){
	FILE *fp = fopen(nomb_file, "r");
	int i;
	if (fp == NULL)
		return;
	if(size > 10){
		fseek(fp, 3, SEEK_SET);
	}
	else{
		fseek(fp, 2, SEEK_SET);
	}
	do{
		fscanf(fp, "%d", &(vector[i]));
		i++;		
	}while((fgetc(fp)) != EOF && i<size);
	
	fclose(fp);
}

int main(int argc, char* argv[]){
	char* nombre_archivo = argv[1];

	if(argv[1]==NULL){
		fprintf(stderr, "Argumento inválido\n");
		return 1;
	}
	else{
		int tamanio_vec = obtener_tamanio(nombre_archivo);
		int* valores = (int*)malloc(tamanio_vec*sizeof(int));
		read_file(nombre_archivo, valores, tamanio_vec);

		int rank, p, src, dst, tag=0, srcmain;
		int mensaje=0; //inicializo el lugar a donde va a el resultado del sum()	
		double start1,end1,t1;
		MPI_Status status;

		MPI_Init(&argc, &argv);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Comm_size(MPI_COMM_WORLD, &p);

		if(rank==0){
			start1 = MPI_Wtime(); 
			for(src = 1; src<p; src++){
				dst=src; //el numero del rango del primer proceso, por lo menos, es 1, los demás son >1
			
				MPI_Send(&mensaje, 1, MPI_INT, dst, tag, MPI_COMM_WORLD);	

				MPI_Recv(&mensaje, 1, MPI_INT, src, tag, MPI_COMM_WORLD, &status);
					
			}
			end1 = MPI_Wtime();
			t1=end1-start1;
			printf("Suma total: %d\n", mensaje);
			fprintf(stderr, "Tiempo transcurrido: %lf\n", t1);
		}	
		else{//rango del main
			//rangos de los otros procesos
			srcmain=0;
			dst=srcmain;

			MPI_Recv(&mensaje, 1, MPI_INT, srcmain, tag, MPI_COMM_WORLD, &status);
			mensaje=sum(valores, tamanio_vec);
			MPI_Send(&mensaje, 1, MPI_INT, dst, tag, MPI_COMM_WORLD);
		}	
		MPI_Finalize();	
		free(valores);
	}
	
	return 0;
}
