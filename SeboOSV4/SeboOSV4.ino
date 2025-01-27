#define ARRAYSIZE 15
#define MTSIZE 25

typedef struct {
  byte Name;
  char type;
  int addr;
  int Size;
  int ID;
} MT;

static byte memory[256];
static MT MTEntry[MTSIZE];
int sizeOfMT = sizeof(MT);
int noOfVars = 0;

int compareMT(const void * a, const void * b) {           //Compare function which returns the subtraction of two fields with the given structs (Through pointers)
  MT *MTA = (MT *)a;
  MT *MTB = (MT *)b;
  return (MTA->addr - MTB->addr);
}

void setup() {
  Serial.begin(9600);
#ifdef DEBUG
  Serial.println("Welcome to SeboOSV4, Debug mode.");
#else
  Serial.println("Welcome to SeboOSV4, your madly Operating System.");
#endif
}

/*---------------------------------------------------------------------------*/
union {                                                                         //Numbers to bytes and back
  int32_t num;
  byte bval[4];
} numAsBytes;

byte* strToByte(char input[], int inputSize) {                                  //String (char array) to bytes
  static byte byteArray[ARRAYSIZE] = {};
  memset(byteArray, NULL, ARRAYSIZE);                                           //Clear old 'string' before using
  strcpy(byteArray, input);
  return byteArray;
}

char* byteToStr(byte input[], int inputSize) {                                  //Byte array to string (char array pointer)
  static char charArray[ARRAYSIZE];
  memset(charArray, NULL, ARRAYSIZE);                                           //Clear old 'string' before using
  strcpy(charArray, input);
  return charArray;
}
/*---------------------------------------------------------------------------*/
bool checkTableSize() {
  for (int i = 0; i < MTSIZE; i++) {
    if (MTEntry[i].Name == 0 && MTEntry[i].type == NULL) return false;
  }
  return true;
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
  for (int i = MTNum; i < MTSIZE; i++) MTEntry[i] = MTEntry[i + 1];
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
  Serial.println("Memorytable Entry created.");
  return memoryAddr;
}
/*---------------------------------------------------------------------------*/
void deleteTableEntry(int ID) {
  for (int i = 0; i < MTSIZE; i++) {
    if (MTEntry[i].ID == ID) moveMTEntry(i);
  }
}
/*---------------------------------------------------------------------------*/
int getSize() {
  byte temp[1];
  temp[0] = popByte();
  char* c = byteToStr(temp, 1);
  if (*(c) == 'c') return 1;
  else if (*(c) == 'i') return 2;
  else if (*(c) == 'f') return 4;
  else if (*(c) == 's') {
    numAsBytes.bval[0] = popByte();
    return *(int *)&numAsBytes.bval;
  } else return -1;
}

void writeMemory(byte Name, int ID) {
  int Size = getSize();
  char type;
  if (Size == 1) type = 'c';
  else if (Size == 2) type = 'i';
  else if (Size == 4) type = 'f';
  else type = 's';
  int pos = writeTableEntry(Name, type, Size, ID);
  for (int i = Size; i > 0; i--) {
    memory[pos + (i - 1)] = popByte();                  //-1 because pos is the addr of the start value, so if you write backwards you have to go 1 element lower.
  }
  Serial.println("Data written to memory.");
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

bool readMemory(byte Name, int ID) {
  MT MTEntry = getMTEntry(Name, ID);
  if (MTEntry.type == NULL) {
    Serial.println("This combination doesn't exist in the memory.");
    return false;
  }
  int Size = MTEntry.Size;
  int addr = MTEntry.addr;
  char type = getType(Size);
  for (int i = addr; i < (addr + Size); i++) pushByte(memory[i]);
  if (Size > 4) pushByte(Size);
  char temp[1];
  temp[0] = type;
  byte* b = strToByte(temp, 1);
  pushByte(*(b));
}
/*---------------------------------------------------------------------------*/
void printTable() {
  for (MT entry : MTEntry) if (entry.type != NULL) Serial.println(entry.type);
}

void setMem() {
  memory[100] = 42;
  memory[101] = 0;
  memory[102] = 105;

  MT newMTEntry = {97, 'i', 100, 2, 0};
  MTEntry[noOfVars++] = newMTEntry;
}

void clearTable() {
  memset(MTEntry, 0, MTSIZE);
}

void loop() {
      setMem();
      readMemory(97, 0);
      writeMemory(98, 2);
}
