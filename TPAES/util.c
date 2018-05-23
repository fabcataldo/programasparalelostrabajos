#include "util.h"
#include <ctype.h>
#include <stdio.h>


int _str2bytes(char *cp, unsigned char *bytes, int len)
{
    int i = 0;			// this is a count for the input digits processed
    int by = 0;
    char ch;

    while (i < (len * 2) && *cp) {
	ch = toupper((int)*cp++);	// process a hexadecimal digit
	if (ch >= '0' && ch <= '9')
	    by = (by << 4) + ch - '0';
	else if (ch >= 'A' && ch <= 'F')
	    by = (by << 4) + ch - 'A' + 10;
	else			// error if not hexadecimal
	{
	    return (-2);
	}

	// store a key byte for each pair of hexadecimal digits
	if (i++ & 1)
	    bytes[i / 2 - 1] = by & 0xff;
    }

    if (*cp) {
	return (-1);
    } else if (i != 2 * len) {
	return (-4);
    }

    return len;
}

int str2bytes(char* cp, unsigned char *bytes, int len){
  int i;
  unsigned int u;
  for (i = 0; i < len; i++){
    if (sscanf(cp,"%2x", &u) < 1)
      return(-2);
    bytes[i] = (unsigned char) u;
    cp+=2;
  }
  return len;
}

void bytes2hex(unsigned char *bytes, char *str, int nbytes)
{
    int i;
    for (i = 0; i < nbytes; i++) {
	str += sprintf(str, "%02x", (int) bytes[i]);
    }
}

void print_block(void* block){
  char str[33];
  bytes2hex((unsigned char*)block,str,16);
  printf("%s\n",str);
}
