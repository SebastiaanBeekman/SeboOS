#include "instruction_set.h"
#define PTSIZE 10

void execute(int instruction, int PTIndex) {
  switch (instruction) {
    case CHAR:
      Serial.println("Char type");
      break;
    case INT:
      Serial.println("Int type");
      break;
    case STRING:
      Serial.println("String type");
      break;
  }
}

void runProcesses() {
  PT *p = getPTArray();
  for (int i = 0; i < PTSIZE; i++) {
    if (strcmp((*(p + i)).Name, "") != 0 && (*(p + i)).state == 1) {                                 //Check if element isn't empty && it's running
      int instructionNum = executeInstruction(i);
      execute(instructionNum, i);
    }
  }
}
