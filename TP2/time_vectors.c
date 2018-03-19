#include "vectors.h"
#include<time.h>
#include<stdio.h>
#include<stdlib.h>

int main(int argc, char **argv){
	struct timespec start, finish; 
	long secs,secs2;
	int i,el,i_el_finded;
	long suma,ns,ns2;
	int n=800000;
	FILE *fp = fopen(argv[1], "w+");

	//printf("\nIngrese elemento del vector a buscar (el vector va a ser de la forma 1,2,3... tamaño n): ");
	//scanf("%d",&el);

	fprintf(fp,"tamaño del vector  elemento a buscar  indice del elemento encontrado   suma total del vector   tiempo que tardo find()     tiempo que tardo sum()");
	
	//i=4;
	for(i=100000;i<=n;i+=100000){
		el=i/2;
		clock_gettime(CLOCK_REALTIME, &start); 
		i_el_finded=find(el, NULL, i);
		clock_gettime(CLOCK_REALTIME, &finish); 
		secs = finish.tv_sec - start.tv_sec;
		ns = finish.tv_nsec - start.tv_nsec; 
	
		if (start.tv_nsec > finish.tv_nsec) { // clock underflow 
			--secs; 
		}	
		
		clock_gettime(CLOCK_REALTIME, &start); 	
		suma=sum(NULL,i);
		clock_gettime(CLOCK_REALTIME, &finish); 
		secs2 = finish.tv_sec - start.tv_sec;
		ns2 = finish.tv_nsec - start.tv_nsec;

		if (start.tv_nsec > finish.tv_nsec) { // clock underflow 
			--secs2; 
		}	
		fprintf(fp, "\n\t%d\t\t%d\t\t\t%d\t\t\t%ld\t\t\t%e seg\t%e seg", i, el,i_el_finded,suma,(double)secs + (double)ns/(double)1000000000, (double)secs2 + (double)ns2/(double)1000000000);
	}
	fclose(fp);
	return 0;
}
