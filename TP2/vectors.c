#include "vectors.h"
#include<stdlib.h>
#include<stdio.h>

int find(int value, int *vector, int size){
	int i;
	for(i=0;i<size;i++){
		if(value==vector[i]){
			return i;
		}
	}
	return -1;
}

long sum(int *vector, int size){
	long suma=0;
	int i;
	for(i=0;i<size;i++){
		suma+=vector[i];	
	}
	return suma;
}

void cargarvector(int *vector, int size){
	int i;
	int contador=1;
	for(i=0;i<size;i++){
		vector[i]=contador;
		contador++;
	}
}

void visualizarvector(int *vector, int size){
	int i;
	for(i=0;i<size;i++){
		printf("\n%d",vector[i]);
	}
}
