#include <stdio.h>
#include <stdlib.h>
#include "matmul.h"

void matmul_seq(double* a, double* b, double* c, int nra, int nca, int ncb){
	int i,j,k;
	for(i=0;i<nra;i++)
		for(j=0;j<ncb;j++)
			c[i*nra+j]=0;							

	for(i=0;i<nra;i++){
		for(j=0;j<ncb;j++){
			for(k=0;k<nca;k++){
				c[i*nra+j]+=a[i*nca+k]*b[k*ncb+j];
			}
		}	
	}
}

void matmul_o(double* a, double* b, double* c, int nra, int nca, int ncb){
	int i,j,k;
	for(i=0;i<nra;i++)
		for(j=0;j<ncb;j++)
			c[i*nra+j]=0;							

	#pragma omp parallel for
	for(i=0;i<nra;i++){
		for(j=0;j<ncb;j++){
			for(k=0;k<nca;k++){
				c[i*nra+j]+=a[i*nca+k]*b[k*ncb+j];
			}
		}	
	}
}

void matmul_i(double* a, double* b, double* c, int nra, int nca, int ncb){
	int i,j,k;
	for(i=0;i<nra;i++)
		for(j=0;j<ncb;j++)
			c[i*nra+j]=0;							

	for(i=0;i<nra;i++){
		for(j=0;j<ncb;j++){
			#pragma omp parallel for
			for(k=0;k<nca;k++){
				c[i*nra+j]+=a[i*nca+k]*b[k*ncb+j];
			}
		}	
	}
}

void cargarmatriz(double* matriz, int nrm, int ncm){ //nrm -> numero de filas de la matriz m, ncm -> num de col de la matriz m
	int i,j;
	
	for(i=0;i<nrm;i++){
		for(j=0;j<ncm;j++){
			matriz[i*nrm+j]=rand();
			//printf("\nmatriz[%d][%d]: \n",i,j);
			//scanf("%lf",&matriz[i*ncm+j]);
		}
	}
}

void mostrarmatriz(double* matriz, int nrm, int ncm){
	int i,j;
	for(i=0;i<nrm;i++){
		for(j=0;j<ncm;j++){
			printf("\nmatriz[%d][%d]: %.3lf \n",i,j,matriz[i*ncm+j]);		
		}
	}
}

void cargar_matriz_random(double* matriz, int nrm, int ncm){
	int i,j;
	
	for(i=0;i<nrm;i++){
		for(j=0;j<ncm;j++){
			matriz[i*ncm+j]=rand();
		}
	}
}
	
