#define ARRAYSIZE 15
#define MTSIZE 25

int compareMT(const void * a, const void * b) {           //Compare function which returns the subtraction of two fields with the given structs (Through pointers)
  MT *MTA = (MT *)a;
  MT *MTB = (MT *)b;
  return (MTA->addr - MTB->addr);
}

typedef struct {
  byte Name;
  char type;
  int addr;
  int Size;
  int ID;
} MT;

static MT MTEntry[MTSIZE];
int sizeOfMT = sizeof(MT);
int noOfVars = 0;
/*---------------------------------------------------------------------------*/
union {                                                                             //Numbers to bytes and back
  int32_t num;
  byte bval[4];
} numAsBytes;

byte* strToByte(char input[]) {                                                     //String (char array) to bytes
  static byte byteArray[ARRAYSIZE];
  memset(byteArray, NULL, sizeof(byteArray));                                       //Clear old 'string' before using
  for (int i = 0; i < sizeof(byteArray); i++) byteArray[i] = (byte)input[i];
  return byteArray;
}

char* byteToStr(byte input[]) {                                                     //Byte array to string (char array pointer)
  static char charArray[ARRAYSIZE];
  memset(charArray, NULL, sizeof(charArray));                                       //Clear old 'string' before using
  for (int i = 0; i < sizeof(charArray); i++) charArray[i] = (char)input[i];
  return charArray;
}
/*---------------------------------------------------------------------------*/
int writeTableEntry(byte Name, char type, int Size, int ID) {
  int addrMT = checkTableSize();
  int addrMemory = checkAllocationSpaceMT(Size);
  checkMTDuplicates(Name, ID);
  if (addrMT == -1) {
    Serial.println("There is no free space in the Memorytable...");
    return -1;
  } else if (addrMemory == -1) {
    Serial.println("There is no free space in the Memory...");
    return -1;
  }
  MT newMTEntry = {Name, type, addrMemory, Size, ID};
  MTEntry[noOfVars++] = newMTEntry;
  Serial.println("Memorytable Entry created.");
  return addrMemory;
}

int checkTableSize() {
  int temp;
  for (int i = 0; i < sizeof(MTEntry); i++) if (MTEntry[i].Name == 0 && MTEntry[i].type == NULL) return i;
  return -1;
}

void checkMTDuplicates(byte Name, int ID) {
  for (int i = 0; i < MTSIZE; i++) {
    if (MTEntry[i].Name == Name && MTEntry[i].ID == ID) moveMTEntry(i);
  }
}

void moveMTEntry(int MTNum) {
  for (int i = MTNum; i < MTSIZE - 1; i++) MTEntry[i] = MTEntry[i + 1];
  noOfVars--;
}

MT* sortMT() {
  static MT tempMT[MTSIZE];
  for (int i = 0; i < noOfVars; i++) tempMT[i] = MTEntry[i];
  qsort(MTEntry, noOfVars, sizeOfMT, compareMT);
  return MTEntry;
}

int checkAllocationSpaceMT(int fileSize) {
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
/*---------------------------------------------------------------------------*/
void writeMemory(byte Name, int ID) {
  int Size = getSize();
  char type;
  if (Size == 1) type = 'c';
  else if (Size == 2) type = 'i';
  else if (Size == 4) type = 'f';
  else type = 's';
  int pos = writeTableEntry(Name, type, Size, ID);
  for (int i = Size; i > 0; i--) memory[pos + (i - 1)] = popByte();                  //-1 because pos is the addr of the start value, so if you write backwards you have to go 1 element lower.
}

int getSize() {
  byte poppedByte = popByte();
  char* p = byteToStr(poppedByte);
  if (*(p) == 'c') return 1;
  else if (*(p) == 'i') return 2;
  else if (*(p) == 'f') return 4;
  else if (*(p) == 's') {
    numAsBytes.bval[0] = popByte();
    return *(int *)&numAsBytes.bval;
  } else return -1;
}

void readMemory(byte Name, int ID) {

}

MTEntry getMTEntry(byte Name, int ID) {
  for (int i = 0; i <= noOfVars; i++) {
    if (MTEntry[i].Name == Name && MTEntry[i].ID == ID) return MTEntry[i];
  }
  MT emptyMT;
  return emptyMT;
}
