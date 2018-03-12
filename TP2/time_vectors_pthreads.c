#include "vectors.c"
#include "vectors.h"
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>

typedef struct{
	int tamaniovector;
	int tamaniomaximo;
	int* vector1;
	int elemento_a_buscar;
	int indice_elemento_a_buscar_encontrado;
	long suma;
	clock_t startclk, endclk;
	double totalclk,totalclk2;
}estructura;

void* finditerativo(void* arg){
	((estructura*)arg)->tamaniovector=10;
	((estructura*)arg)->tamaniomaximo=400;
	((estructura*)arg)->vector1=(int*)malloc(((estructura*)arg)->tamaniovector*sizeof(int));
	cargarvector(((estructura*)arg)->vector1, ((estructura*)arg)->tamaniovector);
	((estructura*)arg)->elemento_a_buscar=((estructura*)arg)->vector1[((estructura*)arg)->tamaniovector-1];
	((estructura*)arg)->indice_elemento_a_buscar_encontrado=0;	

	printf("\ntamanio del vector  elemento a buscar  indice del elemento encontrado  tiempo en buscar el elemento\n");
	for(((estructura*)arg)->tamaniovector=10;((estructura*)arg)->tamaniovector<=((estructura*)arg)->tamaniomaximo;((estructura*)arg)->tamaniovector+=10){
		((estructura*)arg)->startclk=clock();
		((estructura*)arg)->indice_elemento_a_buscar_encontrado=find(((estructura*)arg)->elemento_a_buscar, ((estructura*)arg)->vector1, ((estructura*)arg)->tamaniovector);
		((estructura*)arg)->endclk=clock();
		((estructura*)arg)->totalclk=(double)(((estructura*)arg)->endclk-((estructura*)arg)->startclk)/CLOCKS_PER_SEC;
	
		printf("\n%d   %d   %d  %lf\n",((estructura*)arg)->tamaniovector, ((estructura*)arg)->elemento_a_buscar, ((estructura*)arg)->indice_elemento_a_buscar_encontrado, ((estructura*)arg)->totalclk);
	}
	free(((estructura*)arg)->vector1);
	return arg;
}

void* sumiterativo(void* arg){
	((estructura*)arg)->tamaniovector=10;
	((estructura*)arg)->tamaniomaximo=400;
	((estructura*)arg)->vector1=(int*)malloc(((estructura*)arg)->tamaniovector*sizeof(int));
	cargarvector(((estructura*)arg)->vector1, ((estructura*)arg)->tamaniovector);
	((estructura*)arg)->elemento_a_buscar=((estructura*)arg)->vector1[((estructura*)arg)->tamaniovector-1];
	((estructura*)arg)->indice_elemento_a_buscar_encontrado=0;	
	((estructura*)arg)->suma=0;

	printf("\ntamanio del vector  suma del vector   tiempo en realizar la suma\n");	
	for(((estructura*)arg)->tamaniovector=10;((estructura*)arg)->tamaniovector<=((estructura*)arg)->tamaniomaximo;((estructura*)arg)->tamaniovector+=10){
		((estructura*)arg)->startclk=clock();
		((estructura*)arg)->suma=sum(((estructura*)arg)->vector1, ((estructura*)arg)->tamaniovector);
		((estructura*)arg)->endclk=clock();
		((estructura*)arg)->totalclk2=(double)(((estructura*)arg)->endclk-((estructura*)arg)->startclk)/CLOCKS_PER_SEC;
		printf("\n%d   %ld    %lf\n",((estructura*)arg)->tamaniovector, ((estructura*)arg)->suma, ((estructura*)arg)->totalclk2);
	}
	free(((estructura*)arg)->vector1);
	pthread_exit(arg);
}


int main(int argc, char **argv){
	pthread_t id; //id de los hilos
	void* p = NULL; //puntero a lo que retorna la función
	estructura s; //puntero a la estructura

	//creo dos hilos, uno que llame a find() variando el tamaño del vector
	printf("\nComienzo un hilo llamando a la función find()");
 	pthread_create(&id, NULL, finditerativo, &s);
	pthread_join(id,&p);
	

	//y otro hilo que llame a suma variando el tamaño del vector
	printf("\nComienzo otro hilo llamando a la función sum()");
 	pthread_create(&id, NULL, sumiterativo, &s);
	pthread_join(id,&p);


	return 0;
}
