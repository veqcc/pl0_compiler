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
      RelAddr raddr;
      int pars;
    } f;
    RelAddr raddr;
  } u;
} TableE;

static TableE nameTable[MAXTABLE];
static int level = -1;
static int localAddr;
static int tIndex;
static int lastIndex[MAXLEVEL];
static int addr[MAXLEVEL];

void blockBegin(int firstAddr)
{
  printf("blockBegin\n");
  if (level == -1) {
    localAddr = firstAddr;
    tIndex = 0;
    level++;
    return;
  }

  lastIndex[level] = tIndex;
  addr[level] = localAddr;
  localAddr = firstAddr;
  level++;
  return;
}

void blockEnd()
{
  printf("blockEnd\n");
  level--;
  tIndex = lastIndex[level];
  localAddr = addr[level];
}

int bLevel()
{
  printf("bLevel\n");
  return(level);
}

int fPars()
{
  printf("fPars\n");
  return(nameTable[lastIndex[level-1]].u.f.pars);
}

void enterT(char *id)
{
  printf("enterT\n");
  if (tIndex++ < MAXTABLE) strcpy(nameTable[tIndex].name, id);
}

int enterTvar(char *id)
{
  printf("enterTvar\n");
  enterT(id);
  nameTable[tIndex].kind = varId;
  nameTable[tIndex].u.raddr.level = level;
  nameTable[tIndex].u.raddr.addr = localAddr++;
  return(tIndex);
}

int enterTconst(char *id, int v)
{
  printf("enterTconst\n");
  enterT(id);
  nameTable[tIndex].kind = constId;
  nameTable[tIndex].u.value = v;
  return(tIndex);
}

void changeV(int ti, int newVal)
{
  printf("changeV\n");
  nameTable[ti].u.f.raddr.addr = newVal;
}

int searchT(char *id, KindT k)
{
  printf("searchT\n");
  int i;
  i = tIndex;
  strcpy(nameTable[0].name, id);
  while( strcmp(id, nameTable[i].name) ) i--;
  return(i);
}

KindT kindT(int i)
{
  printf("KindT\n");
  return(nameTable[i].kind);
}

RelAddr relAddr(int ti)
{
  printf("relAddr\n");
  return(nameTable[ti].u.raddr);
}

int val(int ti)
{
  printf("val\n");
  return(nameTable[ti].u.value);
}

int frameL()
{
  printf("frameL\n");
  return(localAddr);
}
