#include "instruction_set.h"
#define PTSIZE 10
#define STRINGSIZE 30

void runProcesses() {
  PT *p = getPTArray();
  for (int i = 0; i < PTSIZE; i++) {
    if (strcmp((*(p + i)).Name, "") != 0 && (*(p + i)).state == 1) {                                 //Check if element isn't empty && it's running
      executeInstruction(i);
      execute(i);
    }
  }
}

void charInstruction(int PTIndex) {
  PT entry = getPTEntry(PTIndex);
  int sp = entry.Registers[2];
  int loopAddr = entry.Registers[3];

  loopAddr++;                                                                                         //Open quote
  loopAddr++;                                                                                         //Char
  byte b = readEEPROM(entry.Name, loopAddr++);                                                        //Close quote
  loopAddr++;                                                                                         //Comma
  loopAddr++;                                                                                         //Next instruction
  pushByte(b, entry.ID, ++sp);
  pushByte('c', entry.ID, ++sp);
  updateInstructionRegisters(PTIndex, sp, loopAddr);
}

void intInstruction(int PTIndex) {
  PT entry = getPTEntry(PTIndex);
  int sp = entry.Registers[2];
  int loopAddr = entry.Registers[3];

  int num1, num2, num3;

  loopAddr++;                                                                                        //Comma
  num1 = readEEPROM(entry.Name, loopAddr++) - 48;
  loopAddr++;                                                                                        //Comma
  num2 = readEEPROM(entry.Name, loopAddr++) - 48;
  loopAddr++;
  pushByte(num1, entry.ID, ++sp);
  pushByte(num2, entry.ID, ++sp);
  pushByte('i', entry.ID, ++sp);
  updateInstructionRegisters(PTIndex, sp, loopAddr);
}

void stringInstruction(int PTIndex) {
  PT entry = getPTEntry(PTIndex);
  int sp = entry.Registers[2];
  int loopAddr = entry.Registers[3];

  char stringArray[STRINGSIZE] = {};
  int loopCounter = 0;
  byte b;

  do {
    b = readEEPROM(entry.Name, loopAddr++);                                                             //Close quote
    if (b != 39 && b != 44) {
      if (b == 92) {
        stringArray[loopCounter++] = 10;  //Change \n to newline byte & skip the 'n' (data is saved as a '\' & 'n')
        loopAddr++;
      }
      else stringArray[loopCounter++] = b;
      pushByte(b, entry.ID, ++sp);
    }
  } while (b != 48);
  loopAddr++;                                                                                         //Next instruction
  pushByte(loopCounter, entry.ID, ++sp);
  pushByte('s', entry.ID, ++sp);
  updateInstructionRegisters(PTIndex, sp, loopAddr);
}

void printInstruction(int PTIndex) {
  PT entry = getPTEntry(PTIndex);
  int sp = entry.Registers[2];
  int loopAddr = entry.Registers[3];
  char type;
  byte ba[2] = {};

  int printSize = getSize(PTIndex, sp--);
  if (printSize == 1) type = 'c';
  else if (printSize == 2) type = 'i';
  else if (printSize == 4) type = 'f';
  else {
    type = 's';
    sp = sp - 2;
  }

  for (int i = printSize; i > 0; i--) {
    byte b = popByte(PTIndex, sp - i);
    if (type == 'c' || type == 's')Serial.print((char)b);
    else if (type == 'i') ba[printSize - i] = b;
    else if (type == 'f') Serial.print((float)b);
  }
  if (type == 'i') Serial.println((ba[0] * 256) + ba[1]);
  else Serial.println();
  loopAddr++;
  updateInstructionRegisters(PTIndex, sp - printSize, loopAddr);
}

void stopInstruction(int PTIndex) {
  PRK(PTIndex, -1);
}

void setInstruction(int PTIndex) {
  PT entry = getPTEntry(PTIndex);
  int sp = entry.Registers[2];
  int loopAddr = entry.Registers[3];

  loopAddr++;                                                                                         //Open quote
  loopAddr++;                                                                                         //Char
  byte b = readEEPROM(entry.Name, loopAddr++);                                                        //Close quote
  loopAddr++;                                                                                         //Comma
  loopAddr++;                                                                                         //Next instruction

  int newSP = writeMemory(b, PTIndex, sp);
  updateInstructionRegisters(PTIndex, newSP, loopAddr);
}

void getInstruction(int PTIndex) {
  PT entry = getPTEntry(PTIndex);
  int sp = entry.Registers[2];
  int loopAddr = entry.Registers[3];

  loopAddr++;                                                                                         //Open quote
  loopAddr++;                                                                                         //Char
  byte b = readEEPROM(entry.Name, loopAddr++);                                                        //Close quote
  loopAddr++;                                                                                         //Comma
  loopAddr++;                                                                                         //Next instruction

  int newSP = readMemory(b, PTIndex, sp);
  updateInstructionRegisters(PTIndex, newSP, loopAddr);
}

void unairInstructions(int PTIndex) {
  PT entry = getPTEntry(PTIndex);
  int pc = entry.Registers[0];
  int sp = entry.Registers[2];
  int loopAddr = entry.Registers[3];

  int Size = getSize(PTIndex, sp--);
  char type;
  if (Size == 1) type = 'c';
  else if (Size == 2) type = 'i';
  else if (Size == 4) type = 'f';
  else type = 's';

  if (Size > 4) sp--;                                                                               //lower sp for pop size in getSize
  byte byteArray[Size] = {};
  for (int i = 1; i <= Size; i++) {
    byteArray[Size - i] = popByte(PTIndex, sp--);
  }

  switch (pc) {
    case INCREMENT:
      if (Size == 1) {                                                                               //Char
        byteArray[0]++;
      } else if (Size == 2) {                                                                        //Int
        if (byteArray[0] + 1 >= 256) {
          byteArray[0]++;
          byteArray[1] -= byteArray[0];
        } else {
          byteArray[1]++;
        }
      }
      break;
    case DECREMENT:
      if (Size == 1) {                                                                               //Char
        byteArray[0]--;
      } else if (Size == 2) {                                                                        //Int
        if (byteArray[0] - 1 <= 0) {
          byteArray[0]--;
          byteArray[1] = 256;
        } else {
          byteArray[1]--;
        }
      }
      break;
  }

  for (int i = 0; i < Size; i++) {
    pushByte(byteArray[i], PTIndex, sp++);
  }
  pushByte(type, PTIndex, sp++);
  loopAddr++;
  updateInstructionRegisters(PTIndex, sp, loopAddr);
}

void execute(int PTIndex) {
  PT entry = getPTEntry(PTIndex);
  switch (entry.Registers[0]) {
    case CHAR:
      charInstruction(PTIndex);
      break;
    case INT:
      intInstruction(PTIndex);
      break;
    case STRING:
      stringInstruction(PTIndex);
      break;
    case FLOAT:                                                                             //Bytecode???
      Serial.println("FLOAT type");
      break;
    case SET:
      setInstruction(PTIndex);
      break;
    case GET:
      getInstruction(PTIndex);
      break;
    case INCREMENT:
      unairInstructions(PTIndex);
      break;
    case DECREMENT:
      unairInstructions(PTIndex);
      break;
    case PLUS:
      Serial.println("PLUS type");
      break;
    case MINUS:
      Serial.println("MINUS type");
      break;
    case TIMES:
      Serial.println("TIMES type");
      break;
    case DIVIDEDBY:
      Serial.println("DIVIDEDBY type");
      break;
    case MODULUS:
      Serial.println("MODULUS type");
      break;
    case UNARYMINUS:
      Serial.println("UNARYMINUS type");
      break;
    case EQUALS:
      Serial.println("EQUALS type");
      break;
    case NOTEQUALS:
      Serial.println("NOTEQUALS type");
      break;
    case LESSTHAN:
      Serial.println("LESSTHAN type");
      break;
    case LESSTHANOREQUALS:
      Serial.println("LESSTHANOREQUALS type");
      break;
    case GREATERTHAN:
      Serial.println("GREATERTHAN type");
      break;
    case GREATERTHANOREQUALS:
      Serial.println("GREATERTHANOREQUALS type");
      break;
    case LOGICALAND:
      Serial.println("LOGICALAND type");
      break;
    case LOGICALOR:
      Serial.println("LOGICALOR type");
      break;
    case LOGICALXOR:
      Serial.println("LOGICALXOR type");
      break;
    case LOGICALNOT:
      Serial.println("LOGICALNOT type");
      break;
    case BITWISEAND:
      Serial.println("BITWISEAND type");
      break;
    case BITWISEOR:
      Serial.println("BITWISEOR type");
      break;
    case BITWISEXOR:
      Serial.println("BITWISEXOR type");
      break;
    case BITWISENOT:
      Serial.println("BITWISENOT type");
      break;
    case TOCHAR:
      Serial.println("TOCHAR type");
      break;
    case TOINT:
      Serial.println("TOINT type");
      break;
    case TOFLOAT:
      Serial.println("TOFLOAT type");
      break;
    case ROUND:
      Serial.println("ROUND type");
      break;
    case FLOOR:
      Serial.println("FLOOR type");
      break;
    case CEIL:
      Serial.println("CEIL type");
      break;
    case MIN:
      Serial.println("MIN type");
      break;
    case MAX:
      Serial.println("MAX type");
      break;
    case ABS:
      Serial.println("ABS type");
      break;
    case CONSTRAIN:
      Serial.println("CONSTRAIN type");
      break;
    case MAP:
      Serial.println("MAP type");
      break;
    case SQ:
      Serial.println("SQ type");
      break;
    case SQRT:
      Serial.println("SQRT type");
      break;
    case DELAY:
      Serial.println("DELAY type");
      break;
    case DELAYUNTIL:
      Serial.println("DELAYUNTIL type");
      break;
    case MILLIS:
      Serial.println("MILLIS type");
      break;
    case PINMODE:
      Serial.println("PINMODE type");
      break;
    case ANALOGREAD:
      Serial.println("ANALOGREAD type");
      break;
    case ANALOGWRITE:
      Serial.println("ANALOGWRITE type");
      break;
    case DIGITALREAD:
      Serial.println("DIGITALREAD type");
      break;
    case DIGITALWRITE:
      Serial.println("DIGITALWRITE type");
      break;
    case PRINT:
      printInstruction(PTIndex);
      break;
    case OPEN:
      Serial.println("OPEN type");
      break;
    case CLOSE:
      Serial.println("CLOSE type");
      break;
    case WRITE:
      Serial.println("WRITE type");
      break;
    case READINT:
      Serial.println("READINT type");
      break;
    case READCHAR:
      Serial.println("READCHAR type");
      break;
    case READFLOAT:
      Serial.println("READFLOAT type");
      break;
    case READSTRING:
      Serial.println("READSTRING type");
      break;
    case IF:
      Serial.println("IF type");
      break;
    case ELSE:
      Serial.println("ELSE type");
      break;
    case ENDIF:
      Serial.println("ENDIF type");
      break;
    case WHILE:
      Serial.println("WHILE type");
      break;
    case ENDWHILE:
      Serial.println("ENDWHILE type");
      break;
    case LOOP:
      Serial.println("LOOP type");
      break;
    case ENDLOOP:
      Serial.println("ENDLOOP type");
      break;
    case STOP:
      stopInstruction(PTIndex);
      break;
    case FORK:
      Serial.println("FORK type");
      break;
    case WAITUNTILDONE:
      Serial.println("WAITUNTILDONE type");
      break;
  }
  if (readFATEntry(entry.Name).Size < getPTEntry(PTIndex).Registers[3]) updateLoopAddr(PTIndex, 0);
  //  if (entry.Registers[0] != -1) {
  //    Serial.println(getPTEntry(PTIndex).Registers[2]);
  //    Serial.println(getPTEntry(PTIndex).Registers[3]);
  //  }
}
