#include<stdlib.h>
#include<stdio.h>
#include<omp.h>

int find(int value, int *v, int size);

void cargarvector(int *vector, int size){
	int i;
	for(i=0;i<=size-1;i++){
		vector[i]=i;
	}
}

void cargarvector2(int *vector, int size){
	int i;
	for(i=size-1;i<=0;i=(i-1)*2){
		vector[i]=i;
	}
}

void cargarvector3(int *vector, int size){
	int i;
	for(i=0;i<size;i++){
		vector[i]=((i*427345)+389421)%size;
	}
}

void visualizarvector(int *vector, int size){
	int i;
	for(i=0;i<size;i++){
		printf("\n%d",vector[i]);
	}
}

int main(int argc, char **argv){
	int el,i_el_finded, i_el_finded_2, i_el_finded_3;
	double start1,start2,start3,end1,end2,end3,t1,t2,t3;
	int size=16777216;
	int* vector1=(int*)malloc(size*sizeof(int));
	cargarvector(vector1,size);
	int* vector2=(int*)malloc(size*sizeof(int));
	cargarvector2(vector2,size);
	int* vector3=(int*)malloc(size*sizeof(int));
	cargarvector3(vector3,size);

	printf("\nBÚSQUEDA PARALELIZADA");
	el = 10000;

	printf("\nLlamo a find().");

	start1 = omp_get_wtime();
	i_el_finded=find(el, vector1, size);
	end1 = omp_get_wtime();
	start2 = omp_get_wtime();
	i_el_finded_2=find(el, vector2, size);
	end2 = omp_get_wtime();
	start3 = omp_get_wtime();
	i_el_finded_3=find(el, vector3, size);
	end3 = omp_get_wtime();
	t1=end1-start1;
	t2=end2-start2;
	t3=end3-start3;
	printf("\nelemento a buscar: %d",el); 
	printf("\nindice del elemento encontrado con el vector1: %d",i_el_finded);
	printf("\nindice del elemento encontrado con el vector2: %d",i_el_finded_2);
	printf("\nindice del elemento encontrado con el vector3: %d", i_el_finded_3);
	printf("\ntiempo que tardó find() en el vector1: %lf", t1);
	printf("\ntiempo que tardó find() en el vector2: %lf", t2);
	printf("\ntiempo que tardó find() en el vector3: %lf\n", t3);

	return 0;
}
