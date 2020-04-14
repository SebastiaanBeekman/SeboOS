#define BUFFERSIZE 20

typedef struct {
  char Name[BUFFERSIZE];
  void *func;
} commandType;

static commandType command[] = {
  {"store", &store},
  {"retrieve", &retrieve},
  {"erase", &erase},
  {"files", &files},
  {"freespace", &freespace},
  {"run", &runProgram},
  {"list", &list},
  {"suspend", &suspendProcess},
  {"resume", &resumeProcess},
  {"kill", &killProcess},
};

int sizeOfCommandList = sizeof(command) / sizeof(commandType);

int checkCommand(char* p) {
  bool functionFlag = true;
  for (int i = 0; i < sizeOfCommandList; i++) {
    if (strncmp(command[i].Name, p, sizeof(command[i].Name)) == 0) {
      functionFlag = false;
      void (*func)() = command[i].func;
      func();
    }
    if (!functionFlag && i == 0) return 1;
    if (!functionFlag) return 0;
  }
  if (functionFlag) Serial.println("Command not found. Known commands:\nstore, retrieve, erase, files, freespace, run, list, suspend, resume and kill.");
  return -1;
}
/*---------------------------------------------------------------------------*/
void store() {                                                                          //Bug where store is activated twice, tried lots of tests. Conclusion: For loop i is reset back to 0
  char* p = getParameterBuffer();
  int* ip = getSpaceBuffer();
  char Name[BUFFERSIZE] = {};
  char sizeBuffer[4];
  for (int i = 0; i < BUFFERSIZE + 4; i++) {
    if (i < *(ip)) Name[i] = *(p + i);                                             //if i < spaceBuffer, save name
    else sizeBuffer[i - * (ip)] = *(p + (i + 1));                                  //else save size
  }
  if (!writeEEPROM(Name, atoi(sizeBuffer))) Serial.println("Failed to write data.");
}

void retrieve() {
  char* p = getParameterBuffer();
  char Name[12];
  strcpy(Name, p);
  printFile(Name);
}

void erase() {
  char* p = getParameterBuffer();
  char Name[12];
  strcpy(Name, p);
  if (!deleteFATEntry(Name)) Serial.println("This file doesn't exist.");
}

void files() {
  printList();
}

void freespace() {
  checkFreeSpace();
}

void runProgram() {
  char* p = getParameterBuffer();
  char Name[12];
  strcpy(Name, p);
  startProcess(Name);
}

void list() {
  printProcessList();
}

void suspendProcess() {
  char* p = getParameterBuffer();
  PRK(atoi(p), 0);
}

void resumeProcess() {
  char* p = getParameterBuffer();
  PRK(atoi(p), 1);
}

void killProcess() {
  char* p = getParameterBuffer();
  PRK(atoi(p), -1);
}
/*---------------------------------------------------------------------------*/
char* getFunctionBuffer() {
  static char functionBuffer[BUFFERSIZE];
  return functionBuffer;
}

char* getParameterBuffer() {
  static char parameterBuffer[BUFFERSIZE];
  return parameterBuffer;
}

int* getSpaceBuffer() {
  static int spaceBuffer[BUFFERSIZE];
  return spaceBuffer;
}
/*---------------------------------------------------------------------------*/
bool readInput() {
  char* p;
  int* p2 = getSpaceBuffer();
  static int bufferCounter = 0;
  static int spaceCounter = 0;
  static bool parameterFlag = false;

  if (Serial.available() > 0) {
    int inputChar = Serial.read();

    if (parameterFlag) p = getParameterBuffer();          //Set pointer to either buffer based on the input (ParameterBuffer)
    else p = getFunctionBuffer();                         //(FunctionBuffer)

    if (inputChar == 32) {                                //Check if input value is a Space, if so add space, reset counter (if its the first space) & change buffer
      if (!parameterFlag) {
        bufferCounter = 0;
        parameterFlag = true;
      } else {
        *(p + bufferCounter) = 32;
        *(p2 + spaceCounter++) = bufferCounter;
        bufferCounter++;
      }
    } else if (inputChar == 10) {                 //Check if input value is a newLine, if so add terminating zero, reset counter & return true (input finished)
      *(p + bufferCounter) = '\0';
      bufferCounter = 0;
      spaceCounter = 0;
      parameterFlag = false;
      return true;
    } else {                                      //Else add the input value to the selected buffer
      *(p + bufferCounter) = inputChar;
      bufferCounter++;
    }
    return false;
  } //Serial.available end
  return false;
} //Function end
/*---------------------------------------------------------------------------*/
void printBuffer(char* p) {
  for (int i = 0; i < BUFFERSIZE; i++) if ((*(p + i)) != 0) Serial.print(*(p + i));
  Serial.println();
}

void printIntBuffer(int* p) {
  for (int i = 0; i < BUFFERSIZE; i++) if ((*(p + i)) != 0) Serial.print(*(p + i));
  Serial.println();
}
/*---------------------------------------------------------------------------*/
void clearSerial() {
  while (Serial.available() > 0) char t = Serial.read();
}

void clearBuffer(char* p) {
  memset(p, 0, BUFFERSIZE);
}
void clearIntBuffer(int* p) {
  memset(p, 0, BUFFERSIZE);
}
