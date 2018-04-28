#include <stdio.h>
#include <stdlib.h>

void vervector(int *vector, int size){
	int i;
	for(i=0;i<size;i++){
		printf("\n%d",vector[i]);
	}
}

void cargarvector(int *vector, int size){
	int i;
	for(i=0;i<size;i++){
		vector[i]=0;
	}
	vector[(size-1)/2]=1;
}

void generararchivo(int* vec, int size) {
	FILE *ptrfile=fopen("outputsec.dat","w+");
	int i;
	for(i=0;i<size;i++){
		if(vec[i]==1){
			fprintf(ptrfile,"%d ",i);		
		}
	}
	fclose(ptrfile);
}

void calculoiterativo(int *vector, int size, int valormenosuno, int valormasuno){
	int i,aux;

	int* vecviejo=(int*)malloc(size*sizeof(int));
	for(i=0;i<size;i++){
		vecviejo[i]=vector[i];
	}
	aux=vector[1];

	if(((valormenosuno+vector[0])+(valormenosuno*vector[0])+(valormenosuno*vector[0]*aux))%2 == 0) 
		vector[0]=0;
	else
		vector[0]=1;

	for(i=1;i<=size;i++){
		valormenosuno=vecviejo[i-1];
		aux=vecviejo[i+1]; 

		if(((valormenosuno+vector[i])+(valormenosuno*vector[i])+(valormenosuno*vector[i]*aux))%2 == 0) 
			vector[i]=0;
		else
			vector[i]=1;
	}	

	valormenosuno=vecviejo[size-2];
	aux=valormasuno;
	if(((valormenosuno+vector[size-1])+(valormenosuno*vector[size-1])+(valormenosuno*vector[size-1]*aux))%2 == 0) 
		vector[size-1]=0;
	else
		vector[size-1]=1;

	free(vecviejo);
}

int main(int argc, char *argv[]){
	if(argv[1]==0){ //tamaño del estado
		fprintf(stderr, "Argumentos inválidos\n");
		return 1;		
	}
	else{
		if(argv[2]==0){ //cant de iteraciones o generaciones
			fprintf(stderr, "Argumentos inválidos\n");
			return 1;
		}	
	}

	int size=atoi(argv[1]);
	int cantiter=atoi(argv[2]);
	int j=1;
	int *vector = (int*)malloc(size*sizeof(int));

	int valorbordes=0;
	cargarvector(vector, size);	

	int aux,aux2;

	while(j<=cantiter){
		calculoiterativo(vector, size, valorbordes, valorbordes);
		j++;
	}
	
	generararchivo(vector, size);

	free(vector);
	printf("Ejecución realizada con un estado de tamaño %d y con %d generaciones utilizadas.",size, cantiter);
	return 0;
}
