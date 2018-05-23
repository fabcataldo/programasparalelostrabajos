#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "util.h"
#include "aes-128.h"
#include "aes-ecb.h"
#include <time.h>

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#endif

#ifdef USE_AESREF
#include "rijndael-alg-fst.h"
#define AES_128_encrypt(in,out,key) rijndaelEncrypt((u32*)key, ROUNDS, in, out)
#define AES_128_set_key(userkey,key)  rijndaelKeySetupEnc((u32*)key, userkey, 128)
#endif


void
do_tests ()
{
  char *test_vector[] = { "6bc1bee22e409f96e93d7e117393172a",
    "00112233445566778899aabbccddeeff", "\0"
  };
  char *keystring[] = { "2b7e151628aed2a6abf7158809cf4f3c",
    "000102030405060708090a0b0c0d0e0f"
  };
  char *okresult[] = { "3ad77bb40d7a3660a89ecaf32466ef97",
    "69c4e0d86a7b0430d8cdb78070b4c55a"
  };
  char result[2*BLOCK_SIZE+1];
  unsigned char userkey[KEY_SIZE];
  unsigned char expanded_key[(ROUNDS+1)*BLOCK_SIZE];
  unsigned char in[BLOCK_SIZE], out[BLOCK_SIZE], ok[BLOCK_SIZE];
  unsigned errors = 0;


  int i;
  printf("ECB Test Vectors\n");
  for (i = 0; test_vector[i][0]; i++)
    {
      if (str2bytes (keystring[i], userkey, BLOCK_SIZE) != BLOCK_SIZE)
	{
	  fprintf (stderr, "Longitud de clave incorrecta\n");
	  exit (EXIT_FAILURE);
	}
      if (str2bytes (test_vector[i], in, BLOCK_SIZE)
	  != BLOCK_SIZE)
	{
	  fprintf (stderr, "Longitud de bloque incorrecta\n");
	  exit (EXIT_FAILURE);
	}
      if (str2bytes (okresult[i], ok, BLOCK_SIZE)
	  != BLOCK_SIZE)
	{
	  fprintf (stderr, "Vector de comprobación incorrecto\n");
	  exit (EXIT_FAILURE);
	}
      AES_128_set_key(userkey,expanded_key);
      AES_128_encrypt(in,out,expanded_key);
	      bytes2hex (out, result, BLOCK_SIZE);
      if (memcmp(out,ok,BLOCK_SIZE) == 0) 
	printf ("%s -> %s: OK\n", keystring[i], result);
      else {
	errors++;
	      printf("%s -> %s: MAL (debería ser %s)\n",keystring[i],result,okresult[i]);
      }
    }
  exit(errors?EXIT_FAILURE:EXIT_SUCCESS);
}


void
usage (char *program_name)
{
  char *msg =
    "Uso: %s [-h] [-t] [-k <clave>] [-i <filename>] [-o <filename>]\n"
    "-h: Presenta esta ayuda\n"
    "-t: Ejecuta aes con los vectores de prueba\n"
    "-k <clave>: Especifica la clave de cifrado (32 dígitos hexadecimales)\n"
    "-i <filename>: Nombre del archivo de entrada\n"
    "-o <filename>: Nombre del archivo de salida\n";
  fprintf (stderr, msg, program_name);
}

void create_buffer(unsigned char* c, size_t len){
    for(int i = 0; i < len; i++)
        c[i] = lrand48();
}


int
main (int argc, char **argv)
{
  unsigned char userkey[KEY_SIZE];
  int opt, keylen = 0;
  FILE* in = stdin;
  FILE* out = stdout;
  int bytes_input=0;

  while ((opt = getopt (argc, argv, "bhtk:i:o")) != -1)
    {
      switch (opt)
	{
	case 'b':
	   bytes_input=1;
	   break;
	case 'h':
	  usage (argv[0]);
	  exit(EXIT_SUCCESS);
	case 't':
	      do_tests ();
	      break;
	case 'k':
	  keylen = str2bytes(optarg,userkey,KEY_SIZE);
	  break;
	case 'i':
	  in = fopen(optarg,"rb");
	  if (in == NULL) {
	    fprintf(stderr,"'%s': %s\n",optarg, strerror(errno));
	    exit(EXIT_FAILURE);
	  }
	  break;
	case 'o':
	  out = fopen(optarg,"wb");
	  if (out == NULL) {
	    fprintf(stderr,"'%s': %s\n",optarg, strerror(errno));
	    exit(EXIT_FAILURE);
	  }
	  break;

	   
	default:		/* '?' */
	  
	  usage (argv[0]);
	  exit (EXIT_FAILURE);
	}
    }
    	  if (keylen != KEY_SIZE){
	    fprintf(stderr,"Debe especificarse una clave con 32 dígitos hexadecimales\n");
	    usage(argv[0]);
	    exit(EXIT_FAILURE);
	  }
	  
	if(bytes_input!=0){
          //int blocks=128; //2mb 
	  int blocks=120;
     	  unsigned char* inblocks = calloc(sizeof(unsigned char),blocks*BLOCK_SIZE);
	  create_buffer(inblocks, blocks*BLOCK_SIZE);
          unsigned char* outblocks = calloc(sizeof(unsigned char),blocks*BLOCK_SIZE);
          size_t length_inb=blocks*BLOCK_SIZE;
          unsigned long long length_outb=0;

	  double stop=0;
	  double stop_2=0;
          double veloc_sec=0;
          double veloc_mult=0;

	  clock_t start= clock();
	  AES_128_ecb_encrypt(inblocks,outblocks, length_inb, &length_outb, userkey);
	  stop=((double)clock() - start) / CLOCKS_PER_SEC;
          
	  clock_t start_2= clock();
          AES_128_ecb_encrypt_multiples(inblocks,outblocks, length_inb, &length_outb, userkey);
	  stop_2=((double)clock() - start_2) / CLOCKS_PER_SEC;

          stop=((double)clock() - start) / CLOCKS_PER_SEC;

	  bytes_input=blocks*BLOCK_SIZE;
          
	  printf("Cifrados %d bytes en %lf seg., utilizando cifrado secuencial\n", bytes_input ,stop);
	  printf("Cifrados %d bytes en %lf seg., utilizando cifrado paralelo\n", bytes_input ,stop_2);

//64 bloques de 16 bytes cada uno=1024 bytes, 1mb. Si 1mb tarda 0.000088 (aprox), bytes_input, x, asi saco la velocidad sec
 	  veloc_sec=bytes_input*0.000088/1;
//64 bloques de 16 bytes cada uno=1024 bytes, 1mb. Si 1mb tarda 0.000025 (aprox), bytes_input, x, asi saco la velocidad pero a nivel de instrucc
 	  veloc_mult=bytes_input*0.000025/1; 

          printf("Velocidad del cifrado secuencial: %lf MB/seg.\n", veloc_sec);
          printf("Velocidad del cifrado paralelo (a nivel de instrucciones): %lf MB/seg.\n", veloc_mult);
	  
	  //AES_128_ecb_encrypt_file(in,out,userkey);
          //fseek(in, 0L, SEEK_END);
	  //bytes_input=ftell(in);
	  //printf("El archivo ingresado ocupa %d bytes\n", bytes_file);         
	  
	}
	else{
		AES_128_ecb_encrypt_file(in,out,userkey);
	}
  if(in != stdin) fclose(in);
  if(out != stdout) fclose(out);

  exit (EXIT_SUCCESS);
}
