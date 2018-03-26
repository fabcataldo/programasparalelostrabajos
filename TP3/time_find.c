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

void tester(int* vector1, int* vector2, int* vector3, int elementos[], int num_hilos, int size){
	omp_set_num_threads(num_hilos);	
	
	double start1,start2,start3,end1,end2,end3,t1,t2,t3;
	int i_el_finded, i_el_finded_2, i_el_finded_3;	

	start1 = omp_get_wtime();
	i_el_finded=find(elementos[0], vector1, size);
	end1 = omp_get_wtime();

	start2 = omp_get_wtime();
	i_el_finded_2=find(elementos[1], vector2, size);
	end2 = omp_get_wtime();

	start3 = omp_get_wtime();
	i_el_finded_3=find(elementos[2], vector3, size);
	end3 = omp_get_wtime();
	t1=end1-start1;
	t2=end2-start2;
	t3=end3-start3;
	printf("\nelemento a buscar en vector1: %d",elementos[0]); 
	printf("\nelemento a buscar en vector2: %d",elementos[1]);
	printf("\nelemento a buscar en vector3: %d",elementos[2]);
	printf("\nindice del elemento encontrado con el vector1: %d",i_el_finded);
	printf("\nindice del elemento encontrado con el vector2: %d",i_el_finded_2);
	printf("\nindice del elemento encontrado con el vector3: %d", i_el_finded_3);
	printf("\ntiempo que tardó find() en el vector1: %lf", t1);
	printf("\ntiempo que tardó find() en el vector2: %lf", t2);
	printf("\ntiempo que tardó find() en el vector3: %lf\n", t3);
	printf("----------------------------------------------------------------\n");
}

int main(int argc, char **argv){
	//int nProcessors = omp_get_max_threads();
	//printf("PROCESOrSSS: %d", nProcessors);
	
	int size=16777216;
	int num_hilos;
	int* vector1=(int*)malloc(size*sizeof(int));
	cargarvector(vector1,size);
	int* vector2=(int*)malloc(size*sizeof(int));
	cargarvector2(vector2, size);
	int* vector3=(int*)malloc(size*sizeof(int));
	cargarvector3(vector3,size);
	
	printf("\nBÚSQUEDA PARALELIZADA\n");
	int elementos_a_buscar[3];

	//vector1[size/2], //vector2[size/2], //vector3[size/2]
	elementos_a_buscar[0]=8388608;
	elementos_a_buscar[1]=16777214;
	elementos_a_buscar[2]=-7999187;
	num_hilos = 3;
	printf("\nLlamo al tester() para buscar elementos en la mitad de los 3 vectores");
	tester(vector1, vector2, vector3, elementos_a_buscar, num_hilos, size);

	elementos_a_buscar[0]=vector1[size-1];
	elementos_a_buscar[1]=vector2[size-2];
	elementos_a_buscar[2]=vector3[size-1];
	num_hilos = 3;
	printf("\nLlamo al tester() para buscar el último elemento por cada vector ");
	tester(vector1, vector2, vector3, elementos_a_buscar, num_hilos, size);
	
	//elementos que no estan en los vectores
	elementos_a_buscar[0]=size+1;
	elementos_a_buscar[1]=size+1;
	elementos_a_buscar[2]=vector3[0]-1;
	num_hilos = 4;
	printf("\nLlamo al tester() para buscar elementos que no están en los 3 vectores");
	tester(vector1, vector2, vector3, elementos_a_buscar, num_hilos, size);

	//elementos que no estan en los vectores
	elementos_a_buscar[0]=size+1;
	elementos_a_buscar[1]=size+1;
	elementos_a_buscar[2]=vector3[0]-1;
	num_hilos = 8;
	printf("\nLlamo al tester() para buscar elementos que no están en los 3 vectores CON 8 HILOS");
	tester(vector1, vector2, vector3, elementos_a_buscar, num_hilos, size);

	free(vector1);
	free(vector2);
	free(vector3);
	return 0;
}
