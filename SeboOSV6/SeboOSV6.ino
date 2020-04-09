#define ARRAYSIZE 12

static bool flag = true;

union {                                                                         //Numbers to bytes and back
  int32_t num;
  byte bval[4];
} numAsBytes;

void setup() {
  Serial.begin(9600);
#ifdef DEBUG
  Serial.println("Welcome to SeboOSV6, Debug mode.");
#else
  Serial.println("Welcome to SeboOSV6, your madly Operating System.");
#endif
}
/*-----------------------------------------------------------------*/
byte* numToByte(int32_t input) {
  static byte byteArray[ARRAYSIZE];
  memset(byteArray, NULL, ARRAYSIZE);
  numAsBytes.num = input;
  strcpy(byteArray, numAsBytes.bval);
  return byteArray;
}

int byteToInt(byte input[]) {
  strcpy(numAsBytes.bval, input);
  return numAsBytes.num;
}

float byteToFloat(byte input[]) {
  strcpy(numAsBytes.bval, input);
  return numAsBytes.num;
}

byte* strToByte(char input[], int inputSize) {                                  //String (char array) to bytes
  static byte byteArray[ARRAYSIZE];
  memset(byteArray, NULL, ARRAYSIZE);                                           //Clear old 'string' before using
  strcpy(byteArray, input);
  return byteArray;
}

char* byteToStr(byte input[], int inputSize) {                                  //Byte array to string (char array pointer)
  static char charArray[ARRAYSIZE];
  memset(charArray, NULL, ARRAYSIZE);                                           //Clear old 'string' before using
  strcpy(charArray, input);
  return charArray;
}
/*-----------------------------------------------------------------*/
void loop() {
  if (flag) {
    char temp[] = {'t','e','s','t','\0'};
    pushString(temp, sizeof(temp), 0);
    char *p = popString(6);
    for (int i = 0; i < 6; i++) Serial.print(*(p+i));
  } flag = false;
}
