#ifndef TBL
#define TBL
#include "table.h"
#endif

#include <string.h>
#include "getSource.h"

#define MAXLEVEL 5
#define MAXNAME 31
#define MAXTABLE 100

typedef struct tableE {
  KindT kind;
  char name[MAXNAME];
  union {
    int value;
    struct {
        RelativeAddr relativeAddr;
        int paramCount;
    } f;
    RelativeAddr relativeAddr;
  } u;
} TableE;

static TableE nameTable[MAXTABLE];
static int level = -1;
static int localAddr;
static int tableIndex;
static int lastIndex[MAXLEVEL];
static int addr[MAXLEVEL];
static int tableFunctionIndex;

void blockBegin(int firstAddr)
{
  if (level == -1) {
    localAddr = firstAddr;
    tableIndex = 0;
    level++;
    return;
  }

  lastIndex[level] = tableIndex;
  addr[level] = localAddr;
  localAddr = firstAddr;
  level++;
  return;
}

void blockEnd()
{
  level--;
  tableIndex = lastIndex[level];
  localAddr = addr[level];
}

int currentBlockLevel()
{
  return(level);
}

int functionParamCount()
{
  return(nameTable[lastIndex[level-1]].u.f.paramCount);
}

void enterT(char *id)
{
  if (tableIndex++ < MAXTABLE) strcpy(nameTable[tableIndex].name, id);
}

int enterTfunc(char *id, int v)
{
  enterT(id);
  nameTable[tableIndex].kind = funcId;
  nameTable[tableIndex].u.f.relativeAddr.level = level;
  nameTable[tableIndex].u.f.relativeAddr.addr = v;
  nameTable[tableIndex].u.f.paramCount = 0;
  tableFunctionIndex = tableIndex;
  return tableIndex;
}

int enterTpar(char *id)
{
  enterT(id);
  nameTable[tableIndex].kind = parId;
  nameTable[tableIndex].u.relativeAddr.level = level;
  nameTable[tableFunctionIndex].u.f.paramCount++;
  return tableIndex;
}

int enterTvar(char *id)
{
  enterT(id);
  nameTable[tableIndex].kind = varId;
  nameTable[tableIndex].u.relativeAddr.level = level;
  nameTable[tableIndex].u.relativeAddr.addr = localAddr++;
  return(tableIndex);
}

int enterTconst(char *id, int v)
{
  enterT(id);
  nameTable[tableIndex].kind = constId;
  nameTable[tableIndex].u.value = v;
  return(tableIndex);
}

void endFunctionParam()
{
  int i;
  int paramCount = nameTable[tableFunctionIndex].u.f.paramCount;
  if (paramCount == 0)  return;
  for (i=1; i<=paramCount; i++) nameTable[tableFunctionIndex+i].u.relativeAddr.addr = i-1-paramCount;
}

void changeV(int tableIndex, int newVal)
{
  nameTable[tableIndex].u.f.relativeAddr.addr = newVal;
}

int searchT(char *id, KindT k)
{
  int i;
  i = tableIndex;
  strcpy(nameTable[0].name, id);
  while( strcmp(id, nameTable[i].name) ) i--;
  return(i);
}

KindT kindT(int i)
{
  return(nameTable[i].kind);
}

RelativeAddr relAddr(int tableIndex)
{
  return(nameTable[tableIndex].u.relativeAddr);
}

int val(int tableIndex)
{
  return(nameTable[tableIndex].u.value);
}

int LocalAddr()
{
  return(localAddr);
}
