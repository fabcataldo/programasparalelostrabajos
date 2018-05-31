#include <wmmintrin.h>
#include "aes-128.h"



#define EXPAND_ASSIST(v1,v2,v3,v4,shuff_const,aes_const)                    \
    v2 = _mm_aeskeygenassist_si128(v4,aes_const);                           \
    v3 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(v3),              \
                                         _mm_castsi128_ps(v1), 16));        \
    v1 = _mm_xor_si128(v1,v3);                                              \
    v3 = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(v3),              \
                                         _mm_castsi128_ps(v1), 140));       \
    v1 = _mm_xor_si128(v1,v3);                                              \
    v2 = _mm_shuffle_epi32(v2,shuff_const);                                 \
    v1 = _mm_xor_si128(v1,v2)

#ifndef _mm_extract_epi32    
#define _mm_extract_epi32(x, imm) \
    _mm_cvtsi128_si32(_mm_srli_si128((x), 4 * (imm)))
#endif

void AES_128_set_key(const unsigned char *userkey, unsigned char *key)
{
    __m128i x0, x1, x2;
    __m128i *kp = (__m128i *) key;
    kp[0] = x0 = _mm_loadu_si128((__m128i *) userkey);
    x2 = _mm_setzero_si128();
    EXPAND_ASSIST(x0, x1, x2, x0, 255, 1);
    kp[1] = x0;
    EXPAND_ASSIST(x0, x1, x2, x0, 255, 2);
    kp[2] = x0;
    EXPAND_ASSIST(x0, x1, x2, x0, 255, 4);
    kp[3] = x0;
    EXPAND_ASSIST(x0, x1, x2, x0, 255, 8);
    kp[4] = x0;
    EXPAND_ASSIST(x0, x1, x2, x0, 255, 16);
    kp[5] = x0;
    EXPAND_ASSIST(x0, x1, x2, x0, 255, 32);
    kp[6] = x0;
    EXPAND_ASSIST(x0, x1, x2, x0, 255, 64);
    kp[7] = x0;
    EXPAND_ASSIST(x0, x1, x2, x0, 255, 128);
    kp[8] = x0;
    EXPAND_ASSIST(x0, x1, x2, x0, 255, 27);
    kp[9] = x0;
    EXPAND_ASSIST(x0, x1, x2, x0, 255, 54);
    kp[10] = x0;
}


void AES_128_encrypt(const unsigned char* in, unsigned char* out, const unsigned char* subkeys){
  int i;
  __m128i subkey = _mm_loadu_si128((__m128i*) subkeys); // Cargamos en subkey la primera subclave
  __m128i output = _mm_xor_si128(_mm_loadu_si128((__m128i*)in),subkey); // XOR del bloque de entrada con la primera clave de ronda
  // Ahora vienen las 9 primeras rondas
  for (i = 1; i < ROUNDS; i++){
    subkey = _mm_loadu_si128((__m128i*)(subkeys+(i*BLOCK_SIZE)));
    output = _mm_aesenc_si128(output,subkey);
  }
  // Última ronda
  subkey = _mm_loadu_si128((__m128i*)(subkeys+(ROUNDS*BLOCK_SIZE)));
  output = _mm_aesenclast_si128(output, subkey);
  _mm_storeu_si128((__m128i*) out, output);
}


void AES_128_encrypt_2(const unsigned char* in, unsigned char* out, const unsigned char* subkeys){
  int i;
  __m128i subkey = _mm_loadu_si128((__m128i*) subkeys); 
  __m128i out_1 = _mm_xor_si128(_mm_loadu_si128((__m128i*)in),subkey);
  __m128i out_2 = _mm_xor_si128(_mm_loadu_si128((__m128i*)(in+BLOCK_SIZE)),subkey);
  for (i = 1; i < ROUNDS; i++){
    subkey = _mm_loadu_si128((__m128i*)(subkeys+(i*BLOCK_SIZE)));
    out_1 = _mm_aesenc_si128(out_1,subkey);
    out_2 = _mm_aesenc_si128(out_2,subkey);
  }
  subkey = _mm_loadu_si128((__m128i*)(subkeys+(ROUNDS*BLOCK_SIZE)));
  out_1 = _mm_aesenclast_si128(out_1, subkey);
  out_2 = _mm_aesenclast_si128(out_2, subkey);
  _mm_storeu_si128((__m128i*) out, out_1);
  _mm_storeu_si128((__m128i*) (out+BLOCK_SIZE), out_2);
}

void AES_128_encrypt_4(const unsigned char* in, unsigned char* out, const unsigned char* subkeys){
  int i;
  __m128i subkey = _mm_loadu_si128((__m128i*) subkeys); 
  __m128i out_1 = _mm_xor_si128(_mm_loadu_si128((__m128i*)in),subkey);
  __m128i out_2 = _mm_xor_si128(_mm_loadu_si128((__m128i*)(in+BLOCK_SIZE)),subkey);

  __m128i out_3 = _mm_xor_si128(_mm_loadu_si128((__m128i*)(in+(BLOCK_SIZE*2))),subkey);
  __m128i out_4 = _mm_xor_si128(_mm_loadu_si128((__m128i*)(in+(BLOCK_SIZE*3))),subkey);
  for (i = 1; i < ROUNDS; i++){
    subkey = _mm_loadu_si128((__m128i*)(subkeys+(i*BLOCK_SIZE)));
    out_1 = _mm_aesenc_si128(out_1,subkey);
    out_2 = _mm_aesenc_si128(out_2,subkey);

    out_3 = _mm_aesenc_si128(out_3,subkey);
    out_4 = _mm_aesenc_si128(out_4,subkey);
  }
  subkey = _mm_loadu_si128((__m128i*)(subkeys+(ROUNDS*BLOCK_SIZE)));
  out_1 = _mm_aesenclast_si128(out_1, subkey);
  out_2 = _mm_aesenclast_si128(out_2, subkey);

  out_3 = _mm_aesenclast_si128(out_3, subkey);
  out_4 = _mm_aesenclast_si128(out_4, subkey);

  _mm_storeu_si128((__m128i*) out, out_1);
  _mm_storeu_si128((__m128i*) (out+BLOCK_SIZE), out_2);
  _mm_storeu_si128((__m128i*) (out+(BLOCK_SIZE*2)), out_3);
  _mm_storeu_si128((__m128i*) (out+(BLOCK_SIZE*3)), out_4);
}

void AES_128_encrypt_8(const unsigned char* in, unsigned char* out, const unsigned char* subkeys){
  int i;
  __m128i subkey = _mm_loadu_si128((__m128i*) subkeys); 
  __m128i out_1 = _mm_xor_si128(_mm_loadu_si128((__m128i*)in),subkey);
  __m128i out_2 = _mm_xor_si128(_mm_loadu_si128((__m128i*)(in+BLOCK_SIZE)),subkey);

  __m128i out_3 = _mm_xor_si128(_mm_loadu_si128((__m128i*)(in+(BLOCK_SIZE*2))),subkey);
  __m128i out_4 = _mm_xor_si128(_mm_loadu_si128((__m128i*)(in+(BLOCK_SIZE*3))),subkey);
  __m128i out_5 = _mm_xor_si128(_mm_loadu_si128((__m128i*)(in+(BLOCK_SIZE*4))),subkey);
  __m128i out_6 = _mm_xor_si128(_mm_loadu_si128((__m128i*)(in+(BLOCK_SIZE*5))),subkey);
  __m128i out_7 = _mm_xor_si128(_mm_loadu_si128((__m128i*)(in+(BLOCK_SIZE*6))),subkey);
  __m128i out_8 = _mm_xor_si128(_mm_loadu_si128((__m128i*)(in+(BLOCK_SIZE*7))),subkey);
  for (i = 1; i < ROUNDS; i++){
    subkey = _mm_loadu_si128((__m128i*)(subkeys+(i*BLOCK_SIZE)));
    out_1 = _mm_aesenc_si128(out_1,subkey);
    out_2 = _mm_aesenc_si128(out_2,subkey);

    out_3 = _mm_aesenc_si128(out_3,subkey);
    out_4 = _mm_aesenc_si128(out_4,subkey);
    out_5 = _mm_aesenc_si128(out_5,subkey);
    out_6 = _mm_aesenc_si128(out_6,subkey);
    out_7 = _mm_aesenc_si128(out_7,subkey);
    out_8 = _mm_aesenc_si128(out_8,subkey);
  }
  subkey = _mm_loadu_si128((__m128i*)(subkeys+(ROUNDS*BLOCK_SIZE)));
  out_1 = _mm_aesenclast_si128(out_1, subkey);
  out_2 = _mm_aesenclast_si128(out_2, subkey);

  out_3 = _mm_aesenclast_si128(out_3, subkey);
  out_4 = _mm_aesenclast_si128(out_4, subkey);
  out_5 = _mm_aesenclast_si128(out_5, subkey);
  out_6 = _mm_aesenclast_si128(out_6, subkey);
  out_7 = _mm_aesenclast_si128(out_7, subkey);
  out_8 = _mm_aesenclast_si128(out_8, subkey);
  _mm_storeu_si128((__m128i*) out, out_1);
  _mm_storeu_si128((__m128i*) (out+BLOCK_SIZE), out_2);

  _mm_storeu_si128((__m128i*) (out+(BLOCK_SIZE*2)), out_3);
  _mm_storeu_si128((__m128i*) (out+(BLOCK_SIZE*3)), out_4);
  _mm_storeu_si128((__m128i*) (out+(BLOCK_SIZE*4)), out_5);
  _mm_storeu_si128((__m128i*) (out+(BLOCK_SIZE*5)), out_6);
  _mm_storeu_si128((__m128i*) (out+(BLOCK_SIZE*6)), out_7);
  _mm_storeu_si128((__m128i*) (out+(BLOCK_SIZE*7)), out_8);
}
