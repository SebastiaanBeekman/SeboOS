#define STACKSIZE 32
static byte stack[STACKSIZE];
static byte sp = 0;

void pushByte(byte b) { stack[sp++] = b; }

byte popByte() { return stack[sp--]; }
