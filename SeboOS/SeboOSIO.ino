#define DEBUG
#define BUFFERSIZE 20

typedef struct {
  char Name[BUFFERSIZE];
  void *func;
} commandType;

void store() {
  Serial.println("store function");
}

void retrieve() {
  Serial.println("retrieve function");
}

void erase() {
  char* p = getParameterBuffer();
  char Name[BUFFERSIZE];
  for (int i = 0; i < BUFFERSIZE; i++) Name[i] = *(p+i);
  if (deleteFATEntry(Name)) Serial.println("File deleted.");
  else Serial.println("File couldn't be deleted.");
}

void files() {
  printList();
}

void freespace() {
  checkFreeSpace();
}

void runProgram() {
  Serial.println("run function");
}

void list() {
  Serial.println("list function");
}

void suspendProces() {
  Serial.println("suspend function");
}

void resumeProces() {
  Serial.println("resume function");
}

void killProces() {
  Serial.println("kill function");
}

static commandType command[] = {
  {"store", &store},
  {"retrieve", &retrieve},
  {"erase", &erase},
  {"files", &files},
  {"freespace", &freespace},
  {"run", &runProgram},
  {"list", &list},
  {"suspend", &suspendProces},
  {"resume", &resumeProces},
  {"kill", &killProces},
};

void checkCommand(char* p) {
  bool functionFlag = true;
  for (int i = 0; i < sizeof(command) / sizeof(commandType); i++) {
    if (strncmp(command[i].Name, p, sizeof(command[i].Name)) == 0) {
      functionFlag = false;
      void (*func)() = command[i].func;
      func();
    }
  }
  if (functionFlag) Serial.println("Command not found.");
}

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
      *(p + bufferCounter) = 0;
      bufferCounter = 0;
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




void printBuffer(char* p) {
  for (int i = 0; i < BUFFERSIZE; i++) Serial.print(*(p + i));
  Serial.println();
}

void printIntBuffer(int* p) {
  for (int i = 0; i < BUFFERSIZE; i++) if ((*(p + i)) != 0) Serial.print(*(p + i));
  Serial.println();
}

void clearBuffer(char* p) {
  memset(p, 0, BUFFERSIZE);
}

void clearIntBuffer(int* p) {
  memset(p, 0, BUFFERSIZE);
}
