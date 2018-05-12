#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include "mpi.h"

#define ANCHOMPRC (size/p_cols)
#define ALTOMPRC (size/p_rows)

double temp_fuente = 60;
int coord_fuente_x = 1;
int coord_fuente_y = 1;
int iteraciones = 100000;
int size = 100;
double delta = 0; //si delta queda como 0, la matriz principal debe seguir evolucionando hasta llegar a las 100000 iterac, si no, se corta hasta que
//la matriz se "estabilice" (cuando el max sea menor o igual al error por iteracion)
int p_cols=1;


//argumentos con valores por defecto, como variables globales
/*
double tempfuente = 60;
int coordtempfuentex = 1;
int coordtempfuentey = 1;
int iteraciones = 10000;
int size = 100;
double delta = 0.0;
char* nombrearchivo="output.txt";
int p_cols=1;
*/

/*
void controlarsetearargumentosprog(int argc, char** argv){
	if(argc < 4){
		printf("Debe ingresar al menos 3 argumentos de la siguiente forma:\n");
		printf("nombre_ejecutable_programa [opciones] <x> <y> <temp>\n");
		printf("* <x> e <y> indican la posición de la fuente de calor. (default: 1 en x, 1 en y)\n");
		printf("* <temp> indica la temperatura de la fuente de calor en grados centigrados. (default: 60)\n");
		printf("donde [opciones] es una o más de:\n");
		printf("* -d <delta>: Diferencia entre iteraciones tras la cual se interrumpe el cálculo (default=0.0)\n");
		printf("* -t <times>: Cantidad máxima de iteraciones (default=10000)\n");
		printf("* -s <size> : Tamaño de la malla (default=100)\n");
		printf("* -f <name> : Nombre del archivo de salida (default=’output.txt’)\n");
		exit(1);
	 }      
	 int c;
	 while ((c = getopt (argc, argv, "d:t:s:f:")) != -1) {
		switch (c) {
			case 'd':
				delta = atof(optarg);
				break;
			case 't':
				iteraciones = atoi(optarg);
				break;
			case 's':
				size = atoi(optarg);
				break;
			case 'f':
				nombrearchivo = optarg;
				break;
		   	default:
		   		break;
		}
	    }
	    coordtempfuentex = atoi(argv[optind]);
	    coordtempfuentey = atoi(argv[optind+1]);
	    tempfuente = atof(argv[optind+2]);  
	if(coordtempfuentex<0 || coordtempfuentex>=size || coordtempfuentey<0 || coordtempfuentey>=size ){
		printf("Valores de posición inválidos.\n");
		printf("<x> e <y> indican la posición de la fuente de calor.\n");
		exit(1);
	}

	coordtempfuentex++;
	coordtempfuentey++;
	size+=2;
	if(tempfuente < 0.){
		printf("La temperatura de la fuente de calor debe ser mayor a 0.\n");
		exit(1);
 	}
}
*/

void vermatriz(double *m, int size){
	int i,j;
	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			printf("%lf ",m[i*size+j]);
		}
		printf("\n");
	}	
}

void versubmatriz(double *m, int f, int c){
	int i,j;
	for(i=0;i<f;i++){
		for(j=0;j<c;j++){
			printf("%lf ",m[i*c+j]);
		}
		printf("\n");
	}	
}

void generararchivo(double* matriz, int size) {
	FILE *ptrfile=fopen("outputmpifinale.dat","w+");
	int i,j;
	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			fprintf(ptrfile,"%lf ", matriz[i*size+j]);				
		}
		fprintf(ptrfile, "\n");
	}
	fclose(ptrfile);
}

void calculoiterativo(double *mnueva, double* mvieja, int p_rows, double* max){
	int i, j;

	//devuelvo el error, para una iteración particular, para un proceso en particular
	double difer;
	for(i=1;i<ALTOMPRC+1;i++){
		for(j=1;j<ANCHOMPRC+1;j++){
				mnueva[i*ANCHOMPRC+j]=(mvieja[( (i-1)*(ANCHOMPRC+2) )+j] + mvieja[(i*(ANCHOMPRC+2))+(j+1)] + mvieja[((i+1)*(ANCHOMPRC+2))+j]+mvieja[(i*(ANCHOMPRC+2))+(j-1)]) /4;			

				//if((mnueva[i*size+j] != mvieja[i*size+j]) && !(i==coord_fuente_x && i==coord_fuente_y)){
				if((delta!=0) && !(i==coord_fuente_x && i==coord_fuente_y)){
					difer=mnueva[(i*(ANCHOMPRC+2))+j]-mvieja[(i*(ANCHOMPRC+2))+j];
					if(difer>*max){
						*max=difer;				
					}
				}
				else{
					difer=mvieja[(i*(ANCHOMPRC+2))+j]-mnueva[(i*(ANCHOMPRC+2))+j];
					if(difer>*max){
						*max=difer;				
					}				
				}
		}	

	}

	
	//return max;
}


int main(int argc, char **argv){
	int np, rank, tag=0;
	MPI_Request req;
	MPI_Status sts;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &np);
	
	/*
	if(rank==0){
		controlarsetearargumentosprog(argc, argv);
	}
	*/
	if(np!=4){
		if(rank==0){
			printf("Se debe especificar 4 procesos");		
		}
		exit(1);
	}

	int p_rows=np/p_cols;
	//matriz original
	double* matriz = (double*) calloc((size)*(size),sizeof(double));
	//submatriz para cada proceso, pero "viejo"
	double* matriz_vieja = (double*)calloc((ALTOMPRC+2)*(ANCHOMPRC+2),sizeof(double));
	//submatriz para cada proceso, pero ya calculado, a partir de "matriz_vieja"
	double* matriz_actual = (double*)calloc((ALTOMPRC+2)*(ANCHOMPRC+2),sizeof(double));

	//calculo a donde debería estar la fuente
	int rank_fuente = ((coord_fuente_x/ALTOMPRC)*p_cols)+(coord_fuente_y/ANCHOMPRC);

	int i,j,src, iter;
	double error_rank=0; //sería el error de cada rank	
	double *perr_r;
	double error_total=0; //sumo todos los error_rank con MPI_Reduce(), y guardo el resultado acá;
	
	//compruebo a cual proceso corresponde la fuente, y la guardo
	if(rank==rank_fuente){
		matriz_vieja[((coord_fuente_x%ALTOMPRC)+1 * (ANCHOMPRC+2)) + (coord_fuente_y%ANCHOMPRC+1)]=temp_fuente;	
	}

	double start_t=MPI_Wtime();
	//variables que sirven para, de acuerdo al proceso, y de acuerdo a la malla, qué cosas mandar y a donde
	int n=rank-p_cols; 
	int s=rank+p_cols; 
	int e=(rank+1)%p_cols;
	int w=rank%p_cols; 
	if(n<0){
		n=-1;	
	}
	if(s>np-1){
		s=-1;
	}
	if(e==0){
		e=-1;	
	}
	if(w==0){
		w=-1;	
	}

	//Preparo el tipo de dato para enviar las columnas al proceso que le toque
	MPI_Datatype submatcol, nsubmatcol;
	//ALTOMATPROC+2 bloques de 1 elemento cada uno, y cuento de a ANCHOMATPROC+2
	MPI_Type_vector(ALTOMPRC+2, 1, ANCHOMPRC+2, MPI_DOUBLE, &submatcol);
	//creo un nuevo tipo reacomodado que arranca de 0 hasta ANCHOMATPROC+2, cada elemento de tamaño de un double
	MPI_Type_create_resized(submatcol, 0, (ANCHOMPRC+2)*sizeof(double),&nsubmatcol);
	MPI_Type_commit(&nsubmatcol);

	int flag=0;
	double* aux;

	for(iter=0;iter<iteraciones;iter++){
		error_rank=0;
		perr_r=&error_rank;

		if(n!=-1){
			//mando la primera fila sin ceros al proceso que sigue
			//MPI_Isend(matriz_vieja+ANCHOMPRC+2, (ANCHOMPRC+2), MPI_DOUBLE, n, tag, MPI_COMM_WORLD,&req); 
			MPI_Send(matriz_vieja+ANCHOMPRC+2, (ANCHOMPRC+2), MPI_DOUBLE, n, tag, MPI_COMM_WORLD); 
			//MPI_Test(&req, &flag, &sts);
			//printf("FLAG : %d",flag);

			//recibo del proc anterior a mi su última fila sin ceros, en mi primera fila que tiene ceros
			MPI_Recv(matriz_vieja, ANCHOMPRC+2, MPI_DOUBLE, n, tag, MPI_COMM_WORLD, &sts);	
			//MPI_Test(&req, &flag, &sts);
			//printf("FLAG : %d\n",flag);

		}
		if(s!=-1){
			//mando la última fila sin ceros al proceso que corresponda
			//MPI_Isend(matriz_vieja+(ALTOMPRC*(ANCHOMPRC+2)), ANCHOMPRC+2, MPI_DOUBLE, s, tag, MPI_COMM_WORLD, &req);
			//MPI_Test(&req, &flag, &sts);
			//printf("FLAG : %d\n",flag);
			MPI_Send(matriz_vieja+(ALTOMPRC*(ANCHOMPRC+2)), ANCHOMPRC+2, MPI_DOUBLE, s, tag, MPI_COMM_WORLD);
			//recbio en mi última fila con ceros lo que me venga del proc que me sigue, o sea, la primera fila sin ceros del proceso que sigue	
			MPI_Recv(matriz_vieja+((ALTOMPRC+1)*(ANCHOMPRC+2)), ANCHOMPRC+2, MPI_DOUBLE, s, tag, MPI_COMM_WORLD, &sts);
			//MPI_Test(&req, &flag, &sts);
			//printf("FLAG : %d\n",flag);
		}
		if(e!=-1){
			//mando la última columna sin ceros al proceso que corresponda
			//MPI_Isend(matriz_vieja+ANCHOMPRC, 1, nsubmatcol, rank+1, tag, MPI_COMM_WORLD, &req);
			//MPI_Test(&req, &flag, &sts);
			//printf("FLAG : %d\n",flag);
			MPI_Send(matriz_vieja+ANCHOMPRC, 1, nsubmatcol, rank+1, tag, MPI_COMM_WORLD);
			//recbio en mi última columna con ceros lo que me venga del proceso que sigue, o sea, la primera columna del proceso que sigue
			MPI_Recv(matriz_vieja+ANCHOMPRC+1, 1, nsubmatcol, rank+1, tag, MPI_COMM_WORLD, &sts);	
		}
		if(w!=-1){
			//mando la primera columna sin ceros al proceso que corresponda
			//MPI_Isend(matriz_vieja+1, 1, nsubmatcol, rank-1, tag, MPI_COMM_WORLD, &req);
			MPI_Send(matriz_vieja+1, 1, nsubmatcol, rank-1, tag, MPI_COMM_WORLD);
			//recbio en mi primera columna con ceros lo que me venga de la izquierda, o sea, la última columna del proceso anterior a mi
			MPI_Recv(matriz_vieja, 1, nsubmatcol, rank-1, tag, MPI_COMM_WORLD, &sts);	
		}

		
		if(rank==rank_fuente){
			matriz_actual[((coord_fuente_x%ALTOMPRC)+1 * (ANCHOMPRC+2)) + (coord_fuente_y%ANCHOMPRC+1)]=temp_fuente;	
		}

		calculoiterativo(matriz_actual,matriz_vieja, p_rows, perr_r);

		aux=matriz_actual;
		matriz_actual=matriz_vieja;
		matriz_vieja=aux;

		//versubmatriz(mvieja, ALTOMPRC+2, ANCHOMPRC+2);
		//versubmatriz(matriz_actual, ALTOMPRC+2, ANCHOMPRC+2);
				
		//printf("\nError_rank: %lf",error_rank);
		MPI_Allreduce(&error_rank, &error_total, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
		//printf("\nError_total: %lf\n",error_total);
		if(delta!=0 && error_total<=delta){
			break;		
		}
		
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
			if(src==1){
				MPI_Recv(matriz+(ANCHOMPRC*src), ALTOMPRC*ANCHOMPRC, MPI_DOUBLE, src, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);				
			}
			else{
				MPI_Recv(matriz+(ANCHOMPRC*src)+size, ALTOMPRC*ANCHOMPRC, MPI_DOUBLE, src, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);		
			}
		}
		
		
	
		printf("Tiempo transcurrido: %f\n", MPI_Wtime()-start_t);	
		generararchivo(matriz, size);	
		printf("Iteraciones realizadas: %d\n", iteraciones);
		printf("Error: %lf\n", error_total);

	}
	else{
		//NO soy el proceso 0, si no otro proceso
		
		//Creo un nuevo tipo, que va a posibilitar enviar mi submatriz al proc 0 para que arme la matriz principal
		MPI_Datatype submtp, nsubmtp;
		MPI_Type_vector(ALTOMPRC, ANCHOMPRC, ANCHOMPRC+2, MPI_DOUBLE, &submtp);
		MPI_Type_create_resized(submtp, 0, (ALTOMPRC*ANCHOMPRC)*sizeof(double),&nsubmtp);
		MPI_Type_commit(&nsubmtp);

		//le envío mi submatriz ya calculada al proc 0 para que lo guarde en la matriz principal
		MPI_Send(aux+ANCHOMPRC+3, 1, nsubmtp, 0, tag, MPI_COMM_WORLD);
		MPI_Type_free(&nsubmtp);
	}

	
	MPI_Type_free(&nsubmatcol);
	
	free(matriz);
	free(matriz_vieja);
	free(matriz_actual);

	MPI_Finalize();
	return 0;
}
