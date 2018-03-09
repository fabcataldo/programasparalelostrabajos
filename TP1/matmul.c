#include <stdio.h>
#include <stdlib.h>


void matmul(double* a, double* b, double* c, int size){
	int i,j,k;
	for(i=0;i<size;i++)
		for(j=0;j<size;j++)
			c[i*size+j]=0;							

	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			for(k=0;k<size;k++){
				c[i*size+j]+=a[i*size+k]*b[k*size+j];
			}
		}	
	}
}

void matmul_5(double* a, double* b, double *c, int size){
	int i,j,k;
	for(i=0;i<size;i++)
		for(j=0;j<size;j++)
			c[i*size+j]=0;							

	for(i=0;i<size;i++)
		for(j=0;j<size;j++)
			for(k=0;k<size;k+5){
				c[i*size+j]+=a[i*size+k]*b[k*size+j]+a[i*size+(k+1)]*b[(k+1)*size+j]
					+a[i*size+(k+2)]*b[(k+2)*size+j]+a[i*size+(k+3)]*b[(k+3)*size+j]
					+a[i*size+(k+4)]*b[(k+4)*size+j];
				//printf("c[i*size+j]: %lf", c[i*size+j]);
				if(k<1){
					break;				
				}	
			}
}

void matmul_10(double* a, double* b,double *c, int size){
	int i,j,k;
	for(i=0;i<size;i++)
		for(j=0;j<size;j++)
			c[i*size+j]=0;							

	for(i=0;i<size;i++)
		for(j=0;j<size;j++)
			for(k=0;k<size;k+10){
				c[i*size+j]+=a[i*size+k]*b[k*size+j]+a[i*size+(k+1)]*b[(k+1)*size+j]
					+a[i*size+(k+2)]*b[(k+2)*size+j]+a[i*size+(k+3)]*b[(k+3)*size+j]
					+a[i*size+(k+4)]*b[(k+4)*size+j]+a[i*size+(k+5)]*b[(k+5)*size+j]
					+a[i*size+(k+6)]*b[(k+6)*size+j]+a[i*size+(k+7)]*b[(k+7)*size+j]
					+a[i*size+(k+8)]*b[(k+8)*size+j]+a[i*size+(k+9)]*b[(k+9)*size+j];
				if(k<1){
					break;				
				}
				//printf("c[i*size+j]: %lf", c[i*size+j]);
						
			}
}

void matriztranspuesta(double* matriz, double* matrizt, int size){
	int i, j;

	for(i=0;i<size;i++)
		for(j=0;j<size;j++)
			matrizt[i*size+j]=0;							

	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			matrizt[i*size+j]=matriz[j*size+i];	
		}	
	}
}

void matmul_t(double* a, double* b, double* c, double* bt, int size){
	int i,j,k;
	
	matriztranspuesta(b, bt, size);


	for(i=0;i<size;i++)
		for(j=0;j<size;j++)
			c[i*size+j]=0;							

	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			for(k=0;k<size;k++){
				//printf("zxzzzzz");
				c[i*size+j]+=a[i*size+k]*b[j*size+k];
				//printf("ASDADSASD:  %lf",c[i*size+j]);			
			}
		}	
	}
}

void matmul_t_5(double* a, double* b, double *c, double* bt, int size){
	int i,j,k;

	matriztranspuesta(b, bt, size);

	for(i=0;i<size;i++)
		for(j=0;j<size;j++)
			c[i*size+j]=0;							

	for(i=0;i<size;i++)
		for(j=0;j<size;j++)
			for(k=0;k<size;k+5){
				c[i*size+j]+=a[i*size+k]*b[k*size+j]+a[i*size+(k+1)]*b[(k+1)*size+j]
					+a[i*size+(k+2)]*b[(k+2)*size+j]+a[i*size+(k+3)]*b[(k+3)*size+j]
					+a[i*size+(k+4)]*b[(k+4)*size+j];
				if(k<1){
					break;				
				}	
			}
}

void matmul_t_10(double* a, double* b,double *c, double* bt, int size){
	int i,j,k;
	
	matriztranspuesta(b, bt, size);

	for(i=0;i<size;i++)
		for(j=0;j<size;j++)
			c[i*size+j]=0;							

	for(i=0;i<size;i++)
		for(j=0;j<size;j++)
			for(k=0;k<size;k+10){
				c[i*size+j]+=a[i*size+k]*b[k*size+j]+a[i*size+(k+1)]*b[(k+1)*size+j]
					+a[i*size+(k+2)]*b[(k+2)*size+j]+a[i*size+(k+3)]*b[(k+3)*size+j]
					+a[i*size+(k+4)]*b[(k+4)*size+j]+a[i*size+(k+5)]*b[(k+5)*size+j]
					+a[i*size+(k+6)]*b[(k+6)*size+j]+a[i*size+(k+7)]*b[(k+7)*size+j]
					+a[i*size+(k+8)]*b[(k+8)*size+j]+a[i*size+(k+9)]*b[(k+9)*size+j];
				if(k<1){
					break;				
				}
				//printf("c[i*size+j]: %lf", c[i*size+j]);
						
			}
}


void cargarmatriz(double* matriz, int size){
	int i,j;

	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			printf("Matriz [%d][%d]: ",i,j);
			scanf("%lf",&matriz[i*size+j]);
		}
	}
}

void mostrarmatriz(double* matriz, int size){
	int i,j;
	for(i=0;i<size;i++){
		for(j=0;j<size;j++)
			printf("\nmatriz[%d][%d]: %.3lf \n",i,j,matriz[i*size+j]);
			
	}
}


/*void liberarmatriz(double* matriz, int size){
	int i,j;

	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			free(matriz[i+j*size]);
		}
	}
}*/

int main(){
	int n,i,j;

	printf("Ingrese el tamanio de la matriz a y b: ");
	scanf("%d",&n);
	
	double* matriza=malloc(n*sizeof(double));
	double* matrizb=malloc(n*sizeof(double)); 
	double*	matrizc=malloc(n*sizeof(double));

	double* matrizt=malloc(n*sizeof(double)); 

	printf("Ahora, ingresamos los distintos valores de la matriz A, por fila:\n");	
	cargarmatriz(matriza, n);
	printf("Ahora, ingresamos los distintos valores de la matriz B, por fila:\n");	
	cargarmatriz(matrizb, n);	

	printf("\n\n\n");	
	printf("Ahora, MOSTRAMOS MATRIZ A:\n");	
	mostrarmatriz(matriza, n);
	printf("\nAhora, MOSTRAMOS MATRIZ B:\n");	
	mostrarmatriz(matrizb, n);
	
	/*matmul_5(matriza,matrizb,matrizc,n);
	printf("\n\n");
	
	printf("\nAhora, MOSTRAMOS MATRIZ C resultado:\n");	
	mostrarmatriz(matrizc,n);
	*/

	/*matmul_10(matriza,matrizb,matrizc,n);
	printf("\n\n");
	
	printf("\nAhora, MOSTRAMOS MATRIZ C resultado:\n");	
	mostrarmatriz(matrizc,n);
	*/

	/*printf("\n\n\nTRANSPONEMOS MATRIZ B");
	matriztranspuesta(matrizb,matrizt,n);	
	
	printf("\nMOSTRAMOS matriz bT");
	mostrarmatriz(matrizt, n);*/

	/*printf("\nProbamos matmul_t");
	matmul_t(matriza, matrizb, matrizc, matrizt, n);
	
	printf("\nAhora, MOSTRAMOS MATRIZ C resultado:\n");	
	mostrarmatriz(matrizc,n);
	*/

	/*free(matriza);
	free(matrizb);
	free(matrizc);
	*/

	printf("\n");
}
