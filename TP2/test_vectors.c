#include "vectors.h"
#include<stdio.h>
#include<stdlib.h>

int main(int argc, char **argv){
	int n,el, i_el_finded;
	printf("\nIngrese tamaño del vector a procesar: ");
	scanf("%d",&n);
	printf("\nIngrese elemento del vector a buscar (el vector va a ser de la forma 1,2,3... tamaño n): ");
	scanf("%d",&el);
	
	
	printf("\nLlamo a find().");
	i_el_finded=find(el, NULL, n);
	printf("\nelemento a buscar  indice del elemento encontrado\n");
	printf("\n\t%d\t\t\t%d\n",el, i_el_finded);


	printf("\nLlamo a sum().");
	printf("\nSUMA TOTAL: %ld\n",sum(NULL, n));
	return 0;
}
