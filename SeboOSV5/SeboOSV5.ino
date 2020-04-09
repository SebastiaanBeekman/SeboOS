#define PTSIZE 10

typedef struct {
  char Name[12];
  int Size;
  int beginPos;
} FAT;

typedef struct {
  char Name[12];
  int ID;
  int state;                                                              //States: 1 (Running), 0 (pauzed), -1 (Terminated)
  int Registers[4];                                                       //Order: PC, FP, SP, loop addr
} PT;

PT PTArray[PTSIZE];
static int IDCounter = 0;
static bool flag = true;

void setup() {
  Serial.begin(9600);
#ifdef DEBUG
  Serial.println("Welcome to SeboOS V5, Debug mode.");
#else
  Serial.println("Welcome to SeboOS V5.");
#endif
}

/*-----------------------------------------------------------------*/
bool checkPTSize() {
  for (int i = 0; i < PTSIZE; i++) {
    if (PTArray[i].Name[0] == 0 && PTArray[i].ID == 0) return false;
  }
  return true;
}

int writePTEntry(char Name[]) {
  FAT FATEntry = readFATEntry(Name);
  Serial.println(FATEntry.Size);
  if (checkPTSize()) {
    Serial.println("There is no free space in the Processtable...");
    return -1;
  } else if (FATEntry.Size == 0) {
    Serial.println("This file doesn't exist...");
    return -1;
  }
  int Register[] = {FATEntry.beginPos, -1, 0, -1};                         //Order: PC, FP, SP, loop addr
  PT PTEntry = {Name, IDCounter, 1, Register};
  PTArray[IDCounter++] = PTEntry;
  Serial.println("Processtable Entry created.");
  return IDCounter - 1;
}

/*-----------------------------------------------------------------*/
bool runProcess(char Name[]) {
  int ID = writePTEntry(Name);
  if (ID == -1) return false;
  Serial.print("Process running, ID: ");
  Serial.println(ID);
}
/*-----------------------------------------------------------------*/
int checkID(int ID) {
  for (int i = 0; i < PTSIZE; i++) {
    if (PTArray[i].ID == ID && PTArray[i].state != -1) return i;
  }
  return -1;
}

void terminateProcess(int PTNum) {
  for (int i = PTNum; i < PTSIZE; i++) PTArray[i] = PTArray[i + 1];
  IDCounter--;
}

bool PRK(int ID, int state) {
  int PTAddr = checkID(ID);
  if (PTAddr == -1) {
    Serial.println("This process doesn't exist...");
    return false;
  }
  if (state == -1) terminateProcess(PTAddr);
  else PTArray[PTAddr].state = state;
  return true;
}
/*-----------------------------------------------------------------*/
void printProcessList() {
  Serial.println("---ID---State---Name---");
  for (PT entry : PTArray) {
    if (entry.state != -1) {
      Serial.print("|   ");
      Serial.print(entry.ID);
      Serial.print(",    ");
      Serial.print(entry.state);
      Serial.print(",    ");
      Serial.print(entry.Name);
      Serial.println(" |");
    }
  }
  Serial.println("------------------------");
}
/*-----------------------------------------------------------------*/
void loop() {
  if (flag) {
  } flag = false;
}
