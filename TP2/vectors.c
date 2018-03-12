#include "vectors.h"
#include<stdlib.h>
#include<stdio.h>

int find(int value, int *vector, int size){
	int i;
	int resultado=-1;
//	printf("asdasdads");
	//int *puntero = &vector[0];
	for(i=0;i<size;i++){
		//printf("\nasdasd %d", *(puntero+i));
		if(value==vector[i]){
			resultado=i;
//			printf("\nresultado: %d",resultado);
		}
		else{
//			printf("\nresultado: %d",resultado);
			resultado=-1;		
		}	
	}
	return resultado;
}

long sum(int *vector, int size){
	long suma;
	int i;
	for(i=0;i<size;i++){
//		printf("suma: %d",suma);
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

void cargarvector2(int *vector, int size){
	int i;
	for(i=0;i<size;i++){
		vector[i]=(i+1)*2;
	}
}

void visualizarvector(int *vector, int size){
	int i;
	for(i=0;i<size;i++){
		printf("\n%d",vector[i]);
	}
}
