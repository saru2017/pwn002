#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void flag()
{
  puts("flag is HANDAI_CTF");
}

void saru()
{
  char buf[16];
  int dummy = 0x1234;
  int secret = 0;

  gets(buf);

  printf("buf[] = %s\n", buf);
}

int main(){
  saru();

  return 0;
}

