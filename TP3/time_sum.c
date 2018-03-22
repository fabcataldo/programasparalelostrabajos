#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<omp.h>

double sum(double *v, int size);

void cargarvector(double *vector, int size){
	int i;
	for(i=0;i<=size-1;i++){
		vector[i]=i;
	}
}

void cargarvector2(double *vector, int size){
	int i;
	for(i=0;i<=size-1;i++){
		vector[i]=sqrt(i);
	}
}

void cargarvector3(double *vector, int size){
	int i;
	for(i=0;i<size;i++){
		srand48(i);
		vector[i]=(((i*427345)+389421)%size)*drand48();
	}
}

void visualizarvector(double *vector, int size){
	int i;
	for(i=0;i<size;i++){
		printf("\n%lf",vector[i]);
	}
}

void tester(double* vector1, double* vector2, double* vector3, int num_hilos, int size){
	omp_set_num_threads(num_hilos);	
	double start1,start2,start3,end1,end2,end3,t1,t2,t3;
	double sum1,sum2,sum3;
	
	start1 = omp_get_wtime();
	sum1=sum(vector1, size);
	end1 = omp_get_wtime();
	t1=end1-start1;

	start2 = omp_get_wtime();
	sum2=sum(vector2, size);
	end2 = omp_get_wtime();
	t2=end2-start2;

	start3 = omp_get_wtime();
	sum3=sum(vector3, size);
	end3 = omp_get_wtime();
	t3=end3-start3;
	
	printf("\nsuma vector1: %.2lf", sum1);
	printf("\nsuma vector2: %.2lf", sum2);
	printf("\nsuma vector3: %.2lf\n", sum3);
	printf("tiempo que tardó sum() en el vector1: %lf\n",t1);
	printf("tiempo que tardó sum() en el vector2: %lf\n",t2);
	printf("tiempo que tardó sum() en el vector3: %lf\n",t3);
}



int main(int argc, char **argv){
	int size=16777216;
	double* vector1=(double*)malloc(size*sizeof(double));
	cargarvector(vector1,size);
	double* vector2=(double*)malloc(size*sizeof(double));
	cargarvector2(vector2,size);
	double* vector3=(double*)malloc(size*sizeof(double));
	cargarvector3(vector3,size);

	printf("\nSUMA PARALELIZADA");
	printf("\nLlamo a sum() con 2 hilos.");
	int num_hilos=2;
	tester(vector1,vector2,vector3,num_hilos,size);
	printf("\nLlamo a sum() con 3 hilos.");
	num_hilos=3;
	tester(vector1,vector2,vector3,num_hilos,size);
	printf("\nLlamo a sum() con 4 hilos.");
	num_hilos=4;
	tester(vector1,vector2,vector3,num_hilos,size);

	return 0;
}
