#include "vectors.c"
#include "vectors.h"
#include<stdio.h>
#include<stdlib.h>

int main(int argc, char **argv){
	//argv[1] es el elemento a buscar, argv[2] es el vector 1 o 2 ya cargados

	int size=10;
	int indice_elemento_a_buscar_encontrado;
	int tamaniomaximo=500;
	long suma1;
	long suma2;
	int tamaniovector=10;
	int elemento_a_buscar;
	int elemento_a_buscar_2;
	int* vector1=(int*)malloc(tamaniovector*sizeof(int));
	int* vector2=(int*)malloc(tamaniovector*sizeof(int));
	cargarvector(vector1, size);
	cargarvector2(vector2, size);

	printf("\ntamanio del vector  elemento a buscar  suma del vector1  suma del vector2\n");	
	elemento_a_buscar=vector1[tamaniovector-3];
	elemento_a_buscar_2=vector2[tamaniovector-3];	
	for(tamaniovector=10;tamaniovector<=tamaniomaximo;tamaniovector+=10){
		indice_elemento_a_buscar_encontrado=find(elemento_a_buscar,vector1,tamaniovector);
		suma1=sum(vector1,tamaniovector);
		suma2=sum(vector2,tamaniovector);
		printf("%d			%d		%li		%li\n", tamaniovector, elemento_a_buscar, suma1, suma2);	
		
	}

	free(vector1);
	free(vector2);
	return 0;
}
