typedef struct {
  char Name[12];
  int Size;
  int beginPos;
} FAT;

typedef struct {
  byte Name;
  char type;
  int addr;
  int Size;
  int ID;
} MT;

typedef struct {
  char Name[12];
  int ID;
  int state;                                                              //States: 1 (Running), 0 (pauzed), -1 (Terminated)
  int Registers[4];                                                       //Order: PC, FP, SP, loop addr
} PT;

static bool flag = true;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(-1);
#ifdef DEBUG
  Serial.println("Welcome to SeboOS, Debug mode.");
#else
  Serial.println("Welcome to SeboOS, your madly Operating System.");
#endif
}

void loop() {
  if (flag) {
    
  } flag = false;
    if (readInput()) {
      checkCommand(getFunctionBuffer());
      
      clearBuffer(getFunctionBuffer());
      clearBuffer(getParameterBuffer());
      clearIntBuffer(getSpaceBuffer());
    }
}
