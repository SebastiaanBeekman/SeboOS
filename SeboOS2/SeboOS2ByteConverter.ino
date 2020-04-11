#define ARRAYSIZE 12

union {                                                                         //Numbers to bytes and back
  int32_t num;
  byte bval[4];
} numAsBytes;

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
