#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

#define ANCHOMPRC (size/p_cols) //ancho (cant de columnas) de las mallas de los procesos
#define ALTOMPRC (size/p_rows) //alto (cant de filas) de las mallas de los procesos

int p_cols=1;
int times = 0; 
char* file="input.dat";
int size=0;
int filas=0;
int columnas=0;
int np;

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
			if(fgetc(fp)!= ' '){
				j++;
			}
		} while((c = fgetc(fp)) != '\n');
	}
	columnas=j;
	size=filas;
	fclose(fp);
}


void controlar_argumentos(int argc, char** argv){
	if(argc < 2){
		printf("Debe ingresar al menos 1 argumento de la siguiente forma:\n");
		printf("nombre_ejecutable_programa [opciones] <t>\n");
		printf("* -t <times>: Cantidad máxima de iteraciones (Por defecto: 10000. Si escribe '-t 0', se va a tomar el valor por defecto). \n");
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
	
	if(times==0){
		times=10000;	
	}

	if(np!=4){
		printf("Se deben especificar 4 procesos para paralelizar el programa");	
		exit(1);
	}	

	if(filas!=columnas){
		printf("La grilla debe ser cuadrada, es decir, debe tener la misma cantidad de filas que de columnas. Reescriba la grilla.");	
		exit(1);
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


void read_file(char* file, int* m){
    FILE *fp;
    char ch;
    fp = fopen(file,"r");
    if ( fp == NULL )
    {
        puts ( "No se puede abrir el archivo");
    }

    char c;

    int i, j;
	for(i = 0; i < filas; i++) {
		j = 0;
		do {
        fscanf(fp, "%d", &(m[i*size+j]));
        j++;
			} while((c = fgetc(fp)) != '\n');
	}
}


void generar_archivo(int* matriz, int size) {
	FILE *ptrfile=fopen("output.dat","w+");
	int i,j;
	for(i=0;i<filas;i++){
		for(j=0;j<columnas;j++){
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


int ver_vecinas(int* m, int i, int j, int size, int proc, int p_rows){
	int f, c, vecinas_vivas=0;
	
	//lo que haya en la fila de arriba en donde estoy, si existe, lo miro, si no, considero que hay ceros en esa fila
	if(i>0){
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
	}

	//miro la columna izquierda de la célula en la que estoy parado
	if(m[i*size+(j-1)] == 1){
		vecinas_vivas++;			
	}
			
	//lo que haya en la fila de abajo, si existe, lo miro, si no, considero que hay ceros en esa fila
	if(i!=ALTOMPRC+1){
		//SIGO MIRANDO la columna izquierda de la célula
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
	}
	
	//miro el valor restante de la columna derecha de la célula
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

void ver_submatriz(int *m, int f, int c){
	int i,j;
	for(i=0;i<f;i++){
		for(j=0;j<c;j++){
			printf("%d ",m[i*c+j]);
		}
		printf("\n");
	}	
}

void calculo_iterativo(int*matriz, int* matriz_nva, int p_rows, int proc){
	int i, j, inf=0, cel_vivas=0;
	if (proc==0){
		inf=1;
	}
	for(i=inf;i<ALTOMPRC+2;i++){
		for(j=1;j<ANCHOMPRC+2;j++){
			cel_vivas=ver_vecinas(matriz, i, j, ANCHOMPRC+2, proc, p_rows);
			if(matriz[(i*(ANCHOMPRC+2))+j] == 1 ){
				if(cel_vivas<2 || cel_vivas>3){  
					//muere o porque hay una sóla o ninguna célula de las vecinas vivas (muerte por soledad)
					//o porque hay más de tres celulas vecinas vivas (muere por sobrepoblación)
					matriz_nva[(i*(ANCHOMPRC+2))+j]=0;
				}
				else{
					matriz_nva[(i*(ANCHOMPRC+2))+j]=matriz[(i*(ANCHOMPRC+2))+j];				
				}
			}
			else{
				if(cel_vivas==3){
					matriz_nva[(i*(ANCHOMPRC+2))+j]=1;			
				}
				else{
					matriz_nva[(i*(ANCHOMPRC+2))+j]=matriz[(i*(ANCHOMPRC+2))+j];
				}
			}	
		}	
	}
}


int main(int argc, char **argv){
	int  rank, tag=0;
	MPI_Request req;
	MPI_Status sts;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	set_dim(file);
	controlar_argumentos(argc, argv);

	int* matriz=NULL;
	if(rank==0){
		matriz=(int*) calloc(size*size,sizeof(int));
		read_file(file, matriz);

	}


	int p_rows=np/p_cols;
	int* matriz_nva = (int*)calloc((ALTOMPRC+2)*(ANCHOMPRC+2),sizeof(int));
	int* matriz_vja = (int*)calloc((ALTOMPRC+2)*(ANCHOMPRC+2),sizeof(int));
	int* aux=NULL;
	int  iter,i,j,src;

	MPI_Scatter(matriz, (size*size)/np, MPI_INT, matriz_vja+( (1*(ANCHOMPRC+2)) +1), (ALTOMPRC+2)*(ANCHOMPRC+2), MPI_INT, 0, MPI_COMM_WORLD);	
	
	//variables que sirven para, de acuerdo al proceso, y de acuerdo a la malla, qué cosas mandar y a donde
	int n=rank-p_cols; 
	int s=rank+p_cols;
	if(n<0){
		n=-1;	
	}
	if(s>np-1){
		s=-1;
	}
	
	
	for(iter=0;iter<times;iter++){
		if(n!=-1){
			//mando la primera fila sin ceros al proceso que sigue
			MPI_Send(matriz_vja+ANCHOMPRC+2, (ANCHOMPRC+2), MPI_INT, n, tag, MPI_COMM_WORLD); 

			//recibo del proc anterior a mi su última fila sin ceros, en mi primera fila que tiene ceros
			MPI_Recv(matriz_vja, ANCHOMPRC+2, MPI_INT, n, tag, MPI_COMM_WORLD, &sts);	
		}
		if(s!=-1){
			//mando la última fila sin ceros al proceso que corresponda
			MPI_Send(matriz_vja+(ALTOMPRC*(ANCHOMPRC+2)), ANCHOMPRC+2, MPI_INT, s, tag, MPI_COMM_WORLD);

			//recbio en mi última fila con ceros lo que me venga del proc que me sigue, o sea, la primera fila sin ceros del proceso que sigue	
			MPI_Recv(matriz_vja+((ALTOMPRC+1)*(ANCHOMPRC+2)), ANCHOMPRC+2, MPI_INT, s, tag, MPI_COMM_WORLD, &sts);
		}


		calculo_iterativo(matriz_vja, matriz_nva, p_rows, rank);

		aux=matriz_nva;
		matriz_nva=matriz_vja;
		matriz_vja=aux;	
				

	}
	
	
	if(rank==0){
		//Primero paso el resultado de lo que hizo el proceso 0 a la matriz principal
		for(i=0;i<ALTOMPRC;i++){
			for(j=0;j<ANCHOMPRC;j++){
				matriz[i*size+j]=aux[(i+1) * (ANCHOMPRC+2) + (j+1)];
			}
		}				
		
		//Ahora recibo el resto de las submatrices y lo paso a la matriz principal
		for(src=1;src<np;src++){			

				MPI_Recv(matriz+(ANCHOMPRC*src), ALTOMPRC*ANCHOMPRC, MPI_INT, src, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);		
		}
		
		generar_archivo(matriz, size);
	}	
	else{
		//NO soy el proceso 0, si no otro proceso
	
	
		//Creo un nuevo tipo, que va a posibilitar enviar mi submatriz al proc 0 para que arme la matriz principal
		MPI_Datatype submtp, nsubmtp;
		MPI_Type_vector(ALTOMPRC, ANCHOMPRC, 1, MPI_INT, &submtp);
		MPI_Type_create_resized(submtp, 0, (ALTOMPRC*ANCHOMPRC)*sizeof(int),&nsubmtp);
		MPI_Type_commit(&nsubmtp);

		//le envío mi submatriz ya calculada al proc 0 para que lo guarde en la matriz principal
		MPI_Send(aux+ANCHOMPRC+3, 1, nsubmtp, 0, tag, MPI_COMM_WORLD);
		MPI_Type_free(&nsubmtp);
	}	

	free(matriz_nva);
	free(matriz_vja);	
	free(matriz);
	MPI_Finalize();
	return 0;
}
