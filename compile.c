#include "getSource.h"
#include "codegen.h"

#ifndef TBL
#define TBL
#include "table.h"
#endif

#define FIRSTADDR 2

static Token token;
static void block(int nameTableIndex);
static void constDecl();
static void varDecl();
static void funcDecl();
static void statement();
static void expression();
static void term();
static void factor();
static void condition();
static int isStBeginKey(Token t);

void compile()
{
  initSource();
  token = nextToken();
  blockBegin(FIRSTADDR);
  block(0);
  listCode();
}

void block(int nameTableIndex)
{
  int backPatchIndex;
  backPatchIndex = genCodeWithValue(jmp, 0);
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
      case Func:
        token = nextToken();
        funcDecl();
        continue;
      default:
        break;
    }
    break;
  }
  backPatch(backPatchIndex);
  changeV(nameTableIndex, nextCode());
  genCodeWithValue(ict, LocalAddr());
  statement();
  genCodeReturn();
  blockEnd();
}

void constDecl()
{
  Token prev_token;
  while(1) {
    prev_token = token;
    token = checkGet(nextToken(), Equal);
    if (token.kind == Num) enterTconst(prev_token.u.id, token.u.value);
    token = nextToken();
    if (token.kind != Comma) break;
    token = nextToken();
  }
  token = checkGet(token, Semicolon);
}

void varDecl()
{
  while(1) {
    enterTvar(token.u.id);
    token = nextToken();
    if (token.kind != Comma) break;
    token = nextToken();
  }
  token = checkGet(token, Semicolon);
}

void funcDecl()
{
  int funcIndex;
  funcIndex = enterTfunc(token.u.id, nextCode());
  token = checkGet(nextToken(), Lparen);
  blockBegin(FIRSTADDR);

  while (1) {
    if (token.kind != Id) break;
    enterTpar(token.u.id);
    token = nextToken();
    if (token.kind != Comma) break;
    token = nextToken();
  }

  token = checkGet(token, Rparen);
  endFunctionParam();
  block(funcIndex);
  token = checkGet(token, Semicolon);
}

void statement()
{
  int tableIndex;
  int backPatchIfIndex, backPatchLoopExitIndex, backPatchLoopStartIndex;

  while(1) {
    switch(token.kind) {
      case Id:
          tableIndex = searchT(token.u.id);
          token = checkGet(nextToken(), Assign);
          expression();
          genCodeWithAddr(sto, tableIndex);
          return;
      case If:
        token = nextToken();
        condition();
        token = checkGet(token, Then);
        backPatchIfIndex = genCodeWithValue(jpc, 0);
        statement();
        backPatch(backPatchIfIndex);
        return;
      case Ret:
        token = nextToken();
        expression();
        genCodeReturn();
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
      case While:
        token = nextToken();
        backPatchLoopStartIndex = nextCode();
        condition();
        token = checkGet(token, Do);
        backPatchLoopExitIndex = genCodeWithValue(jpc, 0);
        statement();
        genCodeWithValue(jmp, backPatchLoopStartIndex);
        backPatch(backPatchLoopExitIndex);
        return;
      case Write:
        token = nextToken();
        expression();
        genCodeOperator(wrt);
        return;
      case Writeln:
        token = nextToken();
        genCodeOperator(wrl);
        return;
      case End: case Semicolon:
        return;
      default:
        token = nextToken();
        continue;
    }
  }
}

void expression()
{
  KeyId k;
  k = token.kind;
  if (k == Plus || k == Minus){
    token = nextToken();
    term();
    if (k == Minus) genCodeOperator(neg);
  } else {
    term();
  }

  k = token.kind;
  while (k == Plus || k == Minus){
    token = nextToken();
    term();
    if (k == Minus) genCodeOperator(sub);
    else genCodeOperator(add);
    k = token.kind;
  }
}

void term()
{
  KeyId k;
  factor();
  k = token.kind;
  while (k == Mult || k == Div){
    token = nextToken();
    factor();
    if (k==Mult) genCodeOperator(mul);
    else genCodeOperator(div);
    k = token.kind;
  }
}

void factor()
{
  int tIndex, i;
  KindT k;
  if (token.kind == Id) {
    tIndex = searchT(token.u.id);
    k = kindT(tIndex);
    switch (k) {
      case varId: case parId:
        genCodeWithAddr(lod, tIndex);
        token = nextToken();
        break;
      case constId:
        genCodeWithValue(lit, val(tIndex));
        token = nextToken();
        break;
      case funcId:
        token = nextToken();
        i=0;
        token = nextToken();
        if (token.kind != Rparen) {
          for (; ; ) {
            expression();
            i++;
            if (token.kind == Comma) {
              token = nextToken();
              continue;
            }
            token = checkGet(token, Rparen);
            break;
          }
        } else token = nextToken();
        genCodeWithAddr(cal, tIndex);
        break;
      default:
        break;
    }
  } else if (token.kind == Num) {
    genCodeWithValue(lit, token.u.value);
    token = nextToken();
  } else if (token.kind == Lparen) {
    token = nextToken();
    expression();
    token = checkGet(token, Rparen);
  }
  switch (token.kind){
    case Id: case Num: case Lparen:
      factor();
    default:
      return;
  }
}

void condition()
{
  KeyId k;
  if (token.kind == Odd) {
    token = nextToken();
    expression();
    genCodeOperator(odd);
  } else {
    expression();
    k = token.kind;
    token = nextToken();
    expression();
    switch (k) {
      case Equal:	genCodeOperator(eq);   break;
      case Lss:		genCodeOperator(ls);   break;
      case Gtr:		genCodeOperator(gr);   break;
      case NotEq:	genCodeOperator(neq);  break;
      case LssEq:	genCodeOperator(lseq); break;
      case GtrEq:	genCodeOperator(greq); break;
      default:                             break;
    }
  }
}

int isStBeginKey(Token t)
{
  switch(t.kind) {
    case If: case Begin: case Ret: case While: case Write: case Writeln:
      return 1;
    default: return 0;
  }
}
