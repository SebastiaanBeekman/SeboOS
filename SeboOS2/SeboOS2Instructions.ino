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

  int num1, num2;
  int counter = 0;
  int numArray[3] = {};
  int numArray2[3] = {};

  loopAddr++;                                                                                        //Comma
  numArray[counter] = readEEPROM(entry.Name, loopAddr++) - 48;
  if (readEEPROM(entry.Name, loopAddr) != ',' && readEEPROM(entry.Name, loopAddr + 1) != ',') {
    numArray[++counter] = readEEPROM(entry.Name, loopAddr++) - 48;
    numArray[++counter] = readEEPROM(entry.Name, loopAddr++) - 48;
  }
  else if (readEEPROM(entry.Name, loopAddr) != ',') {
    numArray[++counter] = readEEPROM(entry.Name, loopAddr++) - 48;
  }
  num1 = calcInt(numArray, counter);
  counter = 0;
  loopAddr++;                                                                                        //Comma
  numArray2[counter] = readEEPROM(entry.Name, loopAddr++) - 48;
  if (readEEPROM(entry.Name, loopAddr) != ',' && readEEPROM(entry.Name, loopAddr + 1) != ',') {
    numArray2[++counter] = readEEPROM(entry.Name, loopAddr++) - 48;
    numArray2[++counter] = readEEPROM(entry.Name, loopAddr++) - 48;
  }
  else if (readEEPROM(entry.Name, loopAddr) != ',') {
    numArray2[++counter] = readEEPROM(entry.Name, loopAddr++) - 48;
  }
  num2 = calcInt(numArray2, counter);
  counter = 0;
  loopAddr++;                                                                                        //Next instruction
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
  int printSize;
  byte byteArray[2] = {};
  char type = popByte(PTIndex, sp--);
  if (type == 's') printSize = popByte(PTIndex, sp--) - 1;
  else if (type == 'c') printSize = 1;
  else if (type == 'i') printSize = 2;
  else if (type == 'f') printSize = 4;

  if (type == 's') {
    for (int i = printSize; i > 1; i--) {
      byte b = popByte(PTIndex, printSize - sp--);
      Serial.print((char)b);
    }
  } else {
    for (int i = printSize; i > 0; i--) {
      byte b = popByte(PTIndex, sp--);
      if (type == 'c') Serial.print((char)b);
      else if (type == 'i') byteArray[printSize - i] = b;
      else if (type == 'f') Serial.print((float)b);
    }
  }
  if (type == 'i') Serial.println((byteArray[1] * 256) + byteArray[0]);
  else Serial.println();
  loopAddr++;
  updateInstructionRegisters(PTIndex, sp - printSize, loopAddr);
}

void stopInstruction(int PTIndex) {
  PRK(PTIndex, -1);
  clearTable();
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

  int printSize;
  char type = popByte(PTIndex, sp--);
  if (type == 'c') printSize = 1;
  else if (type == 'i') printSize = 2;
  else if (type == 'f') printSize = 4;

  byte byteArray[printSize] = {};
  for (int i = 1; i <= printSize; i++) {
    byteArray[printSize - i] = popByte(PTIndex, sp--);
  }

  switch (pc) {
    case INCREMENT:
      if (printSize == 1) {                                                                               //Char
        byteArray[0]++;
      } else if (printSize == 2) {                                                                        //Int
        if (byteArray[0] + 1 >= 256) {
          byteArray[0]++;
          byteArray[1] -= byteArray[0];
        } else {
          byteArray[1]++;
        }
      }
      break;
    case DECREMENT:
      if (printSize == 1) byteArray[0]--;
      else if (printSize == 2) {                                                                        //Int
        if (byteArray[0] - 1 <= 0) {
          byteArray[0]--;
          byteArray[1] = 256;
        } else byteArray[1]--;
      }
      break;
    case UNARYMINUS:
      if (printSize == 1) byteArray[0] *= -1;
      else if (printSize == 2) {
        byteArray[0] *= -1;
        byteArray[1] *= -1;
      }
      break;
    case ABS:
      if (printSize == 1) byteArray[0] = abs(byteArray[0]);
      else if (printSize == 2) {
        byteArray[0] = abs(byteArray[0]);
        byteArray[1] = abs(byteArray[1]);
      }
      break;
    case SQ:
      if (printSize == 1) byteArray[0] *= byteArray[0];
      else if (printSize == 2) {
        int i = calcByteInt(byteArray[0], byteArray[1]);
        byte *p = calcIntByte(sq(i));
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      }
      break;
    case SQRT:
      if (printSize == 1) byteArray[0] /= byteArray[0];
      else if (printSize == 2) {
        int i = calcByteInt(byteArray[0], byteArray[1]);
        byte *p = calcIntByte(sqrt(i));
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      }
      break;
    case ANALOGREAD:
      if (printSize == 1) {
        byte *p = calcIntByte(analogRead(byteArray[0]));
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      }
      else if (printSize == 2) {
        int i = calcByteInt(byteArray[0], byteArray[1]);
        byte *p = calcIntByte(analogRead(i));
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      }
      type = 'i';
      break;
    case DIGITALREAD:
      if (printSize == 1) {
        byteArray[0] = digitalRead(byteArray[0]);
      }
      else if (printSize == 2) {
        int i = calcByteInt(byteArray[0], byteArray[1]);
        byteArray[0] = digitalRead(i);
      }
      type = 'c';
      break;
    case LOGICALNOT:
      if (printSize == 1) {
        if (byteArray[0] != 0) byteArray[0] = 0;
        else byteArray[0] = 1;
      }
      else if (printSize == 2) {
        int i = calcByteInt(byteArray[0], byteArray[1]);
        if (i != 0) byteArray[0] = 0;
        else byteArray[0] = 1;
      }
      type = 'c';
      break;
    case BITWISENOT:
      if (printSize == 1) {
        byteArray[0] = ~byteArray[0];
      }
      else if (printSize == 2) {
        int i = calcByteInt(byteArray[0], byteArray[1]);
        byteArray[0] = ~i;
      }
      break;
    case TOCHAR:
      if (printSize == 2) {
        char c = calcByteInt(byteArray[0], byteArray[1]);
        byteArray[0] = c;
      }
      break;
    case TOINT:
      if (printSize == 1) {
        char c = calcByteInt(byteArray[0], byteArray[1]);
        byte *p = calcIntByte(c);
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      }
      break;
    case ROUND:
      if (printSize == 1) {
        byteArray[0] = (int)round(byteArray[0]);
      }
      else if (printSize == 2) {
        int i = calcByteInt(byteArray[0], byteArray[1]);
        byteArray[0] = (int)round(i);
      }
      type = 'i';
      break;
    case FLOOR:
      if (printSize == 1) {
        byteArray[0] = (int)floor(byteArray[0]);
      }
      else if (printSize == 2) {
        int i = calcByteInt(byteArray[0], byteArray[1]);
        byteArray[0] = (int)floor(i);
      }
      type = 'i';
      break;
    case CEIL:
      if (printSize == 1) {
        byteArray[0] = (int)ceil(byteArray[0]);
      }
      else if (printSize == 2) {
        int i = calcByteInt(byteArray[0], byteArray[1]);
        byteArray[0] = (int)ceil(i);
      }
      type = 'i';
      break;
  }
  if (sp < 0) sp = 0;
  for (int i = 0; i < printSize; i++) {
    pushByte(byteArray[i], PTIndex, sp++);
  }
  pushByte(type, PTIndex, sp);
  loopAddr++;
  updateInstructionRegisters(PTIndex, sp, loopAddr);
}

void binairInstructions(int PTIndex) {
  PT entry = getPTEntry(PTIndex);
  int pc = entry.Registers[0];
  int sp = entry.Registers[2];
  int loopAddr = entry.Registers[3];
  bool customType = false;
  int finalSize;
  char finalType;

  int printSize;
  char type = popByte(PTIndex, sp--);
  if (type == 'c') printSize = 1;
  else if (type == 'i') printSize = 2;
  else if (type == 'f') printSize = 4;

  byte byteArray[printSize] = {};
  for (int i = 1; i <= printSize; i++) {
    byteArray[printSize - i] = popByte(PTIndex, sp--);
  }

  int printSize2;
  char type2 = popByte(PTIndex, sp--);
  if (type2 == 'c') printSize2 = 1;
  else if (type2 == 'i') printSize2 = 2;
  else if (type2 == 'f') printSize2 = 4;

  byte byteArray2[printSize2] = {};
  for (int i = 1; i <= printSize2; i++) {
    byteArray2[printSize2 - i] = popByte(PTIndex, sp--);
  }

  switch (pc) {
    case PLUS:
      if (printSize == 2 && printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        byte *p = calcIntByte(num1 + num2);
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      } else if (printSize == 2) {
        int num1 = byteArray2[0];
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        byte *p = calcIntByte(num1 + num2);
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      } else if (printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = byteArray[0];
        byte *p = calcIntByte(num1 + num2);
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      } else {
        byte *p = calcIntByte(byteArray2[0] + byteArray[0]);
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      }
      break;
    case MINUS:
      if (printSize == 2 && printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        byte *p = calcIntByte(num1 - num2);
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      } else if (printSize == 2) {
        int num1 = byteArray2[0];
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        byte *p = calcIntByte(num1 - num2);
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      } else if (printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = byteArray[0];
        byte *p = calcIntByte(num1 - num2);
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      } else {
        byte *p = calcIntByte(byteArray2[0] - byteArray[0]);
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      }
      break;
    case TIMES:
      if (printSize == 2 && printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        byte *p = calcIntByte(num1 * num2);
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      } else if (printSize == 2) {
        int num1 = byteArray2[0];
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        byte *p = calcIntByte(num1 * num2);
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      } else if (printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = byteArray[0];
        byte *p = calcIntByte(num1 * num2);
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      } else {
        byte *p = calcIntByte(byteArray2[0] * byteArray[0]);
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      }
      break;
    case DIVIDEDBY:
      if (printSize == 2 && printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        byte *p = calcIntByte(num1 / num2);
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      } else if (printSize == 2) {
        int num1 = byteArray2[0];
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        byte *p = calcIntByte(num1 / num2);
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      } else if (printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = byteArray[0];
        byte *p = calcIntByte(num1 / num2);
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      } else {
        byte *p = calcIntByte(byteArray2[0] / byteArray[0]);
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      }
      break;
    case MODULUS:
      if (printSize == 2 && printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        byte *p = calcIntByte(num1 % num2);
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      } else if (printSize == 2) {
        int num1 = byteArray2[0];
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        byte *p = calcIntByte(num1 % num2);
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      } else if (printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = byteArray[0];
        byte *p = calcIntByte(num1 % num2);
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      } else {
        byte *p = calcIntByte(byteArray2[0] % byteArray[0]);
        byteArray[0] = *(p);
        byteArray[1] = *(p + 1);
      }
      break;
    case EQUALS:
      if (printSize == 2 && printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        if (num1 == num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else if (printSize == 2) {
        int num1 = byteArray2[0];
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        if (num1 == num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else if (printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = byteArray[0];
        if (num1 == num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else {
        int num1 = byteArray2[0];
        int num2 = byteArray[0];
        if (num1 == num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      }
      customType = true;
      finalSize = 1;
      break;
    case NOTEQUALS:
      if (printSize == 2 && printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        if (num1 != num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else if (printSize == 2) {
        int num1 = byteArray2[0];
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        if (num1 != num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else if (printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = byteArray[0];
        if (num1 != num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else {
        int num1 = byteArray2[0];
        int num2 = byteArray[0];
        if (num1 != num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      }
      customType = true;
      finalSize = 1;
      break;
    case LESSTHAN:
      if (printSize == 2 && printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        if (num1 < num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else if (printSize == 2) {
        int num1 = byteArray2[0];
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        if (num1 < num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else if (printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = byteArray[0];
        if (num1 < num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else {
        int num1 = byteArray2[0];
        int num2 = byteArray[0];
        if (num1 < num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      }
      customType = true;
      finalSize = 1;
      break;
    case LESSTHANOREQUALS:
      if (printSize == 2 && printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        if (num1 <= num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else if (printSize == 2) {
        int num1 = byteArray2[0];
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        if (num1 <= num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else if (printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = byteArray[0];
        if (num1 <= num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else {
        int num1 = byteArray2[0];
        int num2 = byteArray[0];
        if (num1 <= num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      }
      customType = true;
      finalSize = 1;
      break;
    case GREATERTHAN:
      if (printSize == 2 && printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        if (num1 > num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else if (printSize == 2) {
        int num1 = byteArray2[0];
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        if (num1 > num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else if (printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = byteArray[0];
        if (num1 > num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else {
        int num1 = byteArray2[0];
        int num2 = byteArray[0];
        if (num1 > num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      }
      customType = true;
      finalSize = 1;
      break;
    case GREATERTHANOREQUALS:
      if (printSize == 2 && printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        if (num1 >= num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else if (printSize == 2) {
        int num1 = byteArray2[0];
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        if (num1 >= num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else if (printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = byteArray[0];
        if (num1 >= num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else {
        int num1 = byteArray2[0];
        int num2 = byteArray[0];
        if (num1 >= num2) byteArray[0] = 49;
        else byteArray[0] = 48;
      }
      customType = true;
      finalSize = 1;
      break;
    case MIN:
      if (printSize == 2 && printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        byteArray[0] = min(num1, num2);
      } else if (printSize == 2) {
        int num1 = byteArray2[0];
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        byteArray[0] = min(num1, num2);
      } else if (printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = byteArray[0];
        byteArray[0] = min(num1, num2);
      } else {
        int num1 = byteArray2[0];
        int num2 = byteArray[0];
        byteArray[0] = min(num1, num2);
      }
      break;
    case MAX:
      if (printSize == 2 && printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        byteArray[0] = max(num1, num2);
      } else if (printSize == 2) {
        int num1 = byteArray2[0];
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        byteArray[0] = max(num1, num2);
      } else if (printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = byteArray[0];
        byteArray[0] = max(num1, num2);
      } else {
        int num1 = byteArray2[0];
        int num2 = byteArray[0];
        byteArray[0] = max(num1, num2);
      }
      break;
    case POW:
      if (printSize == 2 && printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        byteArray[0] = pow(num1, num2);
      } else if (printSize == 2) {
        int num1 = byteArray2[0];
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        byteArray[0] = pow(num1, num2);
      } else if (printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = byteArray[0];
        byteArray[0] = pow(num1, num2);
      } else {
        int num1 = byteArray2[0];
        int num2 = byteArray[0];
        byteArray[0] = pow(num1, num2);
      }
      break;
    case LOGICALAND:
      if (printSize == 2 && printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        if (num1 == 1 && num2 == 1) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else if (printSize == 2) {
        int num1 = byteArray2[0];
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        if (num1 == 1 && num2 == 1) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else if (printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = byteArray[0];
        if (num1 == 1 && num2 == 1) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else {
        int num1 = byteArray2[0];
        int num2 = byteArray[0];
        if (num1 == 1 && num2 == 1) byteArray[0] = 49;
        else byteArray[0] = 48;
      }
      customType = true;
      finalSize = 1;
      break;
    case LOGICALOR:
      if (printSize == 2 && printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        if (num1 == 1 || num2 == 1) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else if (printSize == 2) {
        int num1 = byteArray2[0];
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        if (num1 == 1 || num2 == 1) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else if (printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = byteArray[0];
        if (num1 == 1 || num2 == 1) byteArray[0] = 49;
        else byteArray[0] = 48;
      } else {
        int num1 = byteArray2[0];
        int num2 = byteArray[0];
        if (num1 == 1 || num2 == 1) byteArray[0] = 49;
        else byteArray[0] = 48;
      }
      customType = true;
      finalSize = 1;
      break;
    case LOGICALXOR:
      if (printSize == 2 && printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        if ((num1 == 1 && num2 == 1) || (num1 == 0 && num2 == 0)) byteArray[0] = 48;
        else byteArray[0] = 49;
      } else if (printSize == 2) {
        int num1 = byteArray2[0];
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        if ((num1 == 1 && num2 == 1) || (num1 == 0 && num2 == 0)) byteArray[0] = 48;
        else byteArray[0] = 49;
      } else if (printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = byteArray[0];
        if ((num1 == 1 && num2 == 1) || (num1 == 0 && num2 == 0)) byteArray[0] = 48;
        else byteArray[0] = 49;
      } else {
        int num1 = byteArray2[0];
        int num2 = byteArray[0];
        if ((num1 == 1 && num2 == 1) || (num1 == 0 && num2 == 0)) byteArray[0] = 48;
        else byteArray[0] = 49;
      }
      customType = true;
      finalSize = 1;
      break;
    case BITWISEAND:
      if (printSize == 2 && printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        byteArray[0] = num1 & num2;
      } else if (printSize == 2) {
        int num1 = byteArray2[0];
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        byteArray[0] = num1 & num2;
      } else if (printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = byteArray[0];
        byteArray[0] = num1 & num2;
      } else {
        int num1 = byteArray2[0];
        int num2 = byteArray[0];
        byteArray[0] = num1 & num2;
      }
      break;
    case BITWISEOR:
      if (printSize == 2 && printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        byteArray[0] = num1 | num2;
      } else if (printSize == 2) {
        int num1 = byteArray2[0];
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        byteArray[0] = num1 | num2;
      } else if (printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = byteArray[0];
        byteArray[0] = num1 | num2;
      } else {
        int num1 = byteArray2[0];
        int num2 = byteArray[0];
        byteArray[0] = num1 | num2;
      }
      break;
    case BITWISEXOR:
      if (printSize == 2 && printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        byteArray[0] = num1 ^ num2;
      } else if (printSize == 2) {
        int num1 = byteArray2[0];
        int num2 = calcByteInt(byteArray[0], byteArray[1]);
        byteArray[0] = num1 ^ num2;
      } else if (printSize2 == 2) {
        int num1 = calcByteInt(byteArray2[0], byteArray2[1]);
        int num2 = byteArray[0];
        byteArray[0] = num1 ^ num2;
      } else {
        int num1 = byteArray2[0];
        int num2 = byteArray[0];
        byteArray[0] = num1 ^ num2;
      }
      break;
  }

  if (!customType) finalSize = max(printSize, printSize2);
  finalType = getType(finalSize);
  if (sp < 0) sp = 0;
  for (int i = 0; i < finalSize; i++) {
    pushByte(byteArray[i], PTIndex, sp++);
  }
  pushByte(finalType, PTIndex, sp);
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
      binairInstructions(PTIndex);
      break;
    case MINUS:
      binairInstructions(PTIndex);
      break;
    case TIMES:
      binairInstructions(PTIndex);
      break;
    case DIVIDEDBY:
      binairInstructions(PTIndex);
      break;
    case MODULUS:
      binairInstructions(PTIndex);
      break;
    case UNARYMINUS:
      unairInstructions(PTIndex);
      break;
    case EQUALS:
      binairInstructions(PTIndex);
      break;
    case NOTEQUALS:
      binairInstructions(PTIndex);
      break;
    case LESSTHAN:
      binairInstructions(PTIndex);
      break;
    case LESSTHANOREQUALS:
      binairInstructions(PTIndex);
      break;
    case GREATERTHAN:
      binairInstructions(PTIndex);
      break;
    case GREATERTHANOREQUALS:
      binairInstructions(PTIndex);
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
      unairInstructions(PTIndex);
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
      unairInstructions(PTIndex);
      break;
    case TOCHAR:
      unairInstructions(PTIndex);
      break;
    case TOINT:
      unairInstructions(PTIndex);
      break;
    case TOFLOAT:
      unairInstructions(PTIndex);
      break;
    case ROUND:
      unairInstructions(PTIndex);
      break;
    case FLOOR:
      unairInstructions(PTIndex);
      break;
    case CEIL:
      unairInstructions(PTIndex);
      break;
    case MIN:
      binairInstructions(PTIndex);
      break;
    case MAX:
      binairInstructions(PTIndex);
      break;
    case ABS:
      unairInstructions(PTIndex);
      break;
    case CONSTRAIN:
      Serial.println("CONSTRAIN type");
      break;
    case MAP:
      Serial.println("MAP type");
      break;
    case POW:
      binairInstructions(PTIndex);
      break;
    case SQ:
      unairInstructions(PTIndex);
      break;
    case SQRT:
      unairInstructions(PTIndex);
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
      unairInstructions(PTIndex);
      break;
    case ANALOGWRITE:
      Serial.println("ANALOGWRITE type");
      break;
    case DIGITALREAD:
      unairInstructions(PTIndex);
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
    default:
      stopInstruction(PTIndex);
      break;
  }
  if (readFATEntry(entry.Name).Size < getPTEntry(PTIndex).Registers[3]) updateLoopAddr(PTIndex, 0);
  //  if (entry.Registers[0] != -1) {
  //      Serial.println(getPTEntry(PTIndex).Registers[2]);
  //      Serial.println(getPTEntry(PTIndex).Registers[3]);
  //  }
}

byte* calcIntByte(int num) {
  static byte byteArray[2] = {};
  int x = num / 256;
  if (x < 1) {
    byteArray[0] = 0;
    byteArray[1] = (byte)num;
  } else {
    byteArray[0] = (byte)floor(x);
    byteArray[1] = num - (int)(floor(x) * 256);
  }
  return byteArray;
}

int calcByteInt(byte num1, byte num2) {
  return (256 * num1) + num2;
}

int calcInt(int list[], int Size) {
  if (Size == 0) return list[0];
  else if (Size == 1) return (list[0] * 10) + list[1];
  else return (list[0] * 100) + (list[1] * 10) + list[2];
  return 0;
}
