#include "vectors.c"
#include "vectors.h"
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

typedef struct{
	int tamaniovector;
	int* vector1;
	int* semivector11;
	int elemento_a_buscar;
	int indice_elemento_a_buscar_encontrado;
}estructuraf1;

typedef struct{
	int tamaniovector;
	int* vector1;
	int elemento_a_buscar;
	int indice_elemento_a_buscar_encontrado;
	int* semivector12;
}estructuraf2;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //inicializo el mutex para sincronizar el uso de la variable de abajo
pthread_cond_t condicion = PTHREAD_COND_INITIALIZER; // va acompañado del mutex para hacer la sincronización
int banderafin; //bandera para indicar que uno de los hilos ha terminado

typedef struct{
	int* vector1;
	int tamaniovector;
	int* semivector11;
	int* semivector12;
	long sumaparcial1;
	long sumaparcial2;
}estructuras;

void* finditerativof1(void* arg){
	((estructuraf1*)arg)->indice_elemento_a_buscar_encontrado=0;	
	int i;
	((estructuraf1*)arg)->semivector11 = (int*)malloc((((estructuraf1*)arg)->tamaniovector)*sizeof(int));
	for(i=0;i<(((estructuraf1*)arg)->tamaniovector/2);i++){
		((estructuraf1*)arg)->semivector11[i]=((estructuraf1*)arg)->vector1[i];
	}
		
	//while (!banderafin) //dejo que el otro hilo comience a buscar, para que cuando encuentre el elemento, modifique la bandera
		//pthread_cond_wait(&condicion, &mutex);	
	printf("\n\nHilo 1 trabajando en la búsqueda...");
	((estructuraf1*)arg)->indice_elemento_a_buscar_encontrado=find(((estructuraf1*)arg)->elemento_a_buscar, ((estructuraf1*)arg)->semivector11, ((estructuraf1*)arg)->tamaniovector);

	if(((estructuraf1*)arg)->indice_elemento_a_buscar_encontrado != -1){
		pthread_mutex_lock(&mutex); //sección crítica
		banderafin=1; //terminé el hilo	
		printf("\nLO ENCONTREEEEE");	
		printf("\ntamanio del vector en hilo 1  elemento a buscar  indice del elemento encontrado\n");
		printf("\n\t%d\t\t%d\t\t%d",((estructuraf1*)arg)->tamaniovector, ((estructuraf1*)arg)->elemento_a_buscar, ((estructuraf1*)arg)->indice_elemento_a_buscar_encontrado);
		free(((estructuraf1*)arg)->semivector11);
		pthread_mutex_unlock(&mutex); //desbloqueo diciendo "ya toqué banderafin"
		return arg;		
	}
	else{
		pthread_mutex_lock(&mutex); //sección crítica
		banderafin=0; 	
		free(((estructuraf1*)arg)->semivector11);
		pthread_mutex_unlock(&mutex); //desbloqueo diciendo "ya toqué banderafin"
		return arg;		
	}
}

void* finditerativof2(void* arg){
	//while (!banderafin) //dejo que el otro hilo comience a buscar, para que cuando encuentre el elemento, modifique la bandera
		//pthread_cond_wait(&condicion, &mutex);
	if(banderafin==0){
		((estructuraf2*)arg)->indice_elemento_a_buscar_encontrado=0;	
	
		int i;
		((estructuraf2*)arg)->semivector12 = (int*)malloc((((estructuraf2*)arg)->tamaniovector/2)*sizeof(int));

		for(i=((estructuraf2*)arg)->tamaniovector/2;i<((estructuraf2*)arg)->tamaniovector;i++){
			((estructuraf2*)arg)->semivector12[i]=((estructuraf2*)arg)->vector1[i];
		}

		pthread_mutex_lock(&mutex); //sección crítica, bloqueo banderafin para luego modificar esta variable
		printf("\n\nHilo 2 trabajando en la búsqueda...");
		((estructuraf2*)arg)->indice_elemento_a_buscar_encontrado=find(((estructuraf2*)arg)->elemento_a_buscar, ((estructuraf2*)arg)->semivector12, ((estructuraf1*)arg)->tamaniovector);
		if(((estructuraf2*)arg)->indice_elemento_a_buscar_encontrado != 0){
			printf("\nLO ENCONTREEEEE");
			printf("\ntamanio del vector en hilo 2  elemento a buscar  indice del elemento encontrado\n");
			banderafin=1; //terminé el hilo, y encontré el elemento	
			printf("\n\t%d\t\t\t%d\t\t\t%d\n",((estructuraf2*)arg)->tamaniovector, ((estructuraf2*)arg)->elemento_a_buscar, ((estructuraf2*)arg)->indice_elemento_a_buscar_encontrado);
			free(((estructuraf2*)arg)->semivector12);
			//pthread_cond_signal(&condicion); //despierto al otro hilo
			pthread_mutex_unlock(&mutex); //desbloqueo diciendo "ya toqué banderafin", y encontré el elemento
			return arg;		
		}
		else{
			banderafin=0; //terminé el hilo, y no encontré el elemento	
			printf("\n\nNo lo encontre.");	
			printf("\nEl hilo 1 lo va a buscar ahora. Tranquilo!!\n");
			free(((estructuraf2*)arg)->semivector12);
			//pthread_cond_signal(&condicion); //despierto al otro hilo
			pthread_mutex_unlock(&mutex); //desbloqueo diciendo "ya toqué banderafin", y no encontré el elemento
			return arg;		
		}
	}
	else{
		pthread_mutex_lock(&mutex); //sección crítica
		banderafin=1; //terminé el hilo	
		//pthread_cond_signal(&condicion);	
		pthread_mutex_unlock(&mutex);
		return arg;
	}
}


void* sumiterativos1(void* arg){
	int i;
	((estructuras*)arg)->semivector11 = (int*)malloc((((estructuras*)arg)->tamaniovector)*sizeof(int));
	for(i=0;i<(((estructuras*)arg)->tamaniovector/2);i++){
		((estructuras*)arg)->semivector11[i]=((estructuras*)arg)->vector1[i];
	}

	((estructuras*)arg)->sumaparcial1=0;	
	printf("\ntamanio del vector  suma parcial 1 del vector\n");
	for(i=0;i<((estructuras*)arg)->tamaniovector;i++){
		((estructuras*)arg)->sumaparcial1+=((estructuras*)arg)->semivector11[i];		
	}
	printf("\n\t%d\t\t%ld",((estructuras*)arg)->tamaniovector, ((estructuras*)arg)->sumaparcial1);
	free(((estructuras*)arg)->semivector11);
	pthread_exit(arg);
}

void* sumiterativos2(void* arg){
	int i;
	((estructuras*)arg)->semivector12 = (int*)malloc((((estructuras*)arg)->tamaniovector)*sizeof(int));
	for(i=((estructuras*)arg)->tamaniovector/2;i<((estructuras*)arg)->tamaniovector;i++){
		((estructuras*)arg)->semivector12[i]=((estructuras*)arg)->vector1[i];
	}
	visualizarvector(((estructuras*)arg)->semivector12,((estructuras*)arg)->tamaniovector/2);
	((estructuras*)arg)->sumaparcial2=0;	
	printf("\ntamanio del vector  suma parcial 2 del vector\n");
	for(i=0;i<((estructuras*)arg)->tamaniovector;i++){
		((estructuras*)arg)->sumaparcial2+=((estructuras*)arg)->semivector12[i];		
	}
	printf("\n\t%d\t\t%ld",((estructuras*)arg)->tamaniovector, ((estructuras*)arg)->sumaparcial2);
	free(((estructuras*)arg)->semivector12);
	pthread_exit(arg);
}


int main(int argc, char **argv){
	pthread_t idf1; //id de hilo
	pthread_t idf2; //id de hilo
	void* pf1 = NULL; //puntero a lo que retorna la función de finditerativof1
	void* pf2 = NULL;//puntero a lo que retorna la función de finditerativof1
	estructuraf1 f1; //puntero a la estructura del hilo 1 para find()
	estructuraf2 f2; //puntero a la estructura del hilo 2 para find()

	pthread_t ids1; //id de hilo
	pthread_t ids2; //id de hilo
	void* ps1 = NULL; //puntero a lo que retorna la función de sumiterativos1()
	void* ps2 = NULL;//puntero a lo que retorna la función de sumiterativos2()
	estructuras s; //puntero a la estructura del hilo 1 y 2 para sumiterativos2()

	int n,el;
	printf("\nIngrese tamaño del vector a procesar: ");
	scanf("%d",&n);
	printf("\nIngrese elemento del vector a buscar (el vector va a ser de la forma 1,2,3... tamaño n): ");
	scanf("%d",&el);
	
	printf("\n Cada hilo va a trabajar con dos mitades del vector");
	f1.elemento_a_buscar=el;
	f2.elemento_a_buscar=el;	
	//primera mitad del vector para este primer hilo
	f1.vector1 = (int*)malloc(n*sizeof(int));
	cargarvector(f1.vector1, n);

	//primera mitad del vector para este primer hilo
	f2.vector1 = (int*)malloc(n*sizeof(int));
	cargarvector(f2.vector1, n);
	f1.tamaniovector=n;
	f2.tamaniovector=n;	

	//creo dos hilos para find()
	printf("\nComienzo hilo 1 llamando a la función find()\n");
 	pthread_create(&idf1, NULL, finditerativof1, &f1);
	pthread_join(idf1,&pf1);
	printf("\nComienzo hilo 2 llamando a la función find()\n");
 	pthread_create(&idf2, NULL, finditerativof2, &f2);
	pthread_join(idf2,&pf2);

	printf("-----------------------------------------------------------------------------\n");
	printf("\n\nCada hilo va a trabajar con dos mitades del vector");
	s.tamaniovector=n;
	s.tamaniovector=n;
	s.vector1 = (int*)malloc(n*sizeof(int));
	cargarvector(s.vector1, n); 

	//y otro hilo que llame a suma variando el tamaño del vector
	printf("\nComienzo hilo 1 llamando a la función sum()\n");
 	pthread_create(&ids1, NULL, sumiterativos1, &s);
	pthread_join(ids1,&ps1);
	printf("\nComienzo hilo 2 llamando a la función sum()\n");
 	pthread_create(&ids2, NULL, sumiterativos2, &s);
	pthread_join(ids2,&ps2);

	int sumatotal=s.sumaparcial1+s.sumaparcial2;
	printf("\nSUMA TOTAL: %d\n",sumatotal);
	return 0;
}
