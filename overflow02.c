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
//  int dummy = 0x1234;
//  int secret = 0;
  puts("hello world");

  gets(buf);
//  printf("buf = %p\n", buf);
//  printf("dummy = %p\n", &dummy);
//  printf("secret = %p\n", &secret);
}

int main(){
  saru();

  printf("main = %p\n", &main);
  printf("saru = %p\n", &saru);
  printf("flag = %p\n", &flag);
  return 0;
}

