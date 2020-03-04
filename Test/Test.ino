#define BUFFERSIZE 15
static bool flag = true;
char* p;

void setup() {
  Serial.begin(9600);
  p = returnArray();
}

char* returnArray() {
  static char testArray[BUFFERSIZE] = "sebastiaan";
  return testArray;
}

char* returnArray2() {
  static char testArray[BUFFERSIZE] = "frans";
  return testArray;
}

void clearBuffer(char* p) {
  memset(p, 0, BUFFERSIZE);
}

void loop() {
  for (int i = 0; i < BUFFERSIZE; i++) {
    if (*(p+i)!= NULL) {
    Serial.print(*(p + i));
    }
  }
  clearBuffer(p);
}
