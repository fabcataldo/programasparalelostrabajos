all: folders
	gcc test_vectors.c -Wall -O3 -o bin/test_vectors
	gcc time_vectors.c -Wall -O3 -o bin/time_vectors	
	gcc test_vectors_pthreads.c -Wall -O3 -o bin/test_vectors_pthreads -pthread
	gcc time_vectors_pthreads.c -Wall -O3 -o bin/time_vectors_pthreads -pthread
folders:
	mkdir bin
