#ifndef _CAP_AES_128_H
#define _CAP_AES_128_H

#define BLOCK_SIZE 16
#define KEY_SIZE 16
#define ROUNDS 10

/* Key setup
 * Expande la clave de 128 bits proporcionada en userkey
 * Devuelve en key la clave expandida
 * key debe apuntar a un buffer de tamaño 11*16
 * Esto es (ROUNDS+1)*BLOCK_SIZE
 */
void AES_128_set_key(const unsigned char *userkey, unsigned char *key);

/* Cifrado de un bloque
 * Cifra 128 bits provistos en in, y almacena los 128 bits de salida en out
 * subkeys debe apuntar a una clave expandida
 */
void AES_128_encrypt(const unsigned char* in, unsigned char* out, const unsigned char* subkeys);

void AES_128_encrypt_2(const unsigned char* in, unsigned char* out, const unsigned char* subkeys);
void AES_128_encrypt_8(const unsigned char* in, unsigned char* out, const unsigned char* subkeys);
#endif

