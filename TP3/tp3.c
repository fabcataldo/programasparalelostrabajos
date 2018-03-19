#include<stdlib.h>
#include<stdio.h>
#include<omp.h>

int find(int value, int *v, int size){
	int i, resultado;
	int chunk = size/2;
	#pragma omp parallel private(resultado)
	resultado=0;
	{	
		#pragma omp parallel for schedule(dynamic, chunk) //manejo de hilos dinámico
		for(i=0;i<size;i++){
			if(value==v[i]){
				resultado=i;
			}
			else{
				resultado=-1;			
			}
		}
	}
	return resultado;
}

double sum(double *v, int size){
	int i;
	int chunk = size/2;
	double suma=0;
		#pragma omp parallel for reduction(+:suma) schedule(dynamic, chunk)//manejo de hilos dinámico
			for(i=0;i<size;i++){	
				suma+=v[i];	
			}
	return suma;
}
