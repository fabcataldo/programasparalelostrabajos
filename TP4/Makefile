PROG=test_matmul time_matmul
CC=gcc
GLIB_FLAGS=`pkg-config --libs --cflags glib-2.0`
CFLAGS= -O3 -fopenmp -lm

all: $(addprefix bin/, $(PROG))

bin/test_matmul: matmul.c test_matmul.c | bin
	$(CC) $^ $(CFLAGS) $(GLIB_FLAGS) -o $@ 

bin/time_matmul: matmul.c time_matmul.c | bin
	$(CC) $^ $(CFLAGS) -o $@

bin:
	mkdir -p bin

test: bin/test_matmul
	bin/test_matmul
