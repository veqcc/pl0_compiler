#include "getSource.h"
#include "codegen.h"

#ifndef TBL
#define TBL
#include "table.h"
#endif

#define FIRSTADDR 2

static Token token;
static void block(int pIndex);
static void constDecl();
static void varDecl();
static void statement();
static void expression();
static void term();
static void factor();
static int isStBeginKey(Token t);

void compile()
{
  printf("compile\n");
  int i;
  initSource();
  token = nextToken();
  blockBegin(FIRSTADDR);
  block(0);
  listCode();
  finalSource();
}

void block(int pIndex)
{
  printf("block\n");
  int backP;
  backP = genCodeV(jmp, 0);
  while(1) {
    switch (token.kind) {
      case Const:
        token = nextToken();
        constDecl();
        continue;
      case Var:
        token = nextToken();
        varDecl();
        continue;
      default:
        break;
    }
    break;
  }
  backPatch(backP);
  changeV(pIndex, nextCode());
  genCodeV(ict, frameL());
  statement();
  genCodeR();
  blockEnd();
}

void constDecl()
{
  printf("constDecl\n");
  Token tmp;
  while(1) {
    tmp = token;
    token = checkGet(nextToken(), Equal);
    if (token.kind == Num) enterTconst(tmp.u.id, token.u.value);
    token = nextToken();
    break;
  }
  token = checkGet(token, Semicolon);
}

void varDecl()
{
  printf("varDecl\n");
  while(1) {
    enterTvar(token.u.id);
    token = nextToken();
    break;
  }
  token = checkGet(token, Semicolon);
}

void statement()
{
  int tIndex;
  KindT k;
  int backP, backP2;

  while(1) {
    switch(token.kind) {
      case Id:
        tIndex = searchT(token.u.id, varId);
        token = checkGet(nextToken(), Assign);
        expression();
        genCodeT(sto, tIndex);
        return;
      case Begin:
        token = nextToken();
        while(1) {
          statement();
          while(1) {
            if (token.kind == Semicolon) {
              token = nextToken();
              break;
            }
            if (token.kind == End) {
              token = nextToken();
              return;
            }
            if (isStBeginKey(token)) break;
            token = nextToken();
          }
        }
      case Write:
        token = nextToken();
        expression();
        genCodeO(wrl);
        return;
      default:
        token = nextToken();
        continue;
    }
  }
}

void expression()
{
  printf("expression\n");
  term();
}

void term()
{
  printf("term\n");
	factor();
}

void factor()
{
  printf("factor\n");
	int tIndex;
	KindT k;
	if (token.kind==Id){
		tIndex = searchT(token.u.id, varId);
		switch (k) {
  		case varId: case parId:
  			genCodeT(lod, tIndex);
  			token = nextToken(); break;
  		case constId:
  			genCodeV(lit, val(tIndex));
  			token = nextToken(); break;
		}
	}
}

int isStBeginKey(Token t)
{
  printf("isStBeginKey\n");
  switch(t.kind) {
    case Begin: case Write: return 1;
    default: return 0;
  }
}
