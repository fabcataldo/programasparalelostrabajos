#include<stdio.h>
#include<stdlib.h>
#include<stdio.h>

int find(int value, int *v, int size);

void visualizarvector(int *vector, int size){
	int i;
	for(i=0;i<size;i++){
		printf("\n%d",vector[i]);
	}
}

void cargarvector(int *vector, int size){
	int i;
	for(i=0;i<=size-1;i++){
		vector[i]=i;
	}
}

void cargarvector2(int *vector, int size){
	int i;
	for(i=0;i<=size-1;i++){
		vector[i]=((size-1)-i)*2;
	}
}

void cargarvector3(int *vector, int size){
	int i;
	for(i=0;i<size;i++){
		vector[i]=((i*427345)+389421)%size;
	}
}



int main(int argc, char **argv){
	int el,i_el_finded, i_el_finded_2, i_el_finded_3;
	int size=16777216;
	int* vector1=(int*)malloc(size*sizeof(int));
	cargarvector(vector1,size);
	int* vector2=(int*)malloc(size*sizeof(int));
	cargarvector2(vector2, size);
	int* vector3=(int*)malloc(size*sizeof(int));
	cargarvector3(vector3,size);

	printf("\nBÚSQUEDA PARALELIZADA");
	printf("\nIngrese elemento del vector a buscar: ");
	scanf("%d",&el);

	printf("\nLlamo a find().");
	i_el_finded=find(el, vector1, size);
	i_el_finded_2=find(el, vector2, size);
	i_el_finded_3=find(el, vector3, size);
	printf("\nelemento a buscar  indice del elemento encontrado con el vector1  indice del elemento encontrado con el vector2   indice del elemento encontrado con el vector3\n");
	printf("\n\t%d\t\t\t%d\t\t%d\t\t%d\n", el, i_el_finded, i_el_finded_2, i_el_finded_3);
	
	free(vector1);
	free(vector2);
	free(vector3);
	return 0;
}
