#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <omp.h>

int times = 0; 
char* file="input.dat";
int size=0;
int filas=0;
int columnas=0;

void set_dim(char* file){
	FILE *fp = fopen(file, "r");
	int i,j=0;
	if (fp == NULL){
		fprintf(stderr, "El archivo 'input.dat' no se encuentra en el directorio actual. \n");	
		exit(1);
	}

	char c;
	while((c = fgetc(fp)) != EOF){
		if(c == '\n'){
		    filas++;
		}
	}

	fclose(fp);

	fp = fopen(file,"r");

	for(i=0;i<filas;i++){
		j=0;
		do{
			if(fgetc(fp)!= ' ')
				j++;
		} while((c = fgetc(fp)) != '\n');
	}
	columnas=j;
	size=filas;
	fclose(fp);
}


void controlar_argumentos(int argc, char** argv){
	if(argc < 2){
		printf("Debe ingresar al menos 1 argumento de la siguiente forma:\n");
		printf("nombre_ejecutable_programa [opciones] <t> <f>\n");
		printf("* -t <times>: Cantidad máxima de iteraciones (default=10000, se define un corte si la grilla llega a un estado estacionario. Si escribe -t 0, el programa va a setear las iteraciones con el valor por defecto).\n");
		exit(1);
	 }      
	 int c;
	 while ((c = getopt (argc, argv, "t:")) != -1) {
		switch (c) {
			case 't':
				times = atoi(optarg);
				break;
		   	default:
		   		break;
		}
	 }
	if(filas!=columnas){
		printf("El programa sólo admite grillas cuadradas. Escriba un archivo que tenga tal grilla (cantidad de filas=cantidad de columnas).\n");
		exit(1);	
	}
	if(times==0){
		times=10000;	
	}

	if(size!=4){
		printf("El tamaño de la grilla es fijo, y tiene que ser de 4x4.");	
		exit(1);
	}
}


void ver_matriz(int *m, int size){
	int i,j;
	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			printf("%d ",m[i*size+j]);
		}
		printf("\n");
	}	
}


void read_file(char* file, int* m, int size){
	FILE *fp = fopen(file, "r");
	int i,j;
	if (fp == NULL){
		fprintf(stderr, "El archivo 'input.dat' no se encuentra en el directorio actual. \n");	
		exit(1);
	}

	char c;
	for(i=1;i<size-1;i++){
		for(j=1;j<size-1;j++){
				fscanf(fp, "%d", &(m[i*size+j]));
		}	
	}

	fclose(fp);
}

void generar_archivo(int* matriz, int size) {
	FILE *ptrfile=fopen("output.dat","w+");
	int i,j;
	for(i=1;i<size-1;i++){
		for(j=1;j<size-1;j++){
			if(matriz[i*size+j]==1){
				fprintf(ptrfile,"* ");	
			}
			else{
				fprintf(ptrfile,"-  ");				
			}
		}
		fprintf(ptrfile, "\n");
	}
	fclose(ptrfile);
}

int ver_vecinas(int* m, int i, int j, int size){
	int f, c, vecinas_vivas=0;		

	//miro las vecinas de arriba de la célula
	if(m[(i-1)*size+(j-1)] == 1){
		vecinas_vivas++;			
	}
	if(m[(i-1)*size+j] == 1){
		vecinas_vivas++;			
	}	
	if(m[(i-1)*size+(j+1)] == 1){
		vecinas_vivas++;			
	}	
	
	//miro la columna izquierda de la célula
	if(m[i*size+(j-1)] == 1){
		vecinas_vivas++;			
	}
	if(m[(i+1)*size+(j-1)] == 1){
		vecinas_vivas++;			
	}

	//miro la fila de abajo de la célula
	if(m[((i+1)*size)+j] == 1){
		vecinas_vivas++;			
	}
	if(m[((i+1)*size)+(j+1)] == 1){
		vecinas_vivas++;			
	}

	
	//miro la columna derecha de la célula
	if(m[(i*size)+(j+1)]== 1){
		vecinas_vivas++;			
	}

	return vecinas_vivas;
}

//función que compara dos matrices, si son distintas devuelve 1, si no, 0
int comparar_mat(int* m, int* m_2, int size){
	int i, j;
	for(i=1;i<size-1;i++){
		for(j=1;j<size-1;j++){
			if(m[i*size+j]!=m_2[i*size+j]){
				return 1;
			}
		}
	}
	return 0;
}

void calculo_iterativo(int*matriz, int* matriz_nva, int size){
	int i, j, iter, cel_vivas=0;
	int cortar=0;

	int* aux=NULL;
		
	omp_set_num_threads(4);
	//matriz, matriz_nva y aux las comparto para los hilos
	#pragma omp parallel shared (matriz, matriz_nva, aux, times)
	{		

		//con esta directiva hago explícita la sincronización, asegurando que cada hilo ejecute este for de forma exclusiva
		//cada hilo ejecuta esta sección crítica y se asegura que ningún otro hilo modifique las variables que se compartieron
		#pragma omp critical
		for(iter=0;iter<times;iter++){
			for(i=1;i<size-1;i++){
				for(j=1;j<size-1;j++){
					cel_vivas=ver_vecinas(matriz, i, j, size);
					if(matriz[i*size+j] == 1 ){
						if(cel_vivas<2 || cel_vivas>3){  
							//muere o porque hay una sóla o ninguna célula de las vecinas vivas (muerte por soledad)
							//o porque hay más de tres celulas vecinas vivas (muere por sobrepoblación)
							matriz_nva[i*size+j]=0;
						}
						else{
							matriz_nva[i*size+j]=matriz[i*size+j];				
						}
					}
					else{
						if(cel_vivas==3){
							matriz_nva[i*size+j]=1;			
						}
						else{
							matriz_nva[i*size+j]=matriz[i*size+j];
						}
					}	
				}	
			}
			aux=matriz_nva;
			matriz_nva=matriz;
			matriz=aux;
			cel_vivas=0;

		}
	}
	
}


int main(int argc, char **argv){
	set_dim(file);
	controlar_argumentos(argc, argv);
	
	int re_size=size+2;  //cuando haga la matriz me va a quedar con dos filas y dos columnas agregadas
	int* matriz_nva = (int*)calloc(re_size*re_size,sizeof(int));
	int* matriz = (int*) calloc(re_size*re_size,sizeof(int));

	read_file(file, matriz, re_size);

	calculo_iterativo(matriz, matriz_nva, re_size);

	generar_archivo(matriz, re_size);
	free(matriz_nva);
	free(matriz);
}
