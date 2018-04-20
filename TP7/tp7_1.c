#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>


void vervector(int *vector, int size){
	int i;
	for(i=0;i<size;i++){
		printf("\n%d",vector[i]);
	}
	printf("\n");
}

void cargarvector(int *vector, int size){
	int i;
	for(i=0;i<size;i++){
		if(i==0){
			vector[i]=0;		
		}
		else{
			if(i==size-1){
				vector[i]=0;		
			}
			else{
				vector[i]=i+1;
			}
		}

	}
}

void calculoiterativo(int *vector, int size, int valormenosuno, int valormasuno){
	int i,c=0,aux;
	int* vecviejo=(int*)malloc(size*sizeof(int));
	for(i=0;i<size;i++){
		vecviejo[i]=vector[i]; //0,2
	}
	aux=vector[1];
	for(i=0;i<size;i++){
		if(i>0){
			if(i==size-1){	
				valormenosuno=vecviejo[i-1];
				aux=valormasuno;
			}
			else{
				valormenosuno=vecviejo[i-1];
				aux=vecviejo[i+1]; 
			}
			}
		vector[i]=valormenosuno-vector[i]+aux;
	}	
	free(vecviejo); 
}

int main(int argc, char *argv[]){
	if(argv[1]==0){ //tamaño del vector
		fprintf(stderr, "Argumentos inválidos\n");
		return 1;		
	}
	else{
		if(argv[2]==0){ //cant de iteraciones
			fprintf(stderr, "Argumentos inválidos\n");
			return 1;
		}	
	}
	int p, rank,j=0;
	
	MPI_Status stat;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	
	int size=atoi(argv[1]);
	int cantiter=atoi(argv[2]);

	int sizeporp = size/p;
	int *vector = (int*)malloc(size*sizeof(int));
	int *buffrecep = (int*)malloc((sizeporp)*sizeof(int));	
		
	int valorbordes=0; //bordes del primer proceso y del último
	int valormenosuno=0;//borde izquierdo del proceso de los de "entremedio"
	int valormasuno=0;//borde derecho del proceso de los de "entremedio"
	int valoraenviar=0;
	int valorrecibido=0;
	
	MPI_Request req;
	int flag;
	MPI_Status status;

	if(rank==0){
		cargarvector(vector, size);	
		printf("\nVector inicial");		
		vervector(vector,size);
	}
	MPI_Scatter(vector, sizeporp, MPI_INT, buffrecep, sizeporp, MPI_INT, 0, MPI_COMM_WORLD);
	/*
	printf("\nSubvector inicial del proceso %d",rank);	
	vervector(buffrecep, sizeporp);
	printf("\n\n");
	*/
	while(j!=cantiter){
		if(rank==0){
			MPI_Isend(&buffrecep[sizeporp-1], 1, MPI_INT, (rank+1), 0, MPI_COMM_WORLD, &req);
			MPI_Wait(&req, MPI_STATUS_IGNORE);
			//MPI_Test(&req, &flag, &status);
			//printf("FLAG isendDER 00: %d",flag);
			MPI_Irecv(&valormasuno, 1, MPI_INT, (rank+1), 0, MPI_COMM_WORLD, &req);
			MPI_Wait(&req, MPI_STATUS_IGNORE);
			//MPI_Test(&req, &flag, &status);
			//printf("FLAG irecvDER 000 : %d",flag);
		
			calculoiterativo(buffrecep, sizeporp, 0, valormasuno);	
			/*			
			printf("\nSubvector nuevo del proceso 0");	
			vervector(buffrecep, sizeporp);
			printf("\n\n");
			*/
			//printf("Proceso 0000\nvalormasuno: %d\nvalor a enviar: %d\n",valormasuno,buffrecep[sizeporp-1]);
		}
		else{	
			if(rank==p-1){
				MPI_Isend(&buffrecep[0], 1, MPI_INT, (rank-1), 0, MPI_COMM_WORLD, &req);
				MPI_Wait(&req, MPI_STATUS_IGNORE);
				//MPI_Test(&req, &flag, &status);
				//printf("FLAG isendIZQ UUULT: %d",flag);
			
				MPI_Irecv(&valormenosuno, 1, MPI_INT, (rank-1), 0, MPI_COMM_WORLD, &req);
				MPI_Wait(&req, MPI_STATUS_IGNORE);
				//MPI_Test(&req, &flag, &status);
				//printf("FLAG irecvIZQ UULT: %d",flag);
			
				calculoiterativo(buffrecep, sizeporp, valormenosuno, 0);	
					
				/*printf("\nSubvector nuevo del proceso %d",p-1);	
				vervector(buffrecep, sizeporp);
				printf("\n\n");
				*/

				//printf("Proceso %d\nvalormenosuno: %d\n", p-1, valormenosuno);	
			}
			else{
				MPI_Isend(&buffrecep[0], 1, MPI_INT, (rank-1), 0, MPI_COMM_WORLD, &req);
				MPI_Wait(&req, MPI_STATUS_IGNORE);
				//MPI_Test(&req, &flag, &status);
				//printf("FLAG isendIZQ: %d\n",flag);
				MPI_Irecv(&valormenosuno, 1, MPI_INT, (rank-1), 0, MPI_COMM_WORLD, &req);
				MPI_Wait(&req, MPI_STATUS_IGNORE);
				//MPI_Test(&req, &flag, &status);
				//printf("FLAG irecvIZQ: %d\n",flag);
				MPI_Isend(&buffrecep[sizeporp-1], 1, MPI_INT, (rank+1), 0, MPI_COMM_WORLD, &req);
				MPI_Wait(&req, MPI_STATUS_IGNORE);
				//MPI_Test(&req, &flag, &status);
				//printf("FLAG isendDER: %d\n",flag);
				MPI_Irecv(&valormasuno, 1, MPI_INT, (rank+1), 0, MPI_COMM_WORLD, &req);
				MPI_Wait(&req, MPI_STATUS_IGNORE);
				//MPI_Test(&req, &flag, &status);
				//printf("FLAG irecvDER: %d\n",flag);

			
				calculoiterativo(buffrecep, sizeporp, valormenosuno, valormasuno);	
				/*printf("\nSubvector nuevo del proceso %d",rank);	
				vervector(buffrecep, sizeporp);
				printf("\n\n");
				*/
			
				//printf("Proceso %d\nvalormenosuno: %d\n,valor a enviar IZQ: %d asdads\n, valor a enviar DER: %dadsasd\n",rank, valormenosuno,buffrecep[0], buffrecep[sizeporp-1]);
			}
		}

		//usar MPI_Gather() para obtener todos los buffrecep y pasarlos a vector, y luego mostrar vector, el nuevo vector ya actualizado
		MPI_Gather(buffrecep, sizeporp, MPI_INT, vector, sizeporp, MPI_INT, 0, MPI_COMM_WORLD);
	
		if(rank==0){
			printf("\n\nNuevo vector FINAL iteracion %d: \n",j);	
			vervector(vector, size);
			printf("\n\n");	
			
		}
		j++;
	}


	free(vector);
	free(buffrecep);
	MPI_Finalize();
	return 0;
}
