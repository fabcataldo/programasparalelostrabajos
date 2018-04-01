#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

float f(float x) {
	float return_val;
	return_val = x*x;
	return return_val;
}

int main(int argc, char* argv[]){
	int rank, p, src, dst, tag=0, dstmain;
	float integral; //inicializo el lugar a donde va a el resultado de cada integral de cada proceso	
	double start1,end1,t1;
	MPI_Status status;
	MPI_Request *request;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	float x;
	int i;
	int local_n;
	float local_a,local_b,h;
	h=(b-a)/n;	
	
	if(rank!=0){  //rangos de los otros procesos 	
		for (src = 1; src < p; src++ ) {	
			dstmain=0;
			dst=0;
			MPI_Irecv(&integral, 1, MPI_FLOAT, dstmain, tag, MPI_COMM_WORLD, &status);
			local_n=n/p;
			local_a=a+(rank*local_n*h);	
			local_b=local_a+(local_n*h);
			integral = (f(local_a) + f(local_b))/2.0;
			x = local_a;
			h = (local_b-local_a)/local_n;

			for(i=1;i<=local_n-1;i++){
				x = x + h;
				integral = integral + f(x);
			}

			printf("Proceso: %d\n", rank);
			MPI_Isend(&integral, 1, MPI_FLOAT, dst, tag, MPI_COMM_WORLD, &request);
		}
	}	
	else{//rango del main
		float a, b;
		int n;

		printf("Ingrese a: \n");
		scanf("%f", &a);
		printf("Ingrese b: \n");
		scanf("%f", &b);
		printf("Ingrese n (si no escribe n, se va a tomar por default 1024: \n");
		scanf("%d", &n);
		if(n==0){
			n=1024;	
		}

		start1 = MPI_Wtime(); 
		dst=1; //el numero del rango del primer proceso, por lo menos, es 1, los demás son >1
		src=1;//el numero del rango del primer proceso, por lo menos, es 1, los demás son >1

		MPI_Isend(&integral, 1, MPI_FLOAT, dst, tag, MPI_COMM_WORLD, &request);
		MPI_Irecv(&integral, 1, MPI_FLOAT, src, tag, MPI_COMM_WORLD, &status);
		integral += integral*h;
		end1 = MPI_Wtime();
		t1=end1-start1;
		
		printf("Integral de sin(x): %f\n", integral);
		printf("Tiempo transcurrido: %lf\n", t1);		
	}

	MPI_Finalize();
	return 0;
}
