#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>

#define PI 3.14159265

float f(float x) {
	return sin(x);
}

int main(int argc, char* argv[]){
	int rank, p, src, dst, tag=0, dstmain;
	float integral=0; //inicializo el lugar a donde va a el resultado de cada integral de cada proceso	
	double start1,end1,t1;
	MPI_Status status;
	MPI_Request *request;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	float x;
	int i,n;
	int local_n;
	float local_a,local_b,h, a, b;	
	
	if(rank==0){//rango del main
		
		printf("Ingrese a: \n");
		scanf("%f", &a);
		printf("Ingrese b: \n");
		scanf("%f", &b);
		printf("Ingrese n (si no escribe n, se va a tomar por default 1024: \n");
		scanf("%d", &n);
		if(n==0){
			n=1024;	
		}
		a=a*PI;
		b=b*PI;
		h=(b-a)/n;

		local_n=n/p;
		start1 = MPI_Wtime(); 
		for(src=1;src<p;src++){
			dst=src;
			MPI_Send(&integral, 1, MPI_FLOAT, dst, tag, MPI_COMM_WORLD);
			MPI_Recv(&integral, 1, MPI_FLOAT, src, tag, MPI_COMM_WORLD, &status);
			integral+=integral*h;
		}
		end1 = MPI_Wtime();
		t1=end1-start1;
		printf("Valor aproximado de la integral: %f\n", integral);
		printf("Tiempo transcurrido: %lf\n", t1);
	}
	else{  //rangos de los otros procesos 		
		dstmain=0;
		dst=0;
		MPI_Recv(&integral, 1, MPI_FLOAT, dstmain, tag, MPI_COMM_WORLD, &status);

		local_a=a+(rank*local_n*h);	
		local_b=local_a+(local_n*h);
		x = local_a;
		
		integral = (f(local_a) + f(local_b))/2.0;
		
		for(i=1;i<=local_n-1;i++){
			x = x + h;
			integral = integral + f(x);
		}		
		MPI_Send(&integral, 1, MPI_FLOAT, dst, tag, MPI_COMM_WORLD);
	}	

	MPI_Finalize();
	return 0;
}
