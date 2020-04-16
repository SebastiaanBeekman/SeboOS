#define INSTRUCTIONSIZE 20
#define STORAGESIZE 1000

char* getCompareBuffer() {
  static char compareBuffer[INSTRUCTIONSIZE];
  return compareBuffer;
}

void clearCompareBuffer(char* p) {
  memset(p, 0, INSTRUCTIONSIZE);
}

void printBuffer() {
  char *p = getCompareBuffer();
  for (int i = 0; i < INSTRUCTIONSIZE; i++) Serial.println(*(p + i));
}

void executeInstruction(int PTIndex) {
  PT entry = getPTEntry(PTIndex);
  char *p = getCompareBuffer();
  int loopAddr = entry.Registers[3];
  int instruction;
  int i;
  for (i = loopAddr; i < readFATEntry(entry.Name).Size; i++) {
    loopAddr++;
    *(p + (i - entry.Registers[3])) = readEEPROM(entry.Name, i);
    instruction = instructionAdapter();
    if (readEEPROM(entry.Name, i + 1) != ',' && i < (readFATEntry(entry.Name).Size - 1)) instruction = -1;
    if (instruction != -1) break;
  }
  if (loopAddr >= (readFATEntry(entry.Name).Size)) {
    loopAddr = 0;
  }
  updateProcessRegisters(PTIndex, instruction, loopAddr++);                                             //Extra ++ to set it to the next byte (comma)
  clearCompareBuffer(getCompareBuffer());                                                                      
}

int instructionAdapter() {
  char instruction[INSTRUCTIONSIZE] = {};
  char *p = getCompareBuffer();
  for (int i = 0; i < INSTRUCTIONSIZE; i++) {
    instruction[i] = *(p + i);
//    Serial.print(instruction[i]);
  }
//  Serial.println();
  
  if (strcmp(instruction, "CHAR") == 0) return 1;
  else if (strcmp(instruction, "INT") == 0) return 2;
  else if (strcmp(instruction, "STRING") == 0) return 3;
  else if (strcmp(instruction, "FLOAT") == 0) return 4;
  else if (strcmp(instruction, "SET") == 0) return 5;
  else if (strcmp(instruction, "GET") == 0) return 6;
  else if (strcmp(instruction, "INCREMENT") == 0) return 7;
  else if (strcmp(instruction, "DECREMENT") == 0) return 8;
  else if (strcmp(instruction, "PLUS") == 0) return 9;
  else if (strcmp(instruction, "MINUS") == 0) return 10;
  else if (strcmp(instruction, "TIMES") == 0) return 11;
  else if (strcmp(instruction, "DIVIDEDBY") == 0) return 12;
  else if (strcmp(instruction, "MODULUS") == 0) return 13;
  else if (strcmp(instruction, "UNARYMINUS") == 0) return 14;
  else if (strcmp(instruction, "EQUALS") == 0) return 15;
  else if (strcmp(instruction, "NOTEQUALS") == 0) return 16;
  else if (strcmp(instruction, "LESSTHAN") == 0) return 17;
  else if (strcmp(instruction, "LESSTHANOREQUALS") == 0) return 18;
  else if (strcmp(instruction, "GREATERTHAN") == 0) return 19;
  else if (strcmp(instruction, "GREATERTHANOREQUALS") == 0) return 20;
  else if (strcmp(instruction, "LOGICALAND") == 0) return 21;
  else if (strcmp(instruction, "LOGICALOR") == 0) return 22;
  else if (strcmp(instruction, "LOGICALXOR") == 0) return 23;
  else if (strcmp(instruction, "LOGICALNOT") == 0) return 24;
  else if (strcmp(instruction, "BITWISEAND") == 0) return 25;
  else if (strcmp(instruction, "BITWISEOR") == 0) return 26;
  else if (strcmp(instruction, "BITWISEXOR") == 0) return 27;
  else if (strcmp(instruction, "BITWISENOT") == 0) return 28;
  else if (strcmp(instruction, "TOCHAR") == 0) return 29;
  else if (strcmp(instruction, "TOINT") == 0) return 30;
  else if (strcmp(instruction, "TOFLOAT") == 0) return 31;
  else if (strcmp(instruction, "ROUND") == 0) return 32;
  else if (strcmp(instruction, "FLOOR") == 0) return 33;
  else if (strcmp(instruction, "CEIL") == 0) return 34;
  else if (strcmp(instruction, "MIN") == 0) return 35;
  else if (strcmp(instruction, "MAX") == 0) return 36;
  else if (strcmp(instruction, "ABS") == 0) return 37;
  else if (strcmp(instruction, "CONSTRAIN") == 0) return 38;
  else if (strcmp(instruction, "MAP") == 0) return 39;
  else if (strcmp(instruction, "POW") == 0) return 40;
  else if (strcmp(instruction, "SQ") == 0) return 41;
  else if (strcmp(instruction, "SQRT") == 0) return 42;
  else if (strcmp(instruction, "DELAY") == 0) return 43;
  else if (strcmp(instruction, "DELAYUNTIL") == 0) return 44;
  else if (strcmp(instruction, "MILLIS") == 0) return 45;
  else if (strcmp(instruction, "PINMODE") == 0) return 46;
  else if (strcmp(instruction, "ANALOGREAD") == 0) return 47;
  else if (strcmp(instruction, "ANALOGWRITE") == 0) return 48;
  else if (strcmp(instruction, "DIGITALREAD") == 0) return 49;
  else if (strcmp(instruction, "DIGITALWRITE") == 0) return 50;
  else if (strcmp(instruction, "PRINT") == 0) return 51;
  else if (strcmp(instruction, "PRINTLN") == 0) return 52;
  else if (strcmp(instruction, "OPEN") == 0) return 53;
  else if (strcmp(instruction, "CLOSE") == 0) return 54;
  else if (strcmp(instruction, "WRITE") == 0) return 55;
  else if (strcmp(instruction, "READINT") == 0) return 56;
  else if (strcmp(instruction, "READCHAR") == 0) return 57;
  else if (strcmp(instruction, "READFLOAT") == 0) return 58;
  else if (strcmp(instruction, "READSTRING") == 0) return 59;
  else if (strcmp(instruction, "IF") == 0) return 128;
  else if (strcmp(instruction, "ELSE") == 0) return 129;
  else if (strcmp(instruction, "ENDIF") == 0) return 130;
  else if (strcmp(instruction, "WHILE") == 0) return 131;
  else if (strcmp(instruction, "ENDWHILE") == 0) return 132;
  else if (strcmp(instruction, "LOOP") == 0) return 133;
  else if (strcmp(instruction, "ENDLOOP") == 0) return 134;
  else if (strncmp(instruction, "STOP", 4) == 0) return 135;
  else if (strcmp(instruction, "FORK") == 0) return 136;
  else if (strcmp(instruction, "WAITUNTILDONE") == 0) return 137;
  else return -1;
}
