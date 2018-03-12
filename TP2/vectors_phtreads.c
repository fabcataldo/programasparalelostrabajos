#include "vectors.h"
#include<stdlib.h>
#include<stdio.h>

int find(int value, int *vector, int size){
	int i;
	int resultado=-1;
	for(i=0;i<size;i++){
		if(value==vector[i]){
			resultado=i;
		}
		else{
			resultado=-1;		
		}	
	}
	return resultado;
}

long sum(int *vector, int size){
	long suma;
	int i;
	for(i=0;i<size;i++){
		suma+=vector[i];	
	}
	return suma;
}

void cargarvector(int *vector, int size){
	int i;
	for(i=0;i<size;i++){
		vector[i]=i+1;
	}
}

void visualizarvector(int *vector, int size){
	int i;
	for(i=0;i<size;i++){
		printf("\n%d",vector[i]);
	}
}
