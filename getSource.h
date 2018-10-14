#include <stdio.h>

#ifndef TBL
#define TBL
#include "table.h"
#endif

#define MAXNAME 31

typedef  enum  keys {
	Begin, End, Var, Const, Write, end_of_KeyWd,
	Equal, Period, Semicolon, Assign, end_of_KeySym,
	Id, Num, nul, end_of_Token,
	letter, digit, colon, others
} KeyId;

typedef struct token {
	KeyId kind;
	union {
		char id[MAXNAME];
		int value;
	} u;
} Token;

Token nextToken();
Token checkGet(Token t, KeyId k);

void openSource(char fileName[]);
void closeSource();
void initSource();
void finalSource();
