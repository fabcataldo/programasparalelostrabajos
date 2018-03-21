all: folders
	gcc -fopenmp test_find.c tp3.c -Wall -o bin/test_vectors
	gcc -fopenmp test_sum.c tp3.c -Wall -lm -o bin/test_sum	
	gcc -fopenmp time_find.c tp3.c -Wall -o bin/time_find
	gcc -fopenmp time_sum.c tp3.c -Wall -lm -o bin/time_sum
folders:
	mkdir bin
