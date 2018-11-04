typedef enum codes {
  lit, opr, lod, sto, cal, ret, ict, jmp, jpc
} OpCode;

typedef enum ops {
  neg, add, sub, mul, div, odd, eq, ls, gr, neq, lseq, greq, wrt, wrl
} Operator;

int genCodeWithValue(OpCode op, int value);
int genCodeWithAddr(OpCode op, int tableIndex);
int genCodeOperator(Operator operator);
int genCodeReturn();
void backPatch(int index);
int nextCode();
void listCode();
void execute();
