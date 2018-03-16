#include "vectors.c"
#include "vectors.h"
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

typedef struct{
	int tamaniovector;
	int* semivector11;
	int elemento_a_buscar;
	int indice_elemento_a_buscar_encontrado;
}estructuraf1;

typedef struct{
	int tamaniovector;
	int elemento_a_buscar;
	int indice_elemento_a_buscar_encontrado;
	int* semivector12;
}estructuraf2;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //inicializo el mutex para sincronizar el uso de la variable de abajo
pthread_cond_t condicion = PTHREAD_COND_INITIALIZER; // va acompañado del mutex para hacer la sincronización
int banderafin; //bandera para indicar que uno de los hilos ha terminado

typedef struct{
	int tamaniovector;
	int* semivector11;
	int* semivector12;
	long sumaparcial1;
	long sumaparcial2;
}estructuras;

void* finditerativof1(void* arg){
	((estructuraf1*)arg)->indice_elemento_a_buscar_encontrado=0;	
	
	while (!banderafin) //dejo que el otro hilo comience a buscar, para que cuando encuentre el elemento, modifique la bandera
		pthread_cond_wait(&condicion, &mutex);	
	if(banderafin==0){
		printf("\n\nHilo 1 trabajando en la búsqueda...");
		((estructuraf1*)arg)->elemento_a_buscar=((estructuraf1*)arg)->semivector11[((estructuraf1*)arg)->tamaniovector-2];
		((estructuraf1*)arg)->indice_elemento_a_buscar_encontrado=find(((estructuraf1*)arg)->elemento_a_buscar, ((estructuraf1*)arg)->semivector11, ((estructuraf1*)arg)->tamaniovector);
		pthread_mutex_lock(&mutex); //sección crítica
		banderafin=1; //terminé el hilo	
		printf("\nLO ENCONTREEEEE");	
		printf("\ntamanio del vector  elemento a buscar  indice del elemento encontrado\n");
		printf("\n\t%d\t\t%d\t\t%d",((estructuraf1*)arg)->tamaniovector, ((estructuraf1*)arg)->elemento_a_buscar, ((estructuraf1*)arg)->indice_elemento_a_buscar_encontrado);
		free(((estructuraf1*)arg)->semivector11);
		pthread_mutex_unlock(&mutex); //desbloqueo diciendo "ya toqué banderafin"
		return arg;		
	}
	else{
		pthread_mutex_lock(&mutex); //sección crítica
		banderafin=1; 	
		printf("El hilo 2 ya encontró el elemento: %d", ((estructuraf1*)arg)->elemento_a_buscar);
		free(((estructuraf1*)arg)->semivector11);
		pthread_mutex_unlock(&mutex); //desbloqueo diciendo "ya toqué banderafin"
		return arg;		
	}
}

void* finditerativof2(void* arg){
	((estructuraf2*)arg)->indice_elemento_a_buscar_encontrado=0;	

	pthread_mutex_lock(&mutex); //sección crítica, bloqueo banderafin para luego modificar esta variable
	printf("\n\nHilo 2 trabajando en la búsqueda...");
	((estructuraf2*)arg)->elemento_a_buscar=((estructuraf2*)arg)->semivector12[((estructuraf2*)arg)->tamaniovector-2];
	((estructuraf2*)arg)->indice_elemento_a_buscar_encontrado=find(((estructuraf2*)arg)->elemento_a_buscar, ((estructuraf2*)arg)->semivector12, ((estructuraf1*)arg)->tamaniovector);
	if(((estructuraf2*)arg)->indice_elemento_a_buscar_encontrado != 0){
		printf("\nLO ENCONTREEEEE");
		printf("\ntamanio del vector  elemento a buscar  indice del elemento encontrado\n");
		banderafin=1; //terminé el hilo, y encontré el elemento	
		printf("\n\t%d\t\t%d\t\t%d",((estructuraf2*)arg)->tamaniovector, ((estructuraf2*)arg)->elemento_a_buscar, ((estructuraf2*)arg)->indice_elemento_a_buscar_encontrado);
		free(((estructuraf2*)arg)->semivector12);
		pthread_cond_signal(&condicion); //despierto al otro hilo
		pthread_mutex_unlock(&mutex); //desbloqueo diciendo "ya toqué banderafin", y encontré el elemento
		return arg;		
	}
	else{
		banderafin=0; //terminé el hilo, y no encontré el elemento	
		printf("\n\nNo lo encontre.");	
		printf("\nEl hilo 1 lo va a buscar ahora. Tranquilo!!\n");
		free(((estructuraf2*)arg)->semivector12);
		pthread_cond_signal(&condicion); //despierto al otro hilo
		pthread_mutex_unlock(&mutex); //desbloqueo diciendo "ya toqué banderafin", y no encontré el elemento
		return arg;		
	}
}


void* sumiterativos1(void* arg){
	((estructuras*)arg)->sumaparcial1=0;	
	int i;
	printf("\ntamanio del vector  suma parcial 1 del vector\n");
	for(i=0;i<((estructuras*)arg)->tamaniovector;i++){
		((estructuras*)arg)->sumaparcial1+=((estructuras*)arg)->semivector11[i];		
	}
	printf("\n\t%d\t\t%ld",((estructuras*)arg)->tamaniovector, ((estructuras*)arg)->sumaparcial1);
	free(((estructuras*)arg)->semivector11);
	pthread_exit(arg);
}

void* sumiterativos2(void* arg){
	((estructuras*)arg)->sumaparcial2=0;	
	int i;
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

	int n,i;
	printf("\nIngrese tamaño del vector a procesar: ");
	scanf("%d",&n);

	((estructuraf1*)pf1)->tamaniovector=n;
	((estructuraf2*)pf2)->tamaniovector=n;
	int* vector1 = (int*)malloc(n*sizeof(int));
	cargarvector(vector1, n);

	//primera mitad del vector para este primer hilo
	((estructuraf1*)pf1)->semivector11=(int*)malloc((n/2)*sizeof(int));
	for(i=0;i<(n/2);i++){
		((estructuraf1*)pf1)->semivector11[i]=vector1[i];
	}
	
	//primera mitad del vector para este primer hilo
	((estructuraf2*)pf2)->semivector12=(int*)malloc((n/2)*sizeof(int));
	for(i=(n/2);i<n;i++){
		((estructuraf2*)pf2)->semivector12[i]=vector1[i];
	}

/*	
	//creo dos hilos para find()
	printf("\nComienzo hilo 1 llamando a la función find()\n");
 	pthread_create(&idf1, NULL, finditerativof1, &f1);
	pthread_join(idf1,&pf1);
	printf("\nComienzo hilo 2 llamando a la función find()\n");
 	pthread_create(&idf2, NULL, finditerativof2, &f2);
	pthread_join(idf2,&pf2);
*/

	((estructuras*)ps1)->tamaniovector=n;
	//primera mitad del vector para este primer hilo
	((estructuras*)ps1)->semivector11=((estructuraf1*)pf1)->semivector11;
	
	//primera mitad del vector para este primer hilo
	((estructuras*)ps2)->tamaniovector=n;
	((estructuras*)ps2)->semivector12=((estructuraf2*)pf2)->semivector12;


	//y otro hilo que llame a suma variando el tamaño del vector
	printf("\nComienzo hilo 1 llamando a la función sum()\n");
 	pthread_create(&ids1, NULL, sumiterativos1, &s);
	pthread_join(ids1,&ps1);
	printf("\nComienzo hilo 2 llamando a la función sum()\n");
 	pthread_create(&ids2, NULL, sumiterativos2, &s);
	pthread_join(ids2,&ps2);

	return 0;
}
