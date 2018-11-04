typedef enum kindT {
  varId, funcId, parId, constId
} KindT;

typedef struct relativeAddr {
  int level;
  int addr;
} RelativeAddr;

void blockBegin(int firstAddr);
void blockEnd();
int currentBlockLevel();
int functionParamCount();
int enterTfunc(char *id, int v);
int enterTvar(char *id);
int enterTpar(char *id);
int enterTconst(char *id, int v);
void endFunctionParam();
void changeV(int tableIndex, int newVal);
int searchT(char *id, KindT k);
KindT kindT(int i);
RelativeAddr relAddr(int tableIndex);
int val(int tableIndex);
int LocalAddr();
