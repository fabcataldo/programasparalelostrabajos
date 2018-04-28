#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"


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
		vector[i]=0;
	}
	vector[(size-1)/2]=1;
}

void generararchivo(int* vec, int size) {
	FILE *ptrfile=fopen("outputmpi.dat","w+");
	int i;
	for(i=0;i<size;i++){
		if(vec[i]==1){
			fprintf(ptrfile,"%d ",i);		
		}
	}
	fclose(ptrfile);
}

void calculoiterativo(int *vector, int size, int valormenosuno, int valormasuno){
	int i,j,aux;
	int* vecviejo=(int*)malloc((size)*sizeof(int));

	//decidí hacer una copia del vector que viene por parámetro, y del mismo tamaño que él
	//en vez de un vector más grande con dos elementos adicionales (los dos ceros artificiales en los bordes)
	//ya que se me hace más entendible así el problema, y evito tener ciertas complicaciones a la hora de 
	//recorrer el vector para llevarlo a un nuevo estado
	//los bordes que correspondan los mando en el main, y luego aquí los modifico de acuerdo a qué valor esté calculando
	for(i=0;i<size;i++){
		vecviejo[i]=vector[i]; 
	}
	aux=vector[1];

	if(((valormenosuno+vector[0])+(valormenosuno*vector[0])+(valormenosuno*vector[0]*aux))%2 == 0) 
		vector[0]=0;
	else
		vector[0]=1;

	for(i=1;i<=size;i++){
		valormenosuno=vecviejo[i-1];
		aux=vecviejo[i+1]; 

		if(((valormenosuno+vector[i])+(valormenosuno*vector[i])+(valormenosuno*vector[i]*aux))%2 == 0) 
			vector[i]=0;
		else
			vector[i]=1;
	}	

	valormenosuno=vecviejo[size-2];
	aux=valormasuno;
	if(((valormenosuno+vector[size-1])+(valormenosuno*vector[size-1])+(valormenosuno*vector[size-1]*aux))%2 == 0) 
		vector[size-1]=0;
	else
		vector[size-1]=1;

	free(vecviejo); 
}

int main(int argc, char *argv[]){
	if(argv[1]==0){ //tamaño del estado
		fprintf(stderr, "Argumentos inválidos\n");
		return 1;		
	}
	else{
		if(argv[2]==0){ //cant de iteraciones o generaciones
			fprintf(stderr, "Argumentos inválidos\n");
			return 1;
		}	
	}
	int p, rank,j=1;
	double starttime,stoptime,totaltime;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	
	int size=atoi(argv[1]);
	int cantiter=atoi(argv[2]);

	int sizeporp = size/p;
	int *vector = (int*)malloc(size*sizeof(int));
	int *buffrecep = (int*)malloc((sizeporp)*sizeof(int));	
		
	int valorbordes=0;
	int valormenosuno=0;
	int valormasuno=0;
	int valoraenviar=0;
	int valorrecibido=0;
	
	MPI_Request req;

	if(rank==0){
		cargarvector(vector, size);	
	}
	MPI_Scatter(vector, sizeporp, MPI_INT, buffrecep, sizeporp, MPI_INT, 0, MPI_COMM_WORLD);

	while(j<=cantiter){
		starttime=MPI_Wtime();
		if(rank==0){
			MPI_Isend(&buffrecep[sizeporp-1], 1, MPI_INT, (rank+1), 0, MPI_COMM_WORLD, &req);
			MPI_Wait(&req, MPI_STATUS_IGNORE);
			MPI_Irecv(&valormasuno, 1, MPI_INT, (rank+1), 0, MPI_COMM_WORLD, &req);
			MPI_Wait(&req, MPI_STATUS_IGNORE);
			calculoiterativo(buffrecep, sizeporp, 0, valormasuno);
		}
		else{	
			if(rank==p-1){
				MPI_Isend(&buffrecep[0], 1, MPI_INT, (rank-1), 0, MPI_COMM_WORLD, &req);
				MPI_Wait(&req, MPI_STATUS_IGNORE);
				MPI_Irecv(&valormenosuno, 1, MPI_INT, (rank-1), 0, MPI_COMM_WORLD, &req);
				MPI_Wait(&req, MPI_STATUS_IGNORE);
				calculoiterativo(buffrecep, sizeporp, valormenosuno, 0);	
			}
			else{
				MPI_Isend(&buffrecep[0], 1, MPI_INT, (rank-1), 0, MPI_COMM_WORLD, &req);
				MPI_Wait(&req, MPI_STATUS_IGNORE);
				MPI_Irecv(&valormenosuno, 1, MPI_INT, (rank-1), 0, MPI_COMM_WORLD, &req);
				MPI_Wait(&req, MPI_STATUS_IGNORE);
				MPI_Isend(&buffrecep[sizeporp-1], 1, MPI_INT, (rank+1), 0, MPI_COMM_WORLD, &req);
				MPI_Wait(&req, MPI_STATUS_IGNORE);
				MPI_Irecv(&valormasuno, 1, MPI_INT, (rank+1), 0, MPI_COMM_WORLD, &req);
				calculoiterativo(buffrecep, sizeporp, valormenosuno, valormasuno);
			}
		}

		MPI_Gather(buffrecep, sizeporp, MPI_INT, vector, sizeporp, MPI_INT, 0, MPI_COMM_WORLD);
		stoptime=MPI_Wtime();	
		totaltime+=stoptime-starttime;
		j++;
	}
	
	
	if(rank==0){
		generararchivo(vector, size);
		printf("\nTiempo transcurrido con %d células y %d generaciones: %lf\n",size,cantiter,totaltime);
	}

	free(vector);
	free(buffrecep);
	MPI_Finalize();
	return 0;
}
