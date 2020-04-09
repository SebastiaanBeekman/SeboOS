#define STACKSIZE 32
#define ARRAYSIZE 12
static byte stack[STACKSIZE];

void pushByte(byte b, int sp) {
  stack[sp] = b;
}

byte popByte(int sp) {
  return stack[sp];
}
/*-----------------------------------------------------------------*/
void pushInt(int input, int sp) {
  byte *p = numToByte(input);
  byte byteArray[2] = {*(p), *(p+1)};
  for (int i = 0; i < sizeof(int); i++) pushByte(byteArray[i], sp++);
  pushByte('i', sp);
}

void pushFloat(float input, int sp) {
  byte *p = numToByte(input);
  byte byteArray[4] = {*(p), *(p+1), *(p+2), *(p+3)};
  for (int i = 0; i < sizeof(float); i++) pushByte(byteArray[i], sp++);
  pushByte('f', sp);
}
/*-----------------------------------------------------------------*/
byte* getByteArray(int n, int sp) {
  static byte byteArray[4];
  for (int i = 1; i <= n; i++) byteArray[n-i] = popByte(sp--);
  return byteArray;
}

int popInt(int sp) {
  byte *p = getByteArray(sizeof(int), sp);
  byte byteArray[2] = {*(p), *(p+1)};
  return byteToInt(byteArray);
}

int32_t popFloat(int sp) {
  byte *p = getByteArray(sizeof(float), sp);
  byte byteArray[4] = {*(p), *(p+1), *(p+2), *(p+3)};
  return byteToFloat(byteArray);
}
/*-----------------------------------------------------------------*/
