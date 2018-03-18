#include "vectors.h"
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<math.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //inicializo el mutex para sincronizar el uso de la variable de abajo
pthread_cond_t condicion = PTHREAD_COND_INITIALIZER; // va acompañado del mutex para hacer la sincronización
int banderafin; //bandera para indicar que uno de los hilos ha terminado

typedef struct{
	int tamaniovector;
	int* vector1;
	int* semivector11;
	int elemento_a_buscar;
	int indice_elemento_a_buscar_encontrado;
	int* semivector12;
}estructuraf1;

typedef struct{
	int* vector1;
	int tamaniovector;
	int* semivector11;
	int* semivector12;
	long sumaparcial1;
	long sumaparcial2;
}estructuras;


void* finditerativof1(void* arg){
	printf("\n\nHilo 1 trabajando en la búsqueda...");

	((estructuraf1*)arg)->indice_elemento_a_buscar_encontrado=0;	
	int i;
	((estructuraf1*)arg)->semivector11 = (int*)malloc((((estructuraf1*)arg)->tamaniovector)*sizeof(int));
	int tamaniodelvector=((estructuraf1*)arg)->tamaniovector;
	
	for(i=0;i<ceil(tamaniodelvector/2);i++){
		((estructuraf1*)arg)->semivector11[i]=((estructuraf1*)arg)->vector1[i];
	}


	for(i=0;i<ceil(tamaniodelvector/2);i++){
		if(((estructuraf1*)arg)->elemento_a_buscar==((estructuraf1*)arg)->semivector11[i]){
			((estructuraf1*)arg)->indice_elemento_a_buscar_encontrado=i;
		}
		else{
			((estructuraf1*)arg)->indice_elemento_a_buscar_encontrado=-1;		
		}
	}


	if(((estructuraf1*)arg)->indice_elemento_a_buscar_encontrado != -1){
		pthread_mutex_lock(&mutex); //sección crítica
		banderafin=1; //terminé el hilo	
		printf("\nSoy el hilo 1 y... LO ENCONTREEEEE");	
		free(((estructuraf1*)arg)->semivector11);
		pthread_mutex_unlock(&mutex); //desbloqueo diciendo "ya toqué banderafin"
		return arg;		
	}
	else{
		pthread_mutex_lock(&mutex); //sección crítica
		banderafin=0; 	
		free(((estructuraf1*)arg)->semivector11);
		pthread_mutex_unlock(&mutex); //desbloqueo diciendo "ya toqué banderafin"
		printf("\nSoy el hilo 1 y... NOO LO ENCONTREEEEE");	
		return arg;		
	}
}

void* finditerativof2(void* arg){
	//while (!banderafin) //dejo que el otro hilo comience a buscar, para que cuando encuentre el elemento, modifique la bandera
		//pthread_cond_wait(&condicion, &mutex);
	if(banderafin==0){
		printf("\n\nHilo 2 trabajando en la búsqueda...");

		((estructuraf1*)arg)->indice_elemento_a_buscar_encontrado=0;	
	
		int tamaniodelvector=((estructuraf1*)arg)->tamaniovector;
		int i;
		((estructuraf1*)arg)->semivector12 = (int*)malloc((((estructuraf1*)arg)->tamaniovector/2)*sizeof(int));
		for(i=ceil(tamaniodelvector/2);i<((estructuraf1*)arg)->tamaniovector;i++){
			((estructuraf1*)arg)->semivector12[i]=((estructuraf1*)arg)->vector1[i];
		}


		for(i=0;i<ceil(tamaniodelvector/2);i++){
			if(((estructuraf1*)arg)->elemento_a_buscar==((estructuraf1*)arg)->semivector11[i]){
				((estructuraf1*)arg)->indice_elemento_a_buscar_encontrado=i;
			}
			else{
				((estructuraf1*)arg)->indice_elemento_a_buscar_encontrado=-1;
			}
		}


		pthread_mutex_lock(&mutex); //sección crítica, bloqueo banderafin para luego modificar esta variable

		if(((estructuraf1*)arg)->indice_elemento_a_buscar_encontrado != -1){
			banderafin=1; //terminé el hilo, y encontré el elemento
			free(((estructuraf1*)arg)->semivector12);
			//pthread_cond_signal(&condicion); //despierto al otro hilo
			pthread_mutex_unlock(&mutex); //desbloqueo diciendo "ya toqué banderafin", y encontré el elemento
			printf("\nSoy el hilo 2 y... LO ENCONTREEEEE");	
			return arg;		
		}
		else{
			banderafin=0; //terminé el hilo, y no encontré el elemento	
			printf("\nSoy el hilo 2 y no lo encontre.");	
			printf("\nEl hilo 1 lo va a buscar ahora. Tranquilo!!\n");
			free(((estructuraf1*)arg)->semivector12);
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
	int tamaniodelvector=((estructuras*)arg)->tamaniovector;
	
	for(i=0;i<ceil(tamaniodelvector/2);i++){
		((estructuras*)arg)->semivector11[i]=((estructuras*)arg)->vector1[i];
	}

	((estructuras*)arg)->sumaparcial1=0;	
	printf("\ntamanio del vector  suma parcial 1 del vector\n");
	for(i=0;i<ceil(tamaniodelvector/2);i++){
		((estructuras*)arg)->sumaparcial1+=((estructuras*)arg)->semivector11[i];		
	}
	printf("\n\t%d\t\t%ld",((estructuras*)arg)->tamaniovector, ((estructuras*)arg)->sumaparcial1);
	free(((estructuras*)arg)->semivector11);
	pthread_exit(arg);
}

void* sumiterativos2(void* arg){
	int i,j;
	((estructuras*)arg)->semivector12 = (int*)malloc((((estructuras*)arg)->tamaniovector)*sizeof(int));

	int tamaniodelvector=((estructuras*)arg)->tamaniovector;
	for(i=(tamaniodelvector/2);i<((estructuras*)arg)->tamaniovector;i++){ //CORREGIR
		for(j=0;j<(tamaniodelvector/2);j++)
			((estructuras*)arg)->semivector12[j]=((estructuras*)arg)->vector1[i];
	}
	((estructuras*)arg)->sumaparcial2=0;	
	printf("\ntamanio del vector  suma parcial 2 del vector\n");
	for(i=0;i<(tamaniodelvector/2);i++){
		((estructuras*)arg)->sumaparcial2+=((estructuras*)arg)->semivector12[i];		
	}
	printf("\n\t%d\t\t%ld",((estructuras*)arg)->tamaniovector, ((estructuras*)arg)->sumaparcial2);
	free(((estructuras*)arg)->semivector12);
	pthread_exit(arg);
}

int find(int value, int *vector, int size){
	pthread_t idf1; //id de hilo 1
	pthread_t idf2; //id de hilo 2
	void* pf1 = NULL; //puntero 1 a lo que retorna la función de finditerativof1
	void* pf2 = NULL;//puntero 2 a lo que retorna la función de finditerativof1
	estructuraf1 f1; //puntero a la estructura del hilo 1 e hilo 2 para find()
	
	f1.elemento_a_buscar=value;	
	f1.vector1 = (int*)malloc(size*sizeof(int));
	cargarvector(f1.vector1, size);
	f1.tamaniovector=size;

	printf("\nComienzo hilo 1 llamando a la función find()\n");
 	pthread_create(&idf1, NULL, finditerativof1, &f1);
	pthread_join(idf1,&pf1);	

	printf("\nComienzo hilo 2 llamando a la función find()\n");
 	pthread_create(&idf2, NULL, finditerativof2, &f1);
	pthread_join(idf2,&pf2);

	if(((estructuraf1*)pf1)->indice_elemento_a_buscar_encontrado!=-1){
		return ((estructuraf1*)pf1)->indice_elemento_a_buscar_encontrado;
	}
	else{
		if(((estructuraf1*)pf2)->indice_elemento_a_buscar_encontrado!=-1){
			return ((estructuraf1*)pf2)->indice_elemento_a_buscar_encontrado;
		}
	}
	return -1;
}

long sum(int *vector, int size){
	pthread_t ids1; //id de hilo
	pthread_t ids2; //id de hilo
	void* ps1 = NULL; //puntero a lo que retorna la función de sumiterativos1()
	void* ps2 = NULL;//puntero a lo que retorna la función de sumiterativos2()
	estructuras s; //puntero a la estructura del hilo 1 y 2 para sumiterativos2()

	s.tamaniovector=size;
	s.vector1 = (int*)malloc(size*sizeof(int));
	cargarvector(s.vector1, size); 

 	pthread_create(&ids1, NULL, sumiterativos1, &s);
	pthread_join(ids1,&ps1);
 	pthread_create(&ids2, NULL, sumiterativos2, &s);
	pthread_join(ids2,&ps2);
	int sumatotal=((estructuras*)ps1)->sumaparcial1+((estructuras*)ps2)->sumaparcial2;
	return sumatotal;
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
