#ifndef _CAP_UTIL_H
#define _CAP_UTIL_H

int str2bytes(char *cp, unsigned char *bytes, int len);

void bytes2hex(unsigned char *bytes, char *str, int nbytes);

void print_block(void* block);

#endif