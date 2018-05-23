#pragma once
#include <stdio.h>

/* Cifra el contenido del archivo in con la clave userkey provista por el usuario
 * y escribe el resultado en el archivo out
 */
void AES_128_ecb_encrypt_file(FILE* in, FILE* out, const unsigned char* userkey);

/* Cifra len bytes del buffer in con la clave userkey provista por el usuario
 * y escribe el resultado en el buffer out. El argumento olen devuelve la cantidad
 * de bytes escrita en out.
 */
void AES_128_ecb_encrypt(const unsigned char* in, unsigned char* out, unsigned long long len, unsigned long long* olen, const unsigned char* key);

void AES_128_ecb_encrypt_multiples(const unsigned char* in, unsigned char* out, unsigned long long len, unsigned long long* olen, const unsigned char* userkey);
