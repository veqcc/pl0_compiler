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
static int spaces;
static int CR;
static int printed;
static void printSpaces();
static void printcToken();

struct keyWd {
  char *word;
  KeyId keyId;
};

static struct keyWd KeyWdT[] = {
  {"begin", Begin},
	{"end", End},
	{"var", Var},
	{"const", Const},
	{"write", Write},
	{"$dummy1",end_of_KeyWd},
	{"=", Equal},
	{".", Period},
	{";", Semicolon},
	{":=", Assign},
	{"$dummy2",end_of_KeySym}
};

static KeyId charClassT[256];

static void initCharClassT()
{
  printf("initCharClassT\n");
  int i;
	for (i=0; i<256; i++) charClassT[i] = others;
	for (i='0'; i<='9'; i++) charClassT[i] = digit;
	for (i='A'; i<='Z'; i++) charClassT[i] = letter;
	for (i='a'; i<='z'; i++) charClassT[i] = letter;

	charClassT['='] = Equal;
	charClassT['.'] = Period;
  charClassT[';'] = Semicolon;
	charClassT[':'] = colon;
}

void openSource(char fileName[])
{
  printf("openSource\n");
  fpi = fopen(fileName, "r");
}

void closeSource()
{
  printf("closeSource\n");
  fclose(fpi);
}

void initSource()
{
  printf("initSource\n");
  lineIndex = -1;
  ch = '\n';
  printed = 1;
  initCharClassT();
}

void finalSource()
{
  printf("finalSource\n");
  if (cToken.kind==Period) printcToken();
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
  printcToken();
  spaces = 0;
  CR = 0;

  while (1) {
    if (ch == ' ') {
      spaces++;
    } else if (ch == '\t') {
      spaces += TAB;
    } else if (ch == '\n') {
      spaces = 0; CR++;
    } else {
      break;
    }
    ch = nextChar();
  }

  switch (cc = charClassT[ch]) {
    case letter:
      do {
        ident[i] = ch; i++; ch = nextChar();
      } while (charClassT[ch] == letter || charClassT[ch] == digit);
      ident[i] = '\0';
      for (i=0; i<end_of_KeyWd; i++) {
        if (strcmp(ident, KeyWdT[i].word) == 0) {
          next_token.kind = KeyWdT[i].keyId;
          cToken = next_token;
          printed = 0;
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
      } while (charClassT[ch] == digit);
      next_token.kind = num;
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
    default:
      next_token.kind = cc;
      ch = nextChar();
      break;
  }
  cToken = next_token;
  printed = 0;
  return next_token;
}

Token checkGet(Token t, KeyId k)
{
  if (t.kind == k) return(nextToken());
  else return(t);
}

static void printSpaces()
{
  CR = 0;
  spaces = 0;
}

void printcToken()
{
  int i = (int)cToken.kind;
  if (printed) {
    printed = 0;
    return;
  } else {
    printed = 1;
    printSpaces();
  }
}
