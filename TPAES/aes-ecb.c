#include "aes-128.h"
#include "aes-ecb.h"
#include<stdlib.h>
#include <omp.h>

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#endif

#ifdef USE_AESREF
#include "rijndael-alg-fst.h"
#define AES_128_encrypt(in,out,key) rijndaelEncrypt((u32*)key, ROUNDS, in, out)
#define AES_128_set_key(userkey,key)  rijndaelKeySetupEnc((u32*)key, userkey, 128)
#endif




void AES_128_ecb_encrypt_file(FILE* in, FILE* out, const unsigned char* userkey){
  unsigned char inblock[BLOCK_SIZE];
  unsigned char outblock[BLOCK_SIZE];
  unsigned char key[(ROUNDS+1)*BLOCK_SIZE];
  unsigned int bytes_read, padding_value, i;
  AES_128_set_key(userkey,key);
  bytes_read = fread(inblock,1,BLOCK_SIZE,in);
  while (bytes_read == BLOCK_SIZE){
    AES_128_encrypt(inblock,outblock,key);
    fwrite(outblock,1,BLOCK_SIZE,out);
    bytes_read = fread(inblock,1,BLOCK_SIZE,in);
  }
  /* bytes_read != BLOCK_SIZE
   * Es el último bloque, posiblemente vacío.
   * Se aplica padding, colocando en cada byte de padding la longitud del mismo
   */
  padding_value = BLOCK_SIZE-bytes_read;
  for (i=bytes_read; i < BLOCK_SIZE; i++)
    inblock[i] = (unsigned char) padding_value;
  /* Ciframos el bloque con padding */
  AES_128_encrypt(inblock, outblock, key);
  fwrite(outblock,1,BLOCK_SIZE,out);
}


// TODO
void AES_128_ecb_encrypt(const unsigned char* in, unsigned char* out, unsigned long long len, unsigned long long* olen, const unsigned char* userkey){
	//parámetro in: viene con un buffer de len bytes, con bloques de 16 bytes
	//parámetro out: sale con el in ya cifrado pero con padding agregado
	//parámetro olen: el largo de out en bytes
	//parámetro len: el largo de in en bytes

	unsigned char inblock[BLOCK_SIZE];
	unsigned char outblock[BLOCK_SIZE];


	//debo crear un buffer para la clave expandida con el tamaño de 11*16
	unsigned char key[(ROUNDS+1)*BLOCK_SIZE];

	unsigned long bytes_read=0; //cuántos bloques voy leyendo

	unsigned int padding_value;
	int i=0,j=0;

	//obtengo la clave expandida
	AES_128_set_key(userkey,key);

	unsigned long length_in = 0;
	unsigned long length_padding = 0;

	length_in=len;
	
	if(length_in<BLOCK_SIZE || length_in==BLOCK_SIZE){
		//caso en el que estoy mandando un solo bloque
		olen[0]=BLOCK_SIZE;	
		length_padding = 0;
	}
	else{
		//caso en el que mando múltiples bloques de 16 bytes, el tamaño de la salida va a ser igual que el tamaño de la entrada
		olen[0]=length_in;
		
		//limite desde donde se va a poner el bloque de padding ya cifrado, en la salida
		length_padding = length_in-BLOCK_SIZE;
	}

	
	//no importa el largo de "in", paso cada bloque leido a inblock, y la funcion AES_128_encrypt procesa inblock, y lo cifrado, lo voy pasando a "out". hago de cuenta que el último bloque tiene 16 bytes, después lo trato por separado antes de enviar la salida
	
	for(i=0;i<length_in;i++){
		for(j=0;j<BLOCK_SIZE;j++){
			inblock[j]=in[i*BLOCK_SIZE+j];		
		}

		AES_128_encrypt(inblock,outblock,key);

		for(j=0;j<BLOCK_SIZE;j++){
			out[i*BLOCK_SIZE+j]=outblock[j];
		}
		bytes_read++; //ya leí 1 bloque, en la primera vuelta, en la segunda, 2 bloques, y así
	}
	
	
	// bytes_read != BLOCK_SIZE
	//Es el último bloque, posiblemente vacío.
	//Se aplica padding, colocando en cada byte de padding la longitud del mismo
	padding_value = BLOCK_SIZE-bytes_read;
	for (i=bytes_read; i < BLOCK_SIZE; i++){
		inblock[i] = (unsigned char) padding_value;
		//printf("%d\n",inblock[i]);
	}
	// Ciframos el bloque con padding
	AES_128_encrypt(inblock, outblock, key);
	for(i=length_padding;i<olen[0];i++){
		for(j=0;j<BLOCK_SIZE;j++){
			out[i*BLOCK_SIZE+j]=outblock[j];
		}
	}
}


void AES_128_ecb_encrypt_multiples(const unsigned char* in, unsigned char* out, unsigned long long len, unsigned long long* olen, const unsigned char* userkey){
	//parámetro in: viene con un buffer de len bytes, con bloques de 16 bytes
	//parámetro out: sale con el in ya cifrado pero con padding agregado
	//parámetro olen: el largo de out en bytes
	//parámetro len: el largo de in en bytes

	if(len-BLOCK_SIZE<(4*BLOCK_SIZE)-BLOCK_SIZE){
		printf("\nEsta función sirve para cifrar 4 bloques o más. Intente utilizar la versión secuencial.\n");	
		exit(EXIT_FAILURE);
	}
	unsigned char inblock[BLOCK_SIZE];
	unsigned char outblock[BLOCK_SIZE];


	//debo crear un buffer para la clave expandida con el tamaño de 11*16
	unsigned char key[(ROUNDS+1)*BLOCK_SIZE];

	unsigned long bytes_read=0; //cuántos bloques voy leyendo

	unsigned int padding_value;
	int i=0,j=0;

	//obtengo la clave expandida
	AES_128_set_key(userkey,key);
	
	unsigned char* in_multiple=NULL;
	unsigned char* out_multiple=NULL;
	unsigned char* in_multiple_2=NULL;
	unsigned char* out_multiple_2=NULL;

	
	//voy a cifrar los primeros len-(2*BLOCK_SIZE) bloques por un lado
	in_multiple=calloc(sizeof(unsigned char),(len-(BLOCK_SIZE*2))*BLOCK_SIZE);
	out_multiple=calloc(sizeof(unsigned char),(len-(BLOCK_SIZE*2))*BLOCK_SIZE);
	for(i=0;i<len-(BLOCK_SIZE*2);i++){
		for(j=0;j<BLOCK_SIZE;j++){
			in_multiple[i*BLOCK_SIZE+j]=in[i*BLOCK_SIZE+j];	
		}
		bytes_read++;
	}

	//ENCRIPTACION
	//sirve para 4 bloques (para arriba) de 16 bytes, hasta 8 bloques de 16 bytes
	if(len-BLOCK_SIZE>=(4*BLOCK_SIZE)-BLOCK_SIZE && len-BLOCK_SIZE <= (8*BLOCK_SIZE)-BLOCK_SIZE){
		AES_128_encrypt_2(in_multiple,out_multiple,key);
	}
	else{
		//si tengo 8 o más bloques de 16 bytes
		if(len-BLOCK_SIZE > (8*BLOCK_SIZE-BLOCK_SIZE)){
			AES_128_encrypt_8(in_multiple,out_multiple,key);
		}
	}

	//por otro lado, cifro el bloque len-BLOCK_SIZE, o sea, el penúltimo
	in_multiple_2=calloc(sizeof(unsigned char),BLOCK_SIZE);
	out_multiple_2=calloc(sizeof(unsigned char),BLOCK_SIZE);

	for(i=len-(BLOCK_SIZE*2);i<len-BLOCK_SIZE;i++){
		for(j=0;j<BLOCK_SIZE;j++){
			in_multiple_2[(i*BLOCK_SIZE)+j]=in[(i*BLOCK_SIZE)+j];	
		}
		bytes_read++;
	}
	
	AES_128_encrypt(in_multiple_2,out_multiple_2,key);

	for(i=0;i<len-(BLOCK_SIZE*2);i++){
		for(j=0;j<BLOCK_SIZE;j++){
			out[i*(BLOCK_SIZE)+j]=out_multiple[i*(BLOCK_SIZE)+j];	
		}
	}

	for(i=len-(BLOCK_SIZE*2);i<=len-BLOCK_SIZE;i++){
		for(j=0;j<BLOCK_SIZE;j++){	
		out[i*(BLOCK_SIZE)+j]=out_multiple_2[j];	
		}
	}
		

	//el tamaño de lo que sale es igual al tamaño de lo que entra
	olen[0]=len;
	//tomo el último bloque, y le agrego el padding
	unsigned long length_padding = len-BLOCK_SIZE;
	
	// bytes_read != BLOCK_SIZE
	//Es el último bloque, posiblemente vacío.
	//Se aplica padding, colocando en cada byte de padding la longitud del mismo
	padding_value = BLOCK_SIZE-bytes_read;
	for (i=bytes_read; i < BLOCK_SIZE; i++){
		inblock[i] = (unsigned char) padding_value;
		//printf("%d\n",inblock[i]);
	}
	// Ciframos el bloque con padding
	AES_128_encrypt(inblock, outblock, key);
	//le paso el bloque con padding ya cifrado al out
	for(i=length_padding;i<olen[0];i++){
		for(j=0;j<BLOCK_SIZE;j++){
			out[i*BLOCK_SIZE+j]=outblock[j];
		}
	}
}


void AES_128_ecb_encrypt_openmp(const unsigned char* in, unsigned char* out, unsigned long long len, unsigned long long* olen, const unsigned char* userkey){
	if(len<BLOCK_SIZE || len==BLOCK_SIZE){
		printf("\nSi se quiere cifrar un solo bloque de 16 bytes, utilizar la versión secuencial.\n");	
		exit(EXIT_FAILURE);
	}	
	unsigned char inblock[BLOCK_SIZE];
	unsigned char outblock[BLOCK_SIZE];

	unsigned char key[(ROUNDS+1)*BLOCK_SIZE];

	unsigned long bytes_read=0;

	unsigned int padding_value;
	int i=0,j=0;

	AES_128_set_key(userkey,key);

	unsigned long length_padding = 0;
	
	olen[0]=len;
		
	length_padding = len-BLOCK_SIZE;

	int canthilos=4; //cantidad por defecto de hilos para el programa
	omp_set_num_threads(canthilos);

	#pragma omp parallel for
	for(i=0;i<len;i++){
		for(j=0;j<BLOCK_SIZE;j++){
			inblock[j]=in[i*BLOCK_SIZE+j];		
		}

		AES_128_encrypt(inblock,outblock,key);

		for(j=0;j<BLOCK_SIZE;j++){
			out[i*BLOCK_SIZE+j]=outblock[j];
		}
		bytes_read++;
	}
	
	
	padding_value = BLOCK_SIZE-bytes_read;
	for (i=bytes_read; i < BLOCK_SIZE; i++){
		inblock[i] = (unsigned char) padding_value;
	}
	AES_128_encrypt(inblock, outblock, key);

	for(i=length_padding;i<olen[0];i++){
		for(j=0;j<BLOCK_SIZE;j++){
			out[i*BLOCK_SIZE+j]=outblock[j];
		}
	}
}
