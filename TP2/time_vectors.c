#include "vectors.c"
#include "vectors.h"
#include<time.h>
#include<stdio.h>
#include<stdlib.h>

int main(int argc, char **argv){
	//argv[1] es el elemento a buscar, argv[2] es el tamaño del vector con el que se quiere trabajar

	int indice_elemento_a_buscar_encontrado;
	int tamaniomaximo=500;
	//int tamaniovector=atoi(argv[2]);
	//int elemento_a_buscar=atoi(argv[1]); //atoi() pasa de char** (ya que en realidad es una cadena) a int
	long suma1;
	int tamaniovector=10;
	int* vector1=(int*)malloc(tamaniovector*sizeof(int));
	clock_t startclk, endclk;
	double totalclk,totalclk2;

	cargarvector(vector1, tamaniovector);
	int elemento_a_buscar=vector1[tamaniovector-3];	
	
	printf("\ntamanio del vector  elemento a buscar  suma del vector  tiempo que tardó find()  tiempo que tardó sum()\n");	
	for(tamaniovector=10;tamaniovector<=tamaniomaximo;tamaniovector+=10){
		startclk = clock();	
		indice_elemento_a_buscar_encontrado=find(elemento_a_buscar,vector1,tamaniovector);
		endclk = clock();
		totalclk=(double)(endclk-startclk)/CLOCKS_PER_SEC;
	
		startclk=clock();
		suma1=sum(vector1,tamaniovector);
		endclk=clock();
		totalclk2=(double)(endclk-startclk)/CLOCKS_PER_SEC;
		printf("%d			%d		%li		  %lf		  %lf\n", tamaniovector, elemento_a_buscar, suma1, totalclk, totalclk2);	
		
	}
	free(vector1);
	return 0;
}
