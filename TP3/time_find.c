#include<stdlib.h>
#include<stdio.h>
#include<omp.h>

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

void test_1(int* vector1, int* vector2, int* vector3 , int size){
	omp_set_num_threads(2);	
	
	int elemento = 10000;
	double start1,start2,start3,end1,end2,end3,t1,t2,t3;
	int i_el_finded, i_el_finded_2, i_el_finded_3;	

	start1 = omp_get_wtime();
	i_el_finded=find(elemento, vector1, size);
	end1 = omp_get_wtime();

	start2 = omp_get_wtime();
	i_el_finded_2=find(elemento, vector2, size);
	end2 = omp_get_wtime();

	start3 = omp_get_wtime();
	i_el_finded_3=find(elemento, vector3, size);
	end3 = omp_get_wtime();
	t1=end1-start1;
	t2=end2-start2;
	t3=end3-start3;
	printf("\nelemento a buscar: %d",elemento); 
	printf("\nindice del elemento encontrado con el vector1: %d",i_el_finded);
	printf("\nindice del elemento encontrado con el vector2: %d",i_el_finded_2);
	printf("\nindice del elemento encontrado con el vector3: %d", i_el_finded_3);
	printf("\ntiempo que tardó find() en el vector1: %lf", t1);
	printf("\ntiempo que tardó find() en el vector2: %lf", t2);
	printf("\ntiempo que tardó find() en el vector3: %lf\n", t3);
	printf("----------------------------------------------------------------\n");
}

void test_2(int* vector1, int* vector2, int* vector3 , int size){
	omp_set_num_threads(3);	
	
	int elemento = size/2;
	double start1,start2,start3,end1,end2,end3,t1,t2,t3;
	int i_el_finded, i_el_finded_2, i_el_finded_3;	

	start1 = omp_get_wtime();
	i_el_finded=find(elemento, vector1, size);
	end1 = omp_get_wtime();

	start2 = omp_get_wtime();
	i_el_finded_2=find(elemento, vector2, size);
	end2 = omp_get_wtime();

	start3 = omp_get_wtime();
	i_el_finded_3=find(elemento, vector3, size);
	end3 = omp_get_wtime();
	t1=end1-start1;
	t2=end2-start2;
	t3=end3-start3;
	printf("\nelemento a buscar: %d",elemento); 
	printf("\nindice del elemento encontrado con el vector1: %d",i_el_finded);
	printf("\nindice del elemento encontrado con el vector2: %d",i_el_finded_2);
	printf("\nindice del elemento encontrado con el vector3: %d", i_el_finded_3);
	printf("\ntiempo que tardó find() en el vector1: %lf", t1);
	printf("\ntiempo que tardó find() en el vector2: %lf", t2);
	printf("\ntiempo que tardó find() en el vector3: %lf\n", t3);
	printf("\n----------------------------------------------------------------\n");
}

void test_3(int* vector1, int* vector2, int* vector3 , int size){
	omp_set_num_threads(4);	
	
	int elemento = size+1;
	int elemento3 = vector3[0]-1;

	double start1,start2,start3,end1,end2,end3,t1,t2,t3;
	int i_el_finded, i_el_finded_2, i_el_finded_3;	

	start1 = omp_get_wtime();
	i_el_finded=find(elemento, vector1, size);
	end1 = omp_get_wtime();

	start2 = omp_get_wtime();
	i_el_finded_2=find(elemento, vector2, size);
	end2 = omp_get_wtime();

	start3 = omp_get_wtime();
	i_el_finded_3=find(elemento3, vector3, size);
	end3 = omp_get_wtime();
	t1=end1-start1;
	t2=end2-start2;
	t3=end3-start3;
	printf("\nelemento a buscar en vector1 y vector2: %d",elemento); 
	printf("\nelemento a buscar en vector3: %d",elemento3); 
	printf("\nindice del elemento encontrado con el vector1: %d",i_el_finded);
	printf("\nindice del elemento encontrado con el vector2: %d",i_el_finded_2);
	printf("\nindice del elemento encontrado con el vector3: %d", i_el_finded_3);
	printf("\ntiempo que tardó find() en el vector1: %lf", t1);
	printf("\ntiempo que tardó find() en el vector2: %lf", t2);
	printf("\ntiempo que tardó find() en el vector3: %lf\n", t3);
	printf("\n----------------------------------------------------------------\n");
}


int main(int argc, char **argv){
	//int nProcessors = omp_get_max_threads();
	//printf("PROCESOrSSS: %d", nProcessors);
	
	int size=16777216;
	int* vector1=(int*)malloc(size*sizeof(int));
	cargarvector(vector1,size);
	int* vector2=(int*)malloc(size*sizeof(int));
	cargarvector2(vector2, size);
	int* vector3=(int*)malloc(size*sizeof(int));
	cargarvector3(vector3,size);
	
	printf("\nBÚSQUEDA PARALELIZADA\n");

	printf("\nLlamo a test_1().");
	test_1(vector1, vector2, vector3, size);

	printf("\nLlamo a test_2().");
	test_2(vector1, vector2, vector3, size);
	
	printf("\nLlamo a test_3().");
	test_3(vector1, vector2, vector3, size);

	free(vector1);
	free(vector2);
	free(vector3);
	return 0;
}
