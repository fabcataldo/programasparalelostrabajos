#include <stdio.h>
#include <stdlib.h>
#include <time.h>


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
				c[i*size+j]+=a[i*size+k]*bt[j*size+k];		
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
				c[i*size+j]+=a[i*size+k]*bt[j*size+k] + a[i*size+(k+1)]*bt[j*size+(k+1)]
					+a[i*size+(k+2)]*bt[j*size+(k+2)] + a[i*size+(k+3)]*bt[j*size+(k+3)]
					+a[i*size+(k+4)]*bt[j*size+(k+4)];
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
				c[i*size+j]+=a[i*size+k]*bt[k*size+j]+a[i*size+(k+1)]*bt[j*size+(k+1)]
					+a[i*size+(k+2)]*bt[j*size+(k+2)]+a[i*size+(k+3)]*bt[j*size+(k+3)]
					+a[i*size+(k+4)]*bt[j*size+(k+4)]+a[i*size+(k+5)]*bt[j*size+(k+5)]
					+a[i*size+(k+6)]*bt[j*size+(k+6)]+a[i*size+(k+7)]*bt[j*size+(k+7)]
					+a[i*size+(k+8)]*bt[j*size+(k+8)]+a[i*size+(k+9)]*bt[j*size+(k+9)];
				if(k<1){
					break;				
				}						
			}
}


void cargarmatriz(double* matriz, int size){
	int i,j;
	
	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			matriz[i*size+j]=rand();
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


int main(int argc, char **argv){
	int n,i,j;	
	double* matriza=(double*)malloc(n*n*sizeof(double));
	double* matrizb=(double*)malloc(n*n*sizeof(double)); 
	double*	matrizc=(double*)malloc(n*n*sizeof(double));
	double* matrizt=(double*)malloc(n*n*sizeof(double));
	clock_t startclk, endclk;
	double totalclk1,totalclk2,totalclk3,totalclk4,totalclk5,totalclk6; 
	FILE *fp = fopen(argv[1], "w+");

	printf("Ingrese el tamanio de la matriz a y b: ");
	scanf("%d",&n);
	
	cargarmatriz(matriza, n);
	cargarmatriz(matrizb, n);	
	
	printf("n   matmul   matmul_5   matmul_10   matmul_t   matmul_t_5   matmul_t_10\n");
	fprintf(fp, "n   matmul   matmul_5   matmul_10   matmul_t   matmul_t_5   matmul_t_10\n");
		
	startclk = clock();
	matmul(matriza,matrizb,matrizc,n);
	endclk = clock();
	totalclk1 = (double) (endclk-startclk)/CLOCKS_PER_SEC;	
	
	startclk = clock();
	matmul_5(matriza,matrizb,matrizc,n);
	endclk = clock();
	totalclk2 = (double) (endclk-startclk)/CLOCKS_PER_SEC;	
	
	startclk = clock();
	matmul_10(matriza,matrizb,matrizc,n);
	endclk = clock();
	totalclk3 = (double) (endclk-startclk)/CLOCKS_PER_SEC;	
	
	startclk = clock();
	matmul_t(matriza, matrizb, matrizc, matrizt, n);
	endclk = clock();
	totalclk4 = (double) (endclk-startclk)/CLOCKS_PER_SEC;	
		
	startclk = clock();
	matmul_t_5(matriza, matrizb, matrizc, matrizt, n);
	endclk = clock();
	totalclk5 = (double) (endclk-startclk)/CLOCKS_PER_SEC;	

	startclk = clock();
	matmul_t_10(matriza, matrizb, matrizc, matrizt, n);
	endclk = clock();
	totalclk6 = (double) (endclk-startclk)/CLOCKS_PER_SEC;	
	printf("%d   %lf   %lf   %lf   %lf   %lf     %lf\n", n, totalclk1, totalclk2, totalclk3, totalclk4, totalclk5, totalclk6);
	fprintf(fp, "%d   %lf   %lf   %lf   %lf   %lf     %lf", n, totalclk1, totalclk2, totalclk3, totalclk4, totalclk5, totalclk6);
	
	//fclose(fp);
	/*free(matriza);
	free(matrizb);
	free(matrizc);
	free(matrizt);*/
}
