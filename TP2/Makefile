all: folders
	gcc test_vectors.c vectors.c vectors.h -Wall -o bin/test_vectors
	gcc time_vectors.c vectors.c vectors.h -Wall -o bin/time_vectors	
	gcc test_vectors.c vectors_pthreads.c vectors.h -Wall -o bin/test_vectors_pthreads -pthread -lm
	gcc time_vectors.c vectors_pthreads.c vectors.h -Wall -o bin/time_vectors_pthreads -pthread -lm	
folders:
	mkdir bin
