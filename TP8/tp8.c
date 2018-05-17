#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include "mpi.h"

#define FILMP (size/filas_grilla)
#define COLMP (size/columnas_grilla)
int size=4;
int filas_grilla=2;
int columnas_grilla=2;
char* nomb_file="matrix_input";
int np;

void controlarargumentos(int argc, char** argv){
	if(argc < 4){
		fprintf(stderr, "Debe ingresar al menos 3 argumentos de la siguiente forma:\n");
		fprintf(stderr, "mpirun -np <cantidad_de_procesos> nombre_ejecutable_programa <filas_grilla> <columnas_grilla> <size>\n");
		fprintf(stderr, "* <filas_grilla> e <columnas_grilla> se refieren al tamaño de las submatrices que surgen de la matriz (default: filas_grilla=2, columnas_grilla=2).\n");
		fprintf(stderr, "* <size> se refiere al tamaño de la matriz  (default: 4).\n");
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
		fprintf(stderr, "El tamaño de las matrices tiene que ser mayor o igual a 2.\n");	
		exit(1);
	}
	if(filas_grilla>size || columnas_grilla>size){
		fprintf(stderr, "Alguna de las dimensiones de las submatrices es mayor al tamaño de la matriz.\n");	
		exit(1);		
	}

	if(filas_grilla!=columnas_grilla){
		fprintf(stderr, "Este programa multiplica matrices cuadradas. Las dimensiones de las submatrices son distintas. Luego, las dimensiones especificadas son inválidas.\n");	
		exit(1);
	}
	if(np/2!=filas_grilla && np/2!=columnas_grilla){
			fprintf(stderr, "Se recomienda, a fines de conseguir resultados correctos, escribir valores que sean la mitad de la cantidad de procesos especificada.\n");	
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
	if (fp == NULL){
		fprintf(stderr, "El archivo 'matrix_input' no se encuentra en el directorio actual. \n");	
		exit(1);
	}

	for(i=0;(i<size);i++){
		for(j=0;j<size;j++){
			if(fgetc(fp)!=0){
				fscanf(fp, "%d", &(m[i*size+j]));
			}
		}	
	}
	
	for(i=0;((i<size) || (fgetc(fp) != EOF));i++){
		for(j=0;j<size;j++){
			if(fgetc(fp)!=0){
				fscanf(fp, "%d", &(m2[i*size+j]));
			}
		}	
	}

	fclose(fp);
}

void generar_archivo(int* matriz, int size) {
	FILE *ptrfile=fopen("matrix_input.output","w+");
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
	

	int* matriz_1 = NULL;
	int* matriz_2 = NULL;
	int* matriz_res = NULL;
	int* subm_1= (int*)calloc(FILMP*COLMP,sizeof(int));
	int* subm_2= (int*)calloc(FILMP*COLMP,sizeof(int));
	int* subm_res = (int*)calloc(FILMP*COLMP,sizeof(int));

	int i,j,src;

	if(rank==0){
		matriz_1 = (int*)calloc(size*size,sizeof(int));
		matriz_2 = (int*)calloc(size*size,sizeof(int));
		matriz_res = (int*)calloc(size*size,sizeof(int));

		read_file(nomb_file, matriz_1, matriz_2, size);
	}

	double 	start_t=MPI_Wtime();
	int offst[np];	
	offst[0]=0;
	offst[1]=COLMP;
	offst[2]=size*(size/COLMP);
	offst[3]=size*(size/COLMP)+COLMP;
	if(np>4){
		for(i=4;i<np;i++){
			offst[i]=size*(size/COLMP)+(COLMP*i);
		}
	}
	MPI_Datatype submp, nsubmp;
	//el proceso 0 envía a todos (incluso se envía a él mismo) la submatriz a calcular, junto con la submatriz resultado
	MPI_Type_vector(FILMP, COLMP, size, MPI_INT, &submp);
	MPI_Type_create_resized(submp, 0, COLMP*sizeof(int),&nsubmp);
	MPI_Type_commit(&nsubmp);

	
	if(rank==0){
		for(i=0;i<np;i++){
			MPI_Send(matriz_1+offst[i], 1, nsubmp, i, tag, MPI_COMM_WORLD);
		}
	}

	//Ahora, todos los procesos reciben las submatrices que le corresponden, en la matriz_1
	MPI_Recv(subm_1, FILMP*COLMP, MPI_INT, 0, tag, MPI_COMM_WORLD, &sts);


	if(rank==0){
		for(i=0;i<np;i++){
			MPI_Send(matriz_2+offst[i], 1, nsubmp, i, tag, MPI_COMM_WORLD);
		}
	}

	//Ahora, todos los procesos reciben las submatrices que le corresponden, en la matriz_2
	MPI_Recv(subm_2, FILMP*COLMP, MPI_INT, 0, tag, MPI_COMM_WORLD, &sts);

	MPI_Type_free(&nsubmp);

	//luego, todos se ponen a calcular, llamando a matmul(), y almacenando el resultado en subm_res
	matmul(subm_1, subm_2, subm_res, FILMP, COLMP);


	free(subm_1);
	free(subm_2);

	MPI_Send(subm_res, FILMP*COLMP, MPI_INT, 0, tag, MPI_COMM_WORLD);			


	//RECIBO DATOS:
	if(rank==0){		
		free(matriz_1);
		free(matriz_2);

		for(i=0;i<FILMP;i++){
			for(j=0;j<COLMP;j++){
				matriz_res[i*size+j]=subm_res[i*COLMP+j];
			}
		}

		MPI_Datatype res, nres;
		MPI_Type_vector(FILMP, COLMP, 1, MPI_INT, &res);
		MPI_Type_create_resized(submp, 0, COLMP*sizeof(int),&nres);
		MPI_Type_commit(&nres);

		for(src=1;src<np;src++){
			MPI_Recv(matriz_res+offst[src], 1, nres, src, tag, MPI_COMM_WORLD, &sts);
		}	
		MPI_Type_free(&nres);
		free(subm_res);				
		printf("Tiempo transcurrido: %f\n", MPI_Wtime()-start_t);
		generar_archivo(matriz_res, size);	
		
		free(matriz_res);		

	}

	MPI_Finalize();
	return 0;
}
