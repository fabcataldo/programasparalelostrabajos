#include "vectors.h"
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>

/* retorna "a - b" en segundos */
double timeval_diff(struct timeval *a, struct timeval *b)
{
  return
    (double)(a->tv_sec + (double)a->tv_usec/1000000) -
    (double)(b->tv_sec + (double)b->tv_usec/1000000);
}

int main(int argc, char **argv){
	struct timeval t_ini, t_fin;
	double secs,secs1;
	secs = timeval_diff(&t_fin, &t_ini);

	int n,el, i_el_finded;
	long suma;
	printf("\nIngrese tamaño del vector a procesar: ");
	scanf("%d",&n);
	printf("\nIngrese elemento del vector a buscar (el vector va a ser de la forma 1,2,3... tamaño n): ");
	scanf("%d",&el);
	
	
	printf("\nLlamo a find().");
	gettimeofday(&t_ini, NULL);	
	i_el_finded=find(el, NULL, n);
	gettimeofday(&t_fin, NULL);
	secs = timeval_diff(&t_fin, &t_ini);
	printf("\nelemento a buscar  indice del elemento encontrado  tiempo (en seg) que tardó find()\n");
	printf("\n\t%d\t\t\t\t%d\t\t%lf seg\n",el, i_el_finded, secs);

	printf("\nLlamo a sum().");
	gettimeofday(&t_ini, NULL);	
	suma=sum(NULL,n);
	gettimeofday(&t_fin, NULL);
	secs1 = timeval_diff(&t_fin, &t_ini);

	printf("\nsuma total del vector  tiempo que tardó sum()  \n");	
	printf("%ld\t\t\t%lf seg\n", suma, secs1);	

	return 0;
}
