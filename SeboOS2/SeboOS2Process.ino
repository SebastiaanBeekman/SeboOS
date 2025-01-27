#define PTSIZE 10

static PT PTArray[PTSIZE];
static int IDCounter = 0;

PT* getPTArray() {
  return PTArray;
}

int getIDCounter() {
  return IDCounter;
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
  if (checkPTSize()) {
    Serial.println("There is no free space in the Processtable...");
    return -1;
  } else if (strncmp(Name, FATEntry.Name, 12) != 0) {
    Serial.println("This file doesn't exist...");
    return -1;
  }
  int Register[] = {0, 0, 0, 0, 0, 0};                                    //Order: Program Counter, File Pointer, Stack Pointer, loop addr, loopBegin, delayBegin, ifElseRegister
  PT PTEntry = {Name, 0, 0, Register};
  //Bug where assigning the data instantly doesn't work, but after creation it does.
  strcpy(PTEntry.Name, Name);
  PTEntry.ID = IDCounter;
  PTEntry.state = 1;
  PTEntry.Registers[0] = 0;
  PTEntry.Registers[1] = FATEntry.beginPos; 
  PTEntry.Registers[2] = -1;
  PTEntry.Registers[3] = 0;
  PTEntry.Registers[4] = 0;                                               //loopInstruction
  PTEntry.Registers[5] = 0;                                               //millis
  
  PTArray[IDCounter] = PTEntry;
  return IDCounter;
}
/*-----------------------------------------------------------------*/
bool startProcess(char Name[]) {
  int ID = writePTEntry(Name);
  if (ID == -1) return false;
  Serial.print("Process running, ID: ");
  Serial.println(ID);
  IDCounter++;
  return true;
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

void terminate2(int processID) {
  for (int i = 0; i < PTSIZE; i++) {
    if (PTArray[i].ID == processID && PTArray[i].state != -1 && strcmp(PTArray[i].Name,"") != 0) {
      terminateProcess(i);
      i = 0;
    }
  }
}

bool PRK(int ID, int state) {
  int PTAddr = checkID(ID);
  if (PTAddr == -1) {
    Serial.println("This process doesn't exist...");
    return false;
  }
  if (state == -1) {
    terminate2(ID);
    Serial.print("Process ");
    Serial.print(ID);
    Serial.println(" terminated.");
  } else {
    PTArray[PTAddr].state = state;
    Serial.print("Process ");
    Serial.print(ID);
    if (state == 0) Serial.println(" pauzed.");
    else Serial.println(" running.");
  }
  return true;
}
/*-----------------------------------------------------------------*/
PT getPTEntry(int index) {
  return PTArray[index];
}
/*-----------------------------------------------------------------*/
void updateProcessRegisters(int index, int PC, int loopAddr) {
  PTArray[index].Registers[0] = PC;
  PTArray[index].Registers[3] = loopAddr;
}

void updateInstructionRegisters(int index, int sp, int loopAddr) {
  PTArray[index].Registers[2] = sp;
  PTArray[index].Registers[3] = loopAddr;
}

void updateLoopAddr(int index, int loopAddr) {
  PTArray[index].Registers[3] = loopAddr;
}

void updateEndLoop(int index, int endLoop) {
  PTArray[index].Registers[4] = endLoop;
}

void updateMillis(int index, int delayBegin) {
  PTArray[index].Registers[5] = delayBegin;
}
/*-----------------------------------------------------------------*/
void printProcessList() {
  Serial.println("---ID---State---Name---");
  for (int i = 0; i < IDCounter; i++) {
    if (PTArray[i].state != -1) {
      Serial.print("|   ");
      Serial.print(PTArray[i].ID);
      Serial.print(",    ");
      Serial.print(PTArray[i].state);
      Serial.print(",    ");
      Serial.print(PTArray[i].Name);
      Serial.println(" |");
    }
  }
  Serial.println("------------------------");
}
