#include "instruction_set.h"
#define PTSIZE 10

void runProcesses() {
  PT *p = getPTArray();
  for (int i = 0; i < PTSIZE; i++) {
    if (strcmp((*(p + i)).Name, "") != 0 && (*(p + i)).state == 1) {                                 //Check if element isn't empty && it's running
      executeInstruction(i);
      execute(i);
    }
  }
}

void execute(int PTIndex) {
  PT entry = getPTEntry(PTIndex);
  switch (entry.Registers[0]) {
    case CHAR:
      Serial.println("Char type");
      break;
    case INT:
      Serial.println("Int type");
      break;
    case STRING:
      Serial.println("String type");
      break;
    case FLOAT:
      Serial.println("FLOAT type");
      break;
    case SET:
      Serial.println("SET type");
      break;
    case GET:
      Serial.println("GET type");
      break;
    case INCREMENT:
      Serial.println("INCREMENT type");
      break;
    case DECREMENT:
      Serial.println("DECREMENT type");
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
      Serial.println("PRINT type");
      break;
    case PRINTLN:
      Serial.println("PRINTLN type");
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
      Serial.println("STOP type");
      break;
    case FORK:
      Serial.println("FORK type");
      break;
    case WAITUNTILDONE:
      Serial.println("WAITUNTILDONE type");
      break;
    
  }
}
