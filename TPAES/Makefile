CC=gcc
CFLAGS= -Wall
OFLAGS= -O3
GLIB_FLAGS=`pkg-config --libs --cflags glib-2.0`


all: bin/ecb-encrypt-ref bin/ecb-encrypt-aesni bin/test-ecb-encrypt

bin/ecb-encrypt-ref: ecb-encrypt.c rijndael-alg-fst.c util.c aes-ecb.c | bin
	$(CC) $(CFLAGS) $(OFLAGS) -DUSE_AESREF $^ -o $@

bin/ecb-encrypt-aesni: ecb-encrypt.c aesni-128.c util.c aes-ecb.c | bin
	$(CC) -maes $(CFLAGS) $(OFLAGS) $^ -o $@

bin/test-ecb-encrypt: test-ecb-encrypt.c aesni-128.c aes-ecb.c | bin
	$(CC) -maes $(CFLAGS) $(OFLAGS) $^ $(GLIB_FLAGS) -o $@

bin:
	mkdir -p bin

