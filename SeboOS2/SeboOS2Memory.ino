#define ARRAYSIZE 15
#define MTSIZE 25

static byte memory[256];
static MT MTEntry[MTSIZE];
int sizeOfMT = sizeof(MT);
int noOfVars = 0;

int compareMT(const void * a, const void * b) {           //Compare function which returns the subtraction of two fields with the given structs (Through pointers)
  MT *MTA = (MT *)a;
  MT *MTB = (MT *)b;
  return (MTA->addr - MTB->addr);
}
/*---------------------------------------------------------------------------*/
bool checkTableSize() {
  int temp;
  for (int i = 0; i < MTSIZE; i++) {
    if (MTEntry[i].Name != 0 && MTEntry[i].type != NULL) return true;
  }
  return false;
}

MT* sortMT() {
  static MT tempMT[MTSIZE];
  for (int i = 0; i < noOfVars; i++) tempMT[i] = MTEntry[i];
  qsort(MTEntry, noOfVars, sizeOfMT, compareMT);
  return MTEntry;
}

int checkAllocSpaceMT(int fileSize) {
  MT* tempMT = sortMT();
  for (int i = 0; i < noOfVars; i++) {
    if (tempMT[i + 1].addr - (tempMT[i].addr - tempMT[i].Size) > fileSize) {
      return tempMT[i].addr - tempMT[i].Size;
    }
  }
  if (256 - (tempMT[noOfVars].addr - tempMT[noOfVars].Size) > fileSize) {
    return tempMT[noOfVars].addr - tempMT[noOfVars].Size;
  }
  return -1;
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
  if (checkTableSize()) {
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
  else type = 's';
  int pos = writeTableEntry(Name, type, Size, ID);
  for (int i = Size; i > 0; i--) {
    memory[pos + (i - 1)] = popByte(ID, sp--);                  //-1 because pos is the addr of the start value, so if you write backwards you have to go 1 element lower.
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
  byte *p = numToByte(MTEntry.Size);
  int addr = MTEntry.addr;
  byte b = getType(Size);
  for (int i = addr; i < (addr + Size); i++) {
    pushByte(memory[i], ID, sp++);
  }
  if (Size > 4) {
    pushByte(*(p), ID, sp++);
    pushByte(*(p + 1), ID, sp++);
  }
  pushByte(b, ID, sp++);
  return sp;
}
/*---------------------------------------------------------------------------*/
void printTable() {
  for (MT entry : MTEntry) if (entry.type != NULL) Serial.println(entry.type);
}

void clearTable() {
  memset(MTEntry, 0, MTSIZE);
}
