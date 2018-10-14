typedef enum kindT {
  varId, parId, constId
} KindT;

typedef struct relAddr {
  int level;
  int addr;
} RelAddr;

void blockBegin(int firstAddr);
void blockEnd();
int bLevel();
int fPars();
int enterTvar(char *id);
int enterTconst(char *id, int v);
void changeV(int ti, int newVal);
int searchT(char *id, KindT k);
KindT kindT(int i);
RelAddr relAddr(int ti);
int val(int ti);
int frameL();
