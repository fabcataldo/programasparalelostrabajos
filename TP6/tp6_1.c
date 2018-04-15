#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#define SIZEPROC 4
#define SIZEMAT 8

void mostrarmatriz(int* matriz, int size){
	int i,j;
	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			printf("%d ",matriz[i*size+j]);		
		}
		printf("\n");
	}
}

void cargar_matriz(int* matriz, int size){
	int i,j;
	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			matriz[i*size+j]=(10*i)+j;
		}
	}
}


void generaroutput(FILE *ptrfile, int *matriz, int f, int c, int nproc, char *descripcionmatriz){

	int i, j;
	fprintf(ptrfile,"Proceso %d\n",nproc);
	fprintf(ptrfile,"%s\n",descripcionmatriz);
	for(i=0;i<f;i++){
		for(j=0;j<c;j++){
			fprintf(ptrfile,"%d ",matriz[(i*c)+j]);
		}
		fprintf(ptrfile, "\n");
	}

}


int main(int argc, char *argv[]){
	int p, rank;
	
	MPI_Status stat;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	if (p != SIZEPROC) {
		printf("Tiene que especificar %d procesos.\n",SIZEPROC);
		MPI_Finalize();
		return 1;
	}

	FILE *ptrsfiles[SIZEPROC];
	char nombrearchivo[9];
	sprintf(nombrearchivo,"%s.%d","output",rank);
	ptrsfiles[rank]=fopen(nombrearchivo,"w+");
	char descripcionmatriz[10];

	int source=0, dest,tag=1, i,j,k;
	//matriz inicial
	int* a = (int*)malloc(SIZEMAT*SIZEMAT*sizeof(int));
	//buffer de recepción de cada proceso
	int* b = NULL;


	if(rank==0){
		cargar_matriz(a, SIZEMAT);
		generaroutput(ptrsfiles[rank], a, SIZEMAT, SIZEMAT, rank, "Matriz inicial");
	}

	
	//mando la matriz a todos los procesos, y éstos van a llamar a MPI_Bcast para recibir
	MPI_Bcast(a, SIZEMAT*SIZEMAT, MPI_INT, 0, MPI_COMM_WORLD);  
	generaroutput(ptrsfiles[rank], a, SIZEMAT, SIZEMAT, rank, "Contigua");


	b = (int*)malloc(2*SIZEMAT*sizeof(int));
	//mando 2 filas de 8 elementos a cada proceso, y cada proceso va a recibirlos en el buffer b
	MPI_Scatter(a, (SIZEMAT*SIZEMAT)/SIZEPROC, MPI_INT, b, (SIZEMAT*SIZEMAT)/SIZEPROC, MPI_INT, 0, MPI_COMM_WORLD);	
	generaroutput(ptrsfiles[rank], b, 2, SIZEMAT, rank, "Por filas");
	free(b);


	b = (int*)malloc(SIZEMAT*2*sizeof(int));
	MPI_Datatype porcolumnas;
	//defino el nuevo tipo de datos "porcolumnas"
	MPI_Type_contiguous(SIZEMAT*2, MPI_INT, &porcolumnas);
	MPI_Type_commit(&porcolumnas);
	//mando 2 columnas de 8 elementos a cada proceso, y cada proceso va a recibirlos en el buffer b
	MPI_Scatter(a, (SIZEMAT*SIZEMAT)/SIZEPROC, MPI_INT, b, (SIZEMAT*SIZEMAT)/SIZEPROC, porcolumnas, 0, MPI_COMM_WORLD);	
	generaroutput(ptrsfiles[rank], b, SIZEMAT, 2, rank, "Por columnas");
	free(b);
	MPI_Type_free(&porcolumnas);


	//defino el nuevo tipo de datos "transpuesta"	
	MPI_Datatype transpuesta,newtranspuesta;
	//mando 8 bloques de un elemento, cuento cada SIZEMAT, y MPI_INT es el oldtype
	MPI_Type_vector(SIZEMAT, 1, SIZEMAT, MPI_INT, &transpuesta);
	//necesito modificar el tipo anterior, asignando 8 bloques, de tamaño 1, de separación int bytes entre bloques,
	//y creando un nuevo tipo: newtranspuesta
	MPI_Type_create_hvector(SIZEMAT, 1, sizeof(int), transpuesta, &newtranspuesta);
	MPI_Type_commit(&newtranspuesta);
	b = (int*)malloc(SIZEMAT*SIZEMAT*sizeof(int));
	if (rank == 0) {
		for (i=1; i<SIZEPROC; i++) {
			//envío a todos los procesos (menos al 0, la matriz de 8x8
		       MPI_Send(a, SIZEMAT*SIZEMAT, MPI_INT, i, tag, MPI_COMM_WORLD);
		}
  	}
	else{
		//printf("\nTRANSPUESTAA\n");
		//recibe la info cada proceso teniendo en cuenta el tipo newtranspuesta, ignorando el status
		MPI_Recv(b, 1, newtranspuesta, source, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		generaroutput(ptrsfiles[rank], b, SIZEMAT, SIZEMAT, rank, "Transpuesta");
	}
	free(b);
	MPI_Type_free(&transpuesta);
	MPI_Type_free(&newtranspuesta);


	//printf("\nFilas intercaladas\n");
	MPI_Datatype filasintercaladas,newfilasintercaladas;
	//mpi_type_vector(cant de bloques, tamaño de cada bloque, distancia entre bloque y bloque, etc...)	
	//voy a crear un tipo que va a tener dos bloques de 8 elementos, que va a estar separado de a entre 32 elementos (sacando los bloques
	//correspondientes de la matriz)
	//si por ej tengo cuatro filas A,B,C,D, el primer proc va a recibir A y C, luego B y D
	MPI_Type_vector(SIZEMAT/SIZEPROC, SIZEMAT, SIZEMAT*SIZEPROC, MPI_INT, &filasintercaladas);
	//el nuevo tipo va a ser "resizerado", con un limite inferior de 0 y un limite superior de 8*sizeof(int)
	//MPI_Type_create_resized(old_type, MPI_Aint nuevo_limite_inferior, MPI_Aint tamaño_del_nuevo_tipo_que_extiende_del_tipo_viejo, nuevo_tipo)
	MPI_Type_create_resized(filasintercaladas, 0, SIZEMAT*sizeof(int), &newfilasintercaladas);
	MPI_Type_commit(&newfilasintercaladas);
	b = (int*)malloc(((SIZEMAT/SIZEPROC)*SIZEMAT)*sizeof(int));

	//mando a todos los procesos la matriz a por fila, pero ellos la van a recibir en el buffer b, que tiene un tamaño = SIZEMAT*SIZEMAT/SIZEPROC
	//y que va a ser de tipo MPI_INT
	MPI_Scatter(a, 1, newfilasintercaladas, b, SIZEMAT*SIZEMAT/SIZEPROC, MPI_INT, 0, MPI_COMM_WORLD);
	generaroutput(ptrsfiles[rank], b, 2, SIZEMAT, rank, "Filas intercaladas");
	free(b);
	MPI_Type_free(&filasintercaladas);
	MPI_Type_free(&newfilasintercaladas);

	//POR IMPLEMENTARRRRR
	/*
	MPI_Datatype columnasintercaladas,ciresized;
	//mpi_type_vector(cant de bloques, tamaño de cada bloque, distancia entre bloque y bloque, etc...)	
	//voy a crear un tipo que va a tener 8 bloques de 2 elementos, que va a estar separado de a 1 elemento (sacando los bloques
	//correspondientes de la matriz)
	//si por ej tengo cuatro columnas A,B,C,D, el primer proc va a recibir A y C, luego B y D
	MPI_Type_vector(SIZEMAT, 2, 1, MPI_INT, &columnasintercaladas);
	//el nuevo tipo va a ser "resizerado", con un limite inferior de 0 y un limite superior de 8*sizeof(int)
	MPI_Type_create_resized(columnasintercaladas, 0, SIZEMAT*sizeof(int), &ciresized);
	MPI_Type_commit(&ciresized);
	b = (int*)malloc((2*SIZEMAT)*sizeof(int));

	//mando a todos los procesos la matriz a por fila, pero ellos la van a recibir en el buffer b, que tiene un tamaño = SIZEMAT*SIZEMAT/SIZEPROC
	//y que va a ser de tipo MPI_INT
	MPI_Scatter(a, 1, ciresized, b, SIZEMAT*SIZEMAT/SIZEPROC, MPI_INT, 0, MPI_COMM_WORLD);
	generaroutput(ptrsfiles[rank], b, SIZEMAT, 2, rank, "Columnas intercaladas");	
	free(b);
	MPI_Type_free(&columnasintercaladas);
	*/

	/*
	MPI_Datatype submatriz;
	MPI_Type_vector(SIZEMAT/2, 1, SIZEMAT/2, MPI_INT, &submatriz);
	MPI_Type_commit(&submatriz);
	b = (int*)malloc((SIZEMAT/2)*(SIZEMAT/2)*sizeof(int));

	if (rank == 0) {
		for (i=1; i<=SIZEPROC; i++) {
		       MPI_Send(a, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
		}
  	}
	else{
		printf("\nSubmatriz\n");
		MPI_Recv(b, (SIZEMAT/2)*(SIZEMAT/2), submatriz, source, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for(i=0;i<8;i++){
			for(j=0;j<8;j++){
				printf("b= %d ",b[(i*8)+j]);
			}
			printf("\n");
		}
		free(b);
	}
	MPI_Type_free(&submatriz);
	*/	
	free(a);
	fclose(ptrsfiles[rank]);
	MPI_Finalize();
	return 0;
}