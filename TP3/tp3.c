#include<stdlib.h>
#include<stdio.h>
#include<omp.h>

int find(int value, int *v, int size){
	int i, resultado;
	int chunk = size/2;
	#pragma omp parallel private(resultado)//comparto v y size ya que nunca van a cambiar
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

long sum(int *vector, int size){
	long suma=0;
	int i;
	for(i=0;i<size;i++){
		suma+=vector[i];	
	}
	return suma;
}
