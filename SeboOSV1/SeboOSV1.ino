#define DEBUG
#define bufferSize 25
static char inputBuffer[bufferSize];
static char functionBuffer[bufferSize];
static int inputBufferCounter = 0;
static int paramSpot = 0;
static bool paramFlag = true;

void setup() {
  Serial.begin(9600);
#ifdef DEBUG
  Serial.println("Welcome to SeboOS V0.1, Debug mode.");
#else
  Serial.println("Welcome to SeboOS V0.1.");
#endif
}

typedef struct {
  char Name[bufferSize];
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

void checkCommand() {
  bool functionFlag = true;
  for (int i = 0; i < sizeof(command) / sizeof(commandType); i++) {
    if (strncmp(command[i].Name, functionBuffer, sizeof(command[i].Name)) == 0) {
      functionFlag = false;
      void (*func)() = command[i].func;
      func();
    }
  }
  if (functionFlag) Serial.println("Command not found.");
}

void splitArray() {
  for (int i = 0; i < paramSpot; i++) {
    functionBuffer[i] = inputBuffer[i];
  }
  paramSpot = 0;
  paramFlag = true;
}

bool readInput() {
  if (Serial.available() > 0) { 
    bool returnVal = false;
    int temp = Serial.read();

    if (temp == 10 || temp == 32) {                                   //NewLine or Space
      inputBuffer[inputBufferCounter] = "\0";

      if (temp == 10) {                                               //NewLine
        returnVal = true;
      } else if (temp == 32 && paramFlag) {                           //Space
        paramSpot = inputBufferCounter;
        paramFlag = false;
      }
    } else {                                                          //No special char
      inputBuffer[inputBufferCounter] = temp;
      returnVal = false;
    }

    if (returnVal && paramFlag) {                                     //Check if current char is last char & if there has been a space
      paramSpot = inputBufferCounter;
      paramFlag = true;
    }

    if (returnVal == true) {                                          //If current char is last char
      inputBufferCounter = 0;
    } else {
      inputBufferCounter++;
    }

    return returnVal;
  }
  return false;
}

void clearBuffers() {
  memset(inputBuffer, 0, sizeof(inputBuffer));
  memset(functionBuffer, 0, sizeof(functionBuffer));
}

void loop() {
  if (readInput()) {
    splitArray();
    checkCommand();
    clearBuffers();
  }
  //Processes...
}
