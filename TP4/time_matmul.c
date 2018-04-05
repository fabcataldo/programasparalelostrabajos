#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
#include "matmul.h"

int main(int argc, char **argv){
	int i;	
	double start1,start2,start3,end1,end2,end3,t1,t2,t3;
	double* matriza=NULL;
	double* matrizb=NULL;
	double* matrizc=NULL;
	//FILE *fp = fopen(argv[1], "w+");
	
	
	printf("\nMatrices Cuadradas\n");
	for(i=100;i<=1000;i+=100){
		matriza=(double*)malloc(i*i*sizeof(double));
		cargar_matriz_random(matriza, i, i);
		matrizb=(double*)malloc(i*i*sizeof(double)); 
		cargar_matriz_random(matrizb, i, i);	
		matrizc=(double*)malloc(i*i*sizeof(double));

		start1 = omp_get_wtime();
		matmul_seq(matriza, matrizb, matrizc, i, i, i);
		end1 = omp_get_wtime();

		start2 = omp_get_wtime();
		matmul_o(matriza, matrizb, matrizc, i, i, i);
		end2 = omp_get_wtime();

		start3 = omp_get_wtime();
		matmul_i(matriza, matrizb, matrizc, i, i, i);	
		end3 = omp_get_wtime();

		free(matriza);
		free(matrizb);
		free(matrizc);

		t1=end1-start1;
		t2=end2-start2;
		t3=end3-start3;
		printf("n   t de matmul_seq   t de matmul_o   t de matmul_i\n");
		printf("%d   %lf        %lf           %lf\n",i,t1,t2,t3);
		
	}
	
	/*
	printf("\n----------------------------------------------------------------");
	printf("\nMatrices rectangulares\n");
	for(i=1000;i<=10000;i+=1000){
		matriza=(double*)malloc(100*i*sizeof(double));
		cargar_matriz_random(matriza, 100, i);
		matrizb=(double*)malloc(i*100*sizeof(double)); 
		cargar_matriz_random(matrizb, i, 100);	
		matrizc=(double*)malloc(i*i*sizeof(double));

		start1 = omp_get_wtime();
		matmul_seq(matriza, matrizb, matrizc, 100, i, 100);
		end1 = omp_get_wtime();

		start2 = omp_get_wtime();
		matmul_o(matriza, matrizb, matrizc, 100, i, 100);
		end2 = omp_get_wtime();

		start3 = omp_get_wtime();
		matmul_i(matriza, matrizb, matrizc, 100, i, 100);	
		end3 = omp_get_wtime();

		free(matriza);
		free(matrizb);
		free(matrizc);

		t1=end1-start1;
		t2=end2-start2;
		t3=end3-start3;
		printf("nra   nca   ncb     t de matmul_seq   t de matmul_o   t de matmul_i\n");
		printf("100    %d   100          %lf        %lf           %lf\n",i,t1,t2,t3);

	}


	printf("\nnra     nca   ncb     t de matmul_seq   t de matmul_o   t de matmul_i\n");
	for(i=1000;i<=10000;i+=1000){
		matriza=(double*)malloc(i*100*sizeof(double));
		cargar_matriz_random(matriza, i, 100);
		matrizb=(double*)malloc(100*i*sizeof(double)); 
		cargar_matriz_random(matrizb, 100, i);	
		matrizc=(double*)malloc(i*i*sizeof(double));

		start1 = omp_get_wtime();
		matmul_seq(matriza, matrizb, matrizc, i, 100, i);
		end1 = omp_get_wtime();

		start2 = omp_get_wtime();
		matmul_o(matriza, matrizb, matrizc, i, 100, i);
		end2 = omp_get_wtime();

		start3 = omp_get_wtime();
		matmul_i(matriza, matrizb, matrizc, i, 100, i);	
		end3 = omp_get_wtime();

		free(matriza);
		free(matrizb);
		free(matrizc);

		t1=end1-start1;
		t2=end2-start2;
		t3=end3-start3;
	
		printf("%d    100   %d          %lf        %lf           %lf\n",i,i,t1,t2,t3);
	}
	*/
	//fprintf(fp, "n   matmul   matmul_5   matmul_10   matmul_t   matmul_t_5   matmul_t_10\n");
	//fclose(fp);

	return 0;
}
