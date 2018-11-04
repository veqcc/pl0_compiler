#include <stdio.h>
#include <string.h>
#include "getSource.h"

#define MAXLINE 120
#define TAB 5

static FILE *fpi;
static char line[MAXLINE];
static int lineIndex;
static char ch;
static Token cToken;

struct keyWd {
  char *word;
  KeyId keyId;
};

static struct keyWd KeyWdT[] = {
  {"begin", Begin},
  {"end", End},
  {"if", If},
  {"then", Then},
  {"while", While},
  {"do", Do},
  {"return", Ret},
  {"function", Func},
  {"var", Var},
  {"const", Const},
  {"odd", Odd},
  {"write", Write},
  {"writeln", Writeln},
  {"$dummy1", end_of_KeyWd},
  {"+", Plus},
  {"-", Minus},
  {"*", Mult},
  {"/", Div},
  {"(", Lparen},
  {")", Rparen},
  {"=", Equal},
  {"<", Lss},
  {">", Gtr},
  {"<>", NotEq},
  {"<=", LssEq},
  {">=", GtrEq},
  {",", Comma},
  {".", Period},
  {";", Semicolon},
  {":=", Assign},
  {"$dummy2", end_of_KeySym}
};

static KeyId charClassT[256];

static void initCharClassT()
{
  int i;
  for (i=0; i<256; i++)    charClassT[i] = others;
  for (i='0'; i<='9'; i++) charClassT[i] = digit;
  for (i='A'; i<='Z'; i++) charClassT[i] = letter;
  for (i='a'; i<='z'; i++) charClassT[i] = letter;

  charClassT['+'] = Plus;    charClassT['-'] = Minus;
  charClassT['*'] = Mult;    charClassT['/'] = Div;
  charClassT['('] = Lparen;  charClassT[')'] = Rparen;
  charClassT['='] = Equal;   charClassT['<'] = Lss;
  charClassT['>'] = Gtr;     charClassT[','] = Comma;
  charClassT['.'] = Period;  charClassT[';'] = Semicolon;
  charClassT[':'] = colon;
}

void openSource(char fileName[])
{
  fpi = fopen(fileName, "r");
}

void closeSource()
{
  fclose(fpi);
}

void initSource()
{
  lineIndex = -1;
  ch = '\n';
  initCharClassT();
}

char nextChar()
{
  char ch;

  if (lineIndex == -1) {
    fgets(line, MAXLINE, fpi);
    lineIndex = 0;
  }

  ch = line[lineIndex++];

  if (ch == '\n') {
    lineIndex = -1;
    return '\n';
  } else {
    return ch;
  }
}

Token nextToken()
{
  int i = 0;
  int num;
  KeyId cc;
  Token next_token;
  char ident[MAXNAME];

  while (1) {
    if (ch == ' ' || ch == '\t' || ch == '\n') ch = nextChar();
    else break;
  }

  switch (cc = charClassT[(int)ch]) {
    case letter:
      do {
        ident[i] = ch; i++; ch = nextChar();
      } while (charClassT[(int)ch] == letter || charClassT[(int)ch] == digit);
      ident[i] = '\0';
      for (i=0; i<end_of_KeyWd; i++) {
        if (strcmp(ident, KeyWdT[i].word) == 0) {
          next_token.kind = KeyWdT[i].keyId;
          cToken = next_token;
          return next_token;
        }
      }
      next_token.kind = Id;
      strcpy(next_token.u.id, ident);
      break;
    case digit:
      num = 0;
      do {
        num = 10 * num + (ch - '0');
        i++; ch = nextChar();
      } while (charClassT[(int)ch] == digit);
      next_token.kind = Num;
      next_token.u.value = num;
      break;
    case colon:
      ch = nextChar();
      if (ch == '=') {
        ch = nextChar();
        next_token.kind = Assign;
      } else {
        next_token.kind = nul;
      }
      break;
    case Lss:
      ch = nextChar();
      if (ch == '=') {
        ch = nextChar();
        next_token.kind = LssEq;
      } else if (ch == '>') {
        ch = nextChar();
        next_token.kind = NotEq;
      } else {
        next_token.kind = Lss;
      }
      break;
    case Gtr:
      ch = nextChar();
      if (ch == '=') {
        ch = nextChar();
        next_token.kind = GtrEq;
      } else {
        next_token.kind = Gtr;
      }
      break;
    default:
      next_token.kind = cc;
      ch = nextChar();
      break;
  }
  cToken = next_token;
  return next_token;
}

Token checkGet(Token t, KeyId k)
{
  if (t.kind == k) return(nextToken());
  else return(t);
}
