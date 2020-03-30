typedef struct {
  char Name[12];
  int Size;
  int beginPos;
} FAT;

void setup() {
  Serial.begin(9600);
#ifdef DEBUG
  Serial.println("Welcome to SeboOS, Debug mode.");
#else
  Serial.println("Welcome to SeboOS, your madly Operating System.");
#endif
}

void loop() {
  if (readInput()) {
    checkCommand(getFunctionBuffer());
    
    clearBuffer(getFunctionBuffer());
    clearBuffer(getParameterBuffer());
    clearIntBuffer(getSpaceBuffer());
  }
}
