#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void flag()
{
  puts("flag is HANDAI_CTF");
  fflush(stdout);
//  sleep(1);
}

void saru()
{
  char buf[16];

  gets(buf);
}

int main(){
  saru();

  return 0;
}

