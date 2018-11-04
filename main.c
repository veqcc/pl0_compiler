#include <stdio.h>
#include "getSource.h"
#include "compile.h"
#include "codegen.h"

int main()
{
  char fileName[30];
  printf("enter source file name\n");
  scanf("%s", fileName);

  openSource(fileName);
  compile();
  execute();
  closeSource();

  return(0);
}
