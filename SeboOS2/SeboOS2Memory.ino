#define ARRAYSIZE 15
#define MTSIZE 25

static byte memory[256];
static MT MTEntry[MTSIZE];
static int sizeOfMT = sizeof(MT);
static int noOfVars = 0;

int compareMT(const void * a, const void * b) {           //Compare function which returns the subtraction of two fields with the given structs (Through pointers)
  MT *MTA = (MT *)a;
  MT *MTB = (MT *)b;
  return (MTA->addr - MTB->addr);
}
/*---------------------------------------------------------------------------*/
MT* sortMT() {
  static MT tempMT[MTSIZE];
  for (int i = 0; i < noOfVars; i++) tempMT[i] = MTEntry[i];
  qsort(MTEntry, noOfVars, sizeOfMT, compareMT);
  return MTEntry;
}

int checkAllocSpaceMT(int fileSize) {
  MT* tempMT = sortMT();                                                                            //Use sorted FAT
  int returnVal = 0;

  for (int i = 0; i < noOfVars; i++) {
    if (noOfVars == 0) return 0;
    returnVal += tempMT[i].Size;
  }
  return returnVal;
}

void moveMTEntry(int MTNum) {
  for (int i = MTNum; i <= MTSIZE - 1; i++) MTEntry[i] = MTEntry[i + 1];
  noOfVars--;
}

void checkMTDuplicates(byte Name, int ID) {
  for (int i = 0; i < MTSIZE; i++) {
    if (MTEntry[i].Name == Name && MTEntry[i].ID == ID) moveMTEntry(i);
  }
}

int writeTableEntry(byte Name, char type, int Size, int ID) {
  int memoryAddr = checkAllocSpaceMT(Size);
  checkMTDuplicates(Name, ID);
  if (noOfVars >= MTSIZE) {
    Serial.println("There is no free space in the Memorytable...");
    return -1;
  } else if (memoryAddr == -1) {
    Serial.println("There is no free space in the Memory...");
    return -1;
  }
  MT newMTEntry = {Name, type, memoryAddr, Size, ID};
  MTEntry[noOfVars++] = newMTEntry;
  return memoryAddr;
}
/*---------------------------------------------------------------------------*/
void deleteTableEntry(int ID) {
  for (int i = 0; i < MTSIZE; i++) {
    if (MTEntry[i].ID == ID) moveMTEntry(i);
  }
}
/*---------------------------------------------------------------------------*/
int getSize(int ID, int sp) {
  char c = popByte(ID, sp--);
  if (c == 'c') return 1;
  else if (c == 'i') return 2;
  else if (c == 'f') return 4;
  else if (c == 's') {
    int i = popByte(ID, sp--);
    return i;
  } else return -1;
}

int writeMemory(byte Name, int ID, int sp) {
  int Size = getSize(ID, sp--);
  char type;
  if (Size == 1) type = 'c';
  else if (Size == 2) type = 'i';
  else if (Size == 4) type = 'f';
  else { type = 's'; sp--; }
  int pos = writeTableEntry(Name, type, Size, ID);
//  Serial.print("pos: ");
//  Serial.println(pos);
  for (int i = 1; i <= Size; i++) {
    memory[pos + (Size - i)] = popByte(ID, sp--);
//    Serial.print(memory[pos + (Size - i)]);
//    Serial.print("-");
//    Serial.println(pos + (Size - i));
  }
  return sp;
}
/*---------------------------------------------------------------------------*/
MT getMTEntry(byte Name, int ID) {
  for (MT entry : MTEntry) {
    if (entry.Name == Name && entry.ID == ID) return entry;
  }
  MT emptyEntry;
  return emptyEntry;
}

char getType(int Size) {
  if (Size == 1) return 'c';
  else if (Size == 2) return 'i';
  else if (Size == 4) return 'f';
  else return 's';
}

int readMemory(byte Name, int ID, int sp) {
  MT MTEntry = getMTEntry(Name, ID);
  if (MTEntry.type == NULL) {
    return -1;
  }
  int Size = MTEntry.Size;
  int addr = MTEntry.addr;
  byte b = getType(Size);
  if (sp < 0) sp = 0;
  for (int i = addr; i < (addr + Size); i++) {
    pushByte(memory[i], ID, sp++);
//    Serial.println(memory[i]);
  }
  if (Size > 4) {
    pushByte(Size, ID, sp++);
  }
  pushByte(b, ID, sp);
  return sp;
}
/*---------------------------------------------------------------------------*/
void printTable() {
  for (MT entry : MTEntry) if (entry.type != NULL) Serial.println(entry.type);
}

void clearTable() {
  memset(MTEntry, 0, MTSIZE);
  noOfVars = 0;
}
