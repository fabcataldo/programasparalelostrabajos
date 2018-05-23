#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include "aes-ecb.h"
#include "aes-128.h"
#include <assert.h>

#define CANARY_LENGTH (16*BLOCK_SIZE)
#define NUM_KEYS 2

unsigned char userkeys[NUM_KEYS][KEY_SIZE] = {
    {0},
    {1,2,3,4,5,6,7,8,8,7,6,5,4,3,2,1}
};

unsigned char paddings[BLOCK_SIZE][BLOCK_SIZE] = {
    {16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16},
    { 0,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15},
    { 0, 0,14,14,14,14,14,14,14,14,14,14,14,14,14,14},
    { 0, 0, 0,13,13,13,13,13,13,13,13,13,13,13,13,13},
    { 0, 0, 0, 0,12,12,12,12,12,12,12,12,12,12,12,12},
    { 0, 0, 0, 0, 0,11,11,11,11,11,11,11,11,11,11,11},
    { 0, 0, 0, 0, 0, 0,10,10,10,10,10,10,10,10,10,10},
    { 0, 0, 0, 0, 0, 0, 0, 9, 9, 9, 9, 9, 9, 9, 9, 9},
    { 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8},
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 7, 7},
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6},
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5},
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4},
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3},
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2},
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}
};

unsigned char encrypted_paddings[NUM_KEYS][BLOCK_SIZE][BLOCK_SIZE];

unsigned char encrypted_blocks[NUM_KEYS][BLOCK_SIZE];

void gen_test_cases(){
    unsigned char block[BLOCK_SIZE] = {0};
    unsigned char key[(ROUNDS+1)*BLOCK_SIZE];
    for (int i = 0; i < NUM_KEYS; i++){
        AES_128_set_key(userkeys[i],key);
        AES_128_encrypt(block,encrypted_blocks[i],key);
        for (int j = 0; j < BLOCK_SIZE; j++){
            AES_128_encrypt(paddings[j],encrypted_paddings[i][j],key);
        }
    }
}

size_t output_length(size_t len) {
    return (len + BLOCK_SIZE - (len % BLOCK_SIZE));
}

unsigned char* create_canary_buffer(size_t len){
    unsigned char *canary = calloc(sizeof(unsigned char),2*CANARY_LENGTH + len);
    assert(canary != NULL);
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
    srand48((long int)(canary));
    unsigned char* c = canary;
    for(int i = 0; i < CANARY_LENGTH; i++)
        c[i] = lrand48();
    c = canary + CANARY_LENGTH + len;
    for(int i = 0; i < CANARY_LENGTH; i++)
        c[i] = lrand48();
    return canary + CANARY_LENGTH;
}



void free_canary_buffer(unsigned char *buffer){
    free(buffer-CANARY_LENGTH);
}


void check_canary_buffer(unsigned char* buffer, size_t len){
    unsigned char *canary = buffer - CANARY_LENGTH;
    srand48((long int)canary);
    for(int i = 0; i < CANARY_LENGTH; i++)
        g_assert(canary[i] == (unsigned char)lrand48());
    canary += CANARY_LENGTH + len;
    for(int i = 0; i < CANARY_LENGTH; i++)
        g_assert(canary[i] == (unsigned char)lrand48());
}

void test_null_operation(){
    unsigned char* buffer = create_canary_buffer(0);
    check_canary_buffer(buffer,0);
    free_canary_buffer(buffer);
}

void check_ecb_encryption(int key_index, size_t len, size_t olen, unsigned char *ciphertext){
    g_assert(olen == output_length(len));
    size_t blocks = olen / BLOCK_SIZE;
    for (int i = 0; i < blocks-1; i++){
        g_assert(memcmp(ciphertext+i*BLOCK_SIZE,encrypted_blocks[key_index],BLOCK_SIZE) == 0);
    }

    g_assert(memcmp(ciphertext+(blocks-1)*BLOCK_SIZE,encrypted_paddings[key_index][len%BLOCK_SIZE],BLOCK_SIZE) == 0);
}

void check_zero(unsigned char *buffer, size_t len){
  for(size_t i = 0; i < len; i++)
      g_assert(buffer[i] == 0);
}

void test_empty_message(){
    unsigned char in[0];
    unsigned char *out = create_canary_buffer(output_length(0));
    unsigned long long olen = 0;
    for (int k = 0; k < NUM_KEYS; k++){
        AES_128_ecb_encrypt(in,out,0,&olen,userkeys[k]);
        g_assert(olen == BLOCK_SIZE);
        check_canary_buffer(out,output_length(0));
        g_assert(memcmp(out,encrypted_paddings[k][0],BLOCK_SIZE) == 0);
    }
    free_canary_buffer(out);
}

void test_short_messages(){
    unsigned char in[BLOCK_SIZE] = {0};
    unsigned long long olen = 0;
    for (int len = 1; len < BLOCK_SIZE; len++){
        unsigned char *out = create_canary_buffer(output_length(len));
        for (int k = 0; k < NUM_KEYS; k++){
            AES_128_ecb_encrypt(in,out,len,&olen,userkeys[k]);
            g_assert(olen == BLOCK_SIZE);
            check_zero(in,BLOCK_SIZE);
            check_canary_buffer(out,output_length(len));
            g_assert(memcmp(out,encrypted_paddings[k][len],BLOCK_SIZE) == 0);
        }
        free_canary_buffer(out);
    }
}

void test_complete_blocks(){
    const int blocks = 100;
    unsigned char *in = calloc(sizeof(unsigned char),blocks*BLOCK_SIZE);
    unsigned long long olen = 0;
    for(size_t i = 1; i <= blocks; i++){
        size_t len = i * BLOCK_SIZE;
        unsigned char *out = create_canary_buffer(output_length(len));
        for (int k = 0; k < NUM_KEYS; k++){
            AES_128_ecb_encrypt(in,out,len,&olen,userkeys[k]);
            check_zero(in,len);
            check_canary_buffer(out,output_length(len));
	//printf("\nASDADSADS k=%d\n",k);
            check_ecb_encryption(k,len,olen,out);
        }
        free_canary_buffer(out);
    }
    free(in);
}

void test_incomplete_blocks(){
    const int max_len = 200;
    unsigned char *in = calloc(sizeof(unsigned char),max_len);
    unsigned long long olen = 0;
    for(size_t len = 17; len <= max_len; len++){
        if (len % BLOCK_SIZE == 0) continue;
        unsigned char *out = create_canary_buffer(output_length(len));
        for (int k = 0; k < NUM_KEYS; k++){
            AES_128_ecb_encrypt(in,out,len,&olen,userkeys[k]);
            check_zero(in,len);
            check_canary_buffer(out,output_length(len));
            check_ecb_encryption(k,len,olen,out);
        }
        free_canary_buffer(out);
    }
    free(in);
}

void test_inplace(){
    const int max_len = 200;
    unsigned long long olen = 0;
    for(size_t len = 1; len <= max_len; len++){
        for (int k = 0; k < NUM_KEYS; k++){
            unsigned char *inout = create_canary_buffer(output_length(len));
            AES_128_ecb_encrypt(inout,inout,len,&olen,userkeys[k]);
            check_canary_buffer(inout,output_length(len));
            check_ecb_encryption(k,len,olen,inout);
            free_canary_buffer(inout);
        }
    }
}



int main(int argc, char **argv){
    gen_test_cases();
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/sanity/test_null_operation", test_null_operation);
    g_test_add_func("/encrypt/test_empty_message", test_empty_message);
    g_test_add_func("/encrypt/test_short_messages", test_short_messages);
    g_test_add_func("/encrypt/test_complete_blocks",test_complete_blocks);
    g_test_add_func("/encrypt/test_incomplete_blocks",test_incomplete_blocks);
    g_test_add_func("/encrypt/test_inplace",test_inplace);
    return g_test_run();
}
