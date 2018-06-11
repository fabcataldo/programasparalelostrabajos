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

void read_buff(const unsigned char* a, int size){
	int i=0;
	for(i=0;i<size;i++){
		printf("\n %d", a[i]);	
	}
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
	//obtengo la clave expandida
	AES_128_set_key(userkey,key);

	int i;
	unsigned int filled_padd = len%BLOCK_SIZE; //lo que ya llené del último bloque de 16 bytes

	//PADDING es lo que me falta para rellenar el último bloque que tiene que ser de 16 bytes
	unsigned int padding_value = BLOCK_SIZE-filled_padd;

	//complete_blocks se refiere a los bloques que son múltiplos de 16, bloques completos
	//¿cuántos bloques entran en 16 bytes? con esto saco los bloques que están completos
	unsigned int complete_blocks = len/BLOCK_SIZE;
	
	//encripto los primeros bloques múltiplos de 16
	//corro los punteros in y out para que lleguen hasta después del último bloque completo
	for(i=0; i<complete_blocks; i++){
		AES_128_encrypt(in, out, key);
		in+=BLOCK_SIZE;
		out+=BLOCK_SIZE;
	}

	//ahora encripto los bytes que faltan con padding_value
	//Es el último bloque, posiblemente vacío.
	//Se aplica padding, colocando en cada byte de padding la longitud del mismo
	//ahora como in está después del último bloque completo, si quedó algo sin cifrar antes (bloque incompleto) lo paso a inblocks para luego
	//agregarle el padding y cifrar
	for(i=0; i<filled_padd; i++){
		inblock[i]=in[i];
	}
	for (i=filled_padd; i < BLOCK_SIZE; i++){
		inblock[i] = (unsigned char) padding_value;
	}
	// Ciframos el bloque con padding
	AES_128_encrypt(inblock, outblock, key);

	//le paso el bloque con padding ya cifrado al out
	//como out ya está apuntando a después del último bloque completo, le paso outblock conseguido antes
	for(i = 0; i < BLOCK_SIZE; i++){
		out[i] = outblock[i];
	}

	//complete_len sería el len que viene de entrada más el padding
	unsigned int complete_len = len+padding_value;
	*olen=complete_len;
}


void AES_128_ecb_encrypt_niv(const unsigned char* in, unsigned char* out, unsigned long long len, unsigned long long* olen, const unsigned char* userkey){
	unsigned char inblock[BLOCK_SIZE];
	unsigned char outblock[BLOCK_SIZE];

	unsigned char key[(ROUNDS+1)*BLOCK_SIZE];
	AES_128_set_key(userkey,key);

	int i;
	unsigned int filled_padd = len%BLOCK_SIZE;
	unsigned int padding_value = BLOCK_SIZE-filled_padd;
	unsigned int complete_blocks = len/BLOCK_SIZE;

	if(complete_blocks==1){
		printf("\nSi se quiere encriptar un solo bloque, utilice la versión secuencial.");	
		exit(EXIT_FAILURE);
	}
	if(complete_blocks>=2 || complete_blocks <4){
			AES_128_encrypt_2(in,out,key);
			in+=2*BLOCK_SIZE;
			out+=2*BLOCK_SIZE;
	}
	if(complete_blocks>=4  || complete_blocks<8){
		AES_128_encrypt_4(in,out,key);	
		in+=4*BLOCK_SIZE;
		out+=4*BLOCK_SIZE;
	}	
	if(complete_blocks>=8  || complete_blocks >8){
			//ya me tocan 8 bloques o más
			for(i=0;i<complete_blocks;i+=8){
				AES_128_encrypt_8(in,out,key);	
				in+=8*BLOCK_SIZE;
				out+=8*BLOCK_SIZE;
			}
	}

	for(i=0; i<filled_padd; i++){
		inblock[i]=in[i];
	}
	for (i=filled_padd; i < BLOCK_SIZE; i++){
		inblock[i] = (unsigned char) padding_value;
	}
	AES_128_encrypt(inblock, outblock, key);

	for(i = 0; i < BLOCK_SIZE; i++){
		out[i] = outblock[i];
	}

	unsigned int complete_len = len+padding_value;
	*olen=complete_len;
}


void AES_128_ecb_encrypt_openmp(const unsigned char* in, unsigned char* out, unsigned long long len, unsigned long long* olen, const unsigned char* userkey){
	if(len<BLOCK_SIZE || len==BLOCK_SIZE){
		printf("\nSi se quiere cifrar un solo bloque de 16 bytes, utilizar la versión secuencial.\n");	
		exit(EXIT_FAILURE);
	}	

	unsigned char inblock[BLOCK_SIZE];
	unsigned char outblock[BLOCK_SIZE];

	unsigned char key[(ROUNDS+1)*BLOCK_SIZE];
	AES_128_set_key(userkey,key);

	int i;
	unsigned int filled_padd = len%BLOCK_SIZE;
	unsigned int padding_value = BLOCK_SIZE-filled_padd;
	unsigned int complete_blocks = len/BLOCK_SIZE;
	
	omp_set_num_threads(4);	
	#pragma omp parallel for
	for(i=0; i<complete_blocks; i++){
		AES_128_encrypt(in, out, key);
		in+=BLOCK_SIZE;
		out+=BLOCK_SIZE;
	}

	for(i=0; i<filled_padd; i++){
		inblock[i]=in[i];
	}
	for (i=filled_padd; i < BLOCK_SIZE; i++){
		inblock[i] = (unsigned char) padding_value;
	}
	AES_128_encrypt(inblock, outblock, key);

	for(i = 0; i < BLOCK_SIZE; i++){
		out[i] = outblock[i];
	}

	unsigned int complete_len = len+padding_value;
	*olen=complete_len;
}
