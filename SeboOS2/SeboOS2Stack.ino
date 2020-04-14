#define STACKSIZE 32
#define STACKAMOUNT 10
#define ARRAYSIZE 12
static byte stack[STACKAMOUNT][STACKSIZE];

void pushByte(byte b, int processID, int sp) {
  stack[processID][sp] = b;
//  Serial.println(stack[processID][sp]);
}

byte popByte(int processID, int sp) {
  return stack[processID][sp];
}
/*-----------------------------------------------------------------*/
int pushInt(int input, int processID, int sp) {
  byte *p = numToByte(input);
  byte byteArray[2] = {*(p), *(p + 1)};
  for (int i = 0; i < sizeof(int); i++) pushByte(byteArray[i], processID, sp++);
  pushByte('i', processID, sp);
  return sp;
}

int pushFloat(float input, int processID, int sp) {
  byte *p = numToByte(input);
  byte byteArray[4] = {*(p), *(p + 1), *(p + 2), *(p + 3)};
  for (int i = 0; i < sizeof(float); i++) pushByte(byteArray[i], processID, sp++);
  pushByte('f', processID, sp);
  return sp;
}
/*-----------------------------------------------------------------*/
byte* getByteArray(int n, int processID, int sp) {
  static byte byteArray[4];
  for (int i = 1; i <= n; i++) byteArray[n - i] = popByte(processID, sp--);
  return byteArray;
}

int popInt(int processID, int sp) {
  byte *p = getByteArray(sizeof(int), processID, sp);
  byte byteArray[2] = {*(p), *(p + 1)};
  return byteToInt(byteArray);
}

int32_t popFloat(int processID, int sp) {
  byte *p = getByteArray(sizeof(float), processID, sp);
  byte byteArray[4] = {*(p), *(p + 1), *(p + 2), *(p + 3)};
  return byteToFloat(byteArray);
}
/*-----------------------------------------------------------------*/
int pushString(char input[], int Size, int processID, int sp) {
  byte *p = strToByte(input, Size);
  byte byteArray[Size];
  for (int i = 0; i < Size; i++) {
    byteArray[i] = *(p + i);
    pushByte(byteArray[i], processID, sp++);
  }
  pushByte(Size, processID, sp++);
  pushByte('s', processID, sp);
  return sp;
}

char* popString(int processID, int sp) {
  popByte(processID, sp--);                                                                    //Pop type, not important
  int Size = popByte(processID, sp--);
  byte byteArray[Size];
  for (int i = 1; i <= Size; i++) byteArray[Size-i] = popByte(processID, sp--);
  return byteToStr(byteArray, Size);
}
