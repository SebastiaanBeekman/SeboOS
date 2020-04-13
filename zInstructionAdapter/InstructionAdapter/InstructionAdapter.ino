#define INSTRUCTIONSIZE 20
#define STORAGESIZE 1000

int executeInstruction(int PTIndex) {
  static byte file[STORAGESIZE] = {"DELAY"};
  byte *p = readEEPROM(getPTEntry(PTIndex).Name);
  char instructionChar;
  int counter = 0;
  int returnVal;

  do {
//    file[counter] = *(p + counter);
    instructionChar = (char)file[counter++];
    returnVal = instructionAdapter(instructionChar);
  } while (returnVal == -1);
  counter = 0;
  return returnVal;
}
int instructionAdapter(char instruction) {
  static char instructionArray[INSTRUCTIONSIZE];
  static int counter = 0;
  instructionArray[counter++] = instruction;
  if (strcmp(instructionArray, "CHAR") == 0) return 1;
  else if (strcmp(instructionArray, "INT") == 0) return 2;
  else if (strcmp(instructionArray, "STRING") == 0) return 3;
  else if (strcmp(instructionArray, "FLOAT") == 0) return 4;
  else if (strcmp(instructionArray, "SET") == 0) return 5;
  else if (strcmp(instructionArray, "GET") == 0) return 6;
  else if (strcmp(instructionArray, "INCREMENT") == 0) return 7;
  else if (strcmp(instructionArray, "DECREMENT") == 0) return 8;
  else if (strcmp(instructionArray, "PLUS") == 0) return 9;
  else if (strcmp(instructionArray, "MINUS") == 0) return 10;
  else if (strcmp(instructionArray, "TIMES") == 0) return 11;
  else if (strcmp(instructionArray, "DIVIDEDBY") == 0) return 12;
  else if (strcmp(instructionArray, "MODULUS") == 0) return 13;
  else if (strcmp(instructionArray, "UNARYMINUS") == 0) return 14;
  else if (strcmp(instructionArray, "EQUALS") == 0) return 15;
  else if (strcmp(instructionArray, "NOTEQUALS") == 0) return 16;
  else if (strcmp(instructionArray, "LESSTHAN") == 0) return 17;
  else if (strcmp(instructionArray, "LESSTHANOREQUALS") == 0) return 18;
  else if (strcmp(instructionArray, "GREATERTHAN") == 0) return 19;
  else if (strcmp(instructionArray, "GREATERTHANOREQUALS") == 0) return 20;
  else if (strcmp(instructionArray, "LOGICALAND") == 0) return 21;
  else if (strcmp(instructionArray, "LOGICALOR") == 0) return 22;
  else if (strcmp(instructionArray, "LOGICALXOR") == 0) return 23;
  else if (strcmp(instructionArray, "LOGICALNOT") == 0) return 24;
  else if (strcmp(instructionArray, "BITWISEAND") == 0) return 25;
  else if (strcmp(instructionArray, "BITWISEOR") == 0) return 26;
  else if (strcmp(instructionArray, "BITWISEXOR") == 0) return 27;
  else if (strcmp(instructionArray, "BITWISENOT") == 0) return 28;
  else if (strcmp(instructionArray, "TOCHAR") == 0) return 29;
  else if (strcmp(instructionArray, "TOINT") == 0) return 30;
  else if (strcmp(instructionArray, "TOFLOAT") == 0) return 31;
  else if (strcmp(instructionArray, "ROUND") == 0) return 32;
  else if (strcmp(instructionArray, "FLOOR") == 0) return 33;
  else if (strcmp(instructionArray, "CEIL") == 0) return 34;
  else if (strcmp(instructionArray, "MIN") == 0) return 35;
  else if (strcmp(instructionArray, "MAX") == 0) return 36;
  else if (strcmp(instructionArray, "ABS") == 0) return 37;
  else if (strcmp(instructionArray, "CONSTRAIN") == 0) return 38;
  else if (strcmp(instructionArray, "MAP") == 0) return 39;
  else if (strcmp(instructionArray, "POW") == 0) return 40;
  else if (strcmp(instructionArray, "SQ") == 0) return 41;
  else if (strcmp(instructionArray, "SQRT") == 0) return 42;
  else if (strcmp(instructionArray, "DELAY") == 0) return 43;
  else if (strcmp(instructionArray, "DELAYUNTIL") == 0) return 44;
  else if (strcmp(instructionArray, "MILLIS") == 0) return 45;
  else if (strcmp(instructionArray, "PINMODE") == 0) return 46;
  else if (strcmp(instructionArray, "ANALOGREAD") == 0) return 47;
  else if (strcmp(instructionArray, "ANALOGWRITE") == 0) return 48;
  else if (strcmp(instructionArray, "DIGITALREAD") == 0) return 49;
  else if (strcmp(instructionArray, "DIGITALWRITE") == 0) return 50;
  else if (strcmp(instructionArray, "PRINT") == 0) return 51;
  else if (strcmp(instructionArray, "PRINT") == 0) return 52;
  else if (strcmp(instructionArray, "OPEN") == 0) return 53;
  else if (strcmp(instructionArray, "CLOSE") == 0) return 54;
  else if (strcmp(instructionArray, "WRITE") == 0) return 55;
  else if (strcmp(instructionArray, "READINT") == 0) return 56;
  else if (strcmp(instructionArray, "READCHAR") == 0) return 57;
  else if (strcmp(instructionArray, "READFLOAT") == 0) return 58;
  else if (strcmp(instructionArray, "READSTRING") == 0) return 59;
  else if (strcmp(instructionArray, "IF") == 0) return 128;
  else if (strcmp(instructionArray, "ELSE") == 0) return 129;
  else if (strcmp(instructionArray, "ENDIF") == 0) return 130;
  else if (strcmp(instructionArray, "WHILE") == 0) return 131;
  else if (strcmp(instructionArray, "ENDWHILE") == 0) return 132;
  else if (strcmp(instructionArray, "LOOP") == 0) return 133;
  else if (strcmp(instructionArray, "ENDLOOP") == 0) return 134;
  else if (strcmp(instructionArray, "STOP") == 0) return 135;
  else if (strcmp(instructionArray, "FORK") == 0) return 136;
  else if (strcmp(instructionArray, "WAITUNTILDONE") == 0) return 137;
  else return -1;
}
