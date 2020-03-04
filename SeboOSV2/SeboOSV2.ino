#define DEBUG
#define BUFFERSIZE 20

void setup() {
  Serial.begin(9600);
#ifdef DEBUG
  Serial.println("Welcome to SeboOS V0.1, Debug mode.");
#else
  Serial.println("Welcome to SeboOS V0.1.");
#endif
}

typedef struct {
  char Name[BUFFERSIZE];
  void *func;
} commandType;

void store() {
  Serial.println("store");
}

void retrieve() {
  Serial.println("retrieve");
}

void erase() {
  Serial.println("erase");
}

void files() {
  Serial.println("files");
}

static commandType command[] = {
  {"store", &store},
  {"retrieve", &retrieve},
  {"erase", &erase},
  {"files", &files}
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

bool readInput() {
  char* p;
  static int bufferCounter = 0;
  static bool parameterFlag = false;

  if (Serial.available() > 0) {
    int inputChar = Serial.read();

    if (parameterFlag) {                          //Set pointer to either buffer based on the input (ParameterBuffer)
      p = getParameterBuffer();
    } else {                                      //(FunctionBuffer)
      p = getFunctionBuffer();
    }

    if (inputChar == 32) {                        //Check if input value is a Space, if so add terminating zero, reset counter (if its the first space) & change buffer
      *(p + bufferCounter) = 0;
      if (!parameterFlag) bufferCounter = 0;
      parameterFlag = true;
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
  }
  return false;
}

void printBuffer(char* p) {
  for (int i = 0; i <= BUFFERSIZE; i++) {
    Serial.print(*(p+i));
  }
  Serial.println();
}

void clearBuffer(char* p) {
  memset(p, 0, BUFFERSIZE);
}

void loop() {
  if (readInput()) {
    checkCommand(getFunctionBuffer());
    clearBuffer(getFunctionBuffer());
    clearBuffer(getParameterBuffer());
  }
}
