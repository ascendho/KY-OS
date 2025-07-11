#include <stdio.h>
#include "greeting.h"
#define N 10
int main()
{
  char name[N];
  printf("your name, please:");
  scanf("%s",name);
  greeting(name);
  return 0;
}

