#include <stdio.h>
#include "getSource.h"

int main()
{
  printf("main\n");
  char fileName[30];
  printf("enter source file name¥n");
  scanf("%s", fileName);

  openSource(fileName);
  compile();
  execute();
  closeSource();

  return(0);
}
