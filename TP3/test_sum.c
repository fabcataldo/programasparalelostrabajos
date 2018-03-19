#include<stdio.h>
#include<stdlib.h>
#include<math.h>

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

double sum(double *v, int size);

int main(int argc, char **argv){
	double sum1,sum2,sum3;
	int size=16777216;
	double* vector1=(double*)malloc(size*sizeof(double));
	cargarvector(vector1,size);
	double* vector2=(double*)malloc(size*sizeof(double));
	cargarvector2(vector2,size);
	double* vector3=(double*)malloc(size*sizeof(double));
	cargarvector3(vector3,size);

	printf("\nSUMA PARALELIZADA");

	printf("\nLlamo a sum().");
	sum1=sum(vector1, size);
	sum2=sum(vector2, size);
	sum3=sum(vector3, size);
	printf("\nsuma vector1                suma vector2         suma vector3\n");
	printf("\n%lf   %lf    %lf\n", sum1, sum2, sum3);

	return 0;
}
