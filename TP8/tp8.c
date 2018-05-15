#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include "mpi.h"

#define FILMP (size/filas_grilla)
#define COLMP (size/columnas_grilla)
int size=4;
int filas_grilla=2;
int columnas_grilla=2;
char* nomb_file="matrix.input";
int np;

void controlarargumentos(int argc, char** argv){
	if(argc < 4){
		printf("Debe ingresar al menos 3 argumentos de la siguiente forma:\n");
		printf("mpirun -np <cantidad_de_procesos> nombre_ejecutable_programa <filas_grilla> <columnas_grilla> <size>\n");
		printf("* <filas_grilla> e <columnas_grilla> se refieren al tamaño de las submatrices que surgen de la matriz (default: filas_grilla=2, columnas_grilla=2).\n");
		printf("* <size> se refiere al tamaño de la matriz  (default: 4).\n");
		exit(1);
	}
	if(np>4){
		printf("Debe especificar una cantidad de procesos menor o igual a 4.");		
		exit(1);
	}
	
	if(filas_grilla!=0){
		filas_grilla=atoi(argv[1]);
	}
	if(columnas_grilla!=0){
		columnas_grilla=atoi(argv[2]);
	}
	if(size!=0){
		size=atoi(argv[3]);
	}

	if(size<2){
		printf("El tamaño de las matrices tiene que ser mayor o igual a 2.");	
		exit(1);
	}
	if(filas_grilla>size || columnas_grilla>size){
		printf("Algunas de las dimensiones de las submatrices es mayor al tamaño de la matriz.");	
		exit(1);		
	}
}

void vermatriz(int *m, int size){
	int i,j;
	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			printf("%d ",m[i*size+j]);
		}
		printf("\n");
	}	
}

void versubmatriz(int *m, int f, int c){
	int i,j;
	for(i=0;i<f;i++){
		for(j=0;j<c;j++){
			printf("%d ",m[i*c+j]);
		}
		printf("\n");
	}	
}

void read_file(char* nomb_file, int* m, int* m2, int size){
	FILE *fp = fopen(nomb_file, "r");
	int i,j;
	if (fp == NULL)
		return;

	for(i=0;(i<size);i++){
		for(j=0;j<size;j++){
			fscanf(fp, "%d", &(m[i*size+j]));
		}	
	}
	
	for(i=0;((i<size) || (fgetc(fp) != EOF));i++){
		for(j=0;j<size;j++){
			fscanf(fp, "%d", &(m2[i*size+j]));
		}	
	}

	fclose(fp);
}

void generar_input(char* nomb_file, int size){
	FILE *ptrfile2=fopen(nomb_file,"w+");
	int i,j;
	int num;
	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			num=1+rand()%(101-1);
			fprintf(ptrfile2,"%d ", num);				
		}
		fprintf(ptrfile2, "\n");
	}
	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			num=1+rand()%(101-1);
			fprintf(ptrfile2,"%d ", num);				
		}
		fprintf(ptrfile2, "\n");
	}
	fclose(ptrfile2);	
}

void generar_archivo(int* matriz, int size) {
	FILE *ptrfile=fopen("matrix.output","w+");
	int i,j;
	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			fprintf(ptrfile,"%d ", matriz[i*size+j]);				
		}
		fprintf(ptrfile, "\n");
	}
	fclose(ptrfile);
}

void matmul(int* a, int* b, int* c, int fil, int col){
	int i,j,k;
	for(i=0;i<fil;i++)
		for(j=0;j<col;j++)
			c[i*col+j]=0;							

	for(i=0;i<fil;i++){
		for(j=0;j<col;j++){
			for(k=0;k<fil;k++){
				c[i*col+j]+=a[i*col+k]*b[k*col+j];
			}
		}	
	}
}


int main(int argc, char **argv){
	int rank, tag=0;
	MPI_Request req;
	MPI_Status sts;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &np);
	
	
	if(rank==0){
		controlarargumentos(argc, argv);
	}
	

	int* matriz_1 = (int*)calloc(size*size,sizeof(int));
	int* matriz_2 = (int*)calloc(size*size,sizeof(int));
	int* matriz_res = (int*)calloc(size*size,sizeof(int));
	int* subm_1= (int*)calloc(FILMP*COLMP,sizeof(int));
	int* subm_2= (int*)calloc(FILMP*COLMP,sizeof(int));
	int* subm_res = (int*)calloc(FILMP*COLMP,sizeof(int));

	double start_t=MPI_Wtime();
	int i,j,src;

	if(rank==0){
		generar_input(nomb_file, size);
		read_file(nomb_file, matriz_1, matriz_2, size);
		//vermatriz(matriz_1, size);	
		//printf("\n");
		//vermatriz(matriz_2, size);	
	}

	//el proceso 0 envía a todos (incluso se envía a él mismo) la submatriz a calcular, junto con la submatriz resultado
	MPI_Datatype submp, nsubmp;
	MPI_Type_vector(FILMP, COLMP, size, MPI_INT, &submp);
	MPI_Type_create_resized(submp, 0, COLMP*sizeof(int),&nsubmp);
	MPI_Type_commit(&nsubmp);
	//printf("Enviando matrices\n");
	int offst[4]={0, COLMP, size*(size/COLMP), size*(size/COLMP)+COLMP};
	if(rank==0){
		for(i=0;i<np;i++){
			MPI_Send(matriz_1+offst[i], 1, nsubmp, i, tag, MPI_COMM_WORLD);
		}
	}

	//Ahora, todos los procesos reciben las submatrices que le corresponden, en la matriz_1
	MPI_Recv(subm_1, FILMP*COLMP, MPI_INT, 0, tag, MPI_COMM_WORLD, &sts);
	//printf("\nsubmatriz_1 proceso %d\n",rank);
	//versubmatriz(subm_1, FILMP, COLMP);

	if(rank==0){
		for(i=0;i<np;i++){
			MPI_Send(matriz_2+offst[i], 1, nsubmp, i, tag, MPI_COMM_WORLD);
		}
	}

	//Ahora, todos los procesos reciben las submatrices que le corresponden, en la matriz_2
	MPI_Recv(subm_2, FILMP*COLMP, MPI_INT, 0, tag, MPI_COMM_WORLD, &sts);


	//luego, todos se ponen a calcular, llamando a matmul(), y almacenando el resultado en subm_res
	matmul(subm_1, subm_2, subm_res, FILMP, COLMP);

	
	//int flag=0;	
	//MPI_Test(&req, &flag, &sts);
	//printf("FLAG : %d",flag);

	
	MPI_Send(subm_res, FILMP*COLMP, MPI_INT, 0, tag, MPI_COMM_WORLD);			


	//RECIBO DATOS:
	if(rank==0){
		for(i=0;i<FILMP;i++){
			for(j=0;j<COLMP;j++){
				matriz_res[i*size+j]=subm_res[i*COLMP+j];
			}
		}		
		
		for(src=1;src<np;src++){
			MPI_Recv(matriz_res+offst[i], 1, nsubmp, src, tag, MPI_COMM_WORLD, &sts);
		}	

		printf("Tiempo transcurrido: %f\n", MPI_Wtime()-start_t);
		generar_archivo(matriz_res, size);			
	}

	MPI_Type_free(&nsubmp);

	free(matriz_1);
	free(matriz_2);
	free(matriz_res);
	free(subm_1);
	free(subm_2);
	free(subm_res);

	MPI_Finalize();
	return 0;
}
