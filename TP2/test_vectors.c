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
	int tamaniovector=10;
	int elemento_a_buscar;
	int* vector1=(int*)malloc(tamaniovector*sizeof(int));
	cargarvector(vector1, size);
	

	printf("\ntamanio del vector  elemento a buscar  suma del vector1   indice del elem encontrado\n");		
	for(tamaniovector=10;tamaniovector<=tamaniomaximo;tamaniovector+=10){
		suma1=0;
		if(tamaniovector>10){
			vector1=(int*)malloc(tamaniovector*sizeof(int));
			cargarvector(vector1, tamaniovector);
		}
		elemento_a_buscar=vector1[tamaniovector-2];

		indice_elemento_a_buscar_encontrado=find(elemento_a_buscar,vector1,tamaniovector);
		suma1=sum(vector1,tamaniovector);
		printf("%d\t\t\t%d\t\t\t%ld\t\t\t%d\n", tamaniovector, elemento_a_buscar, suma1, indice_elemento_a_buscar_encontrado);	
		
	}

	free(vector1);
	return 0;
}
