#include <EEPROM.h>
#define STORAGESIZE 1000

EERef noOfFiles = EEPROM[160];
int sizeOfFAT = sizeof(FAT);

int compare(const void * a, const void * b) {                                   //Compare function which returns the subtraction of two fields with the given structs (Through pointers)
  FAT *FATA = (FAT *)a;
  FAT *FATB = (FAT *)b;
  return (FATA->beginPos - FATB->beginPos);
}

FAT test1 = {"test1", 90, 161};    //6
FAT test2 = {"test2", 16, 250};   //2
FAT test3 = {"test3", 32, 380};   //4
FAT test4 = {"test4", 48, 1005};  //8
FAT test5 = {"test5", 64, 161};   //1
FAT test6 = {"test6", 80, 956};   //7

FAT values[] = {test1/*, test2, test3/*, test4, test5, test6*/};
/*---------------------------------------------------------------------------*/
int checkFATSize() {
  int temp;
  for (int i = 0; i < 10; i++) {                                                //Loop till 10, because there are a max of 9 FAT entries possible (16*9=144, last spot possible)
    EEPROM.get(i * sizeOfFAT, temp);
    if (temp == -1) return i * sizeOfFAT;                                       //Return adress of first space in the fAT
  }
  return -1;
}

FAT readFATEntry(int pos) {                                                     //Return FATEntry based on adress of the FAT
  FAT FATEntry;
  EEPROM.get(pos * sizeOfFAT, FATEntry);
  return FATEntry;
}

FAT* sortFAT() {
  static FAT FATEntry[9];                                                                                 //Size of 9, because there are only 9 possible fat entries (9 * 16 = 144, so last spot till 160)
  for (int i = 0; i < noOfFiles; i++) FATEntry[i] = readFATEntry(i);                                      //Copy the Original FAT into array, so no accidents happen & easier to work with
  qsort(FATEntry, noOfFiles, sizeOfFAT, compare);                                                         //Call sort function 'compare'
  return FATEntry;
}

int checkAllocSpaceEEPROM(byte fileSize) {
  FAT* tempFAT = sortFAT();                                                                                //Use sorted FAT
  Serial.print("FAT: ");
  Serial.println(tempFAT[0].Size);

  if (noOfFiles == 0) return 161;
  else if (noOfFiles == 1) return 161 + tempFAT[0].Size;
  else if (noOfFiles == 2) return 161 + tempFAT[0].Size + tempFAT[1].Size;

  for (int i = 0; i < noOfFiles; i++) {
    if (tempFAT[i + 1].beginPos - (tempFAT[i].beginPos + tempFAT[i].Size) > fileSize) {                    //Compare the space between the EEPROM adresses
      return tempFAT[i].beginPos + tempFAT[i].Size;
    }
  }
  if (EEPROM.length() - (tempFAT[noOfFiles].beginPos + tempFAT[noOfFiles].Size) > fileSize) {              //Check if there's space between last FAT and size of EEPROM
    return tempFAT[noOfFiles].beginPos + tempFAT[noOfFiles].Size;
  }
  return -1;
}

bool checkFATDuplicates(char Name[]) {
  FAT FATEntry;
  for (int i = 0; i < noOfFiles; i++) {                                     //Cycle through every FATEntry and compare given name with FAT name
    EEPROM.get(i * sizeOfFAT, FATEntry);
    if (strncmp(Name, FATEntry.Name, 12) == 0) return true;
  }
  return false;
}

int writeFATEntry(char Name[], int Size) {
  int FATAddr = checkFATSize();
  int EEPROMAddr = checkAllocSpaceEEPROM(Size);
  Serial.print("EEPROM: ");
  Serial.println(EEPROMAddr);
  if (FATAddr == -1) {
    Serial.println("There is no free space in the FAT...");
    return -1;
  } else if (checkFATDuplicates(Name)) {
    Serial.println("This filename is already in use...");
    return -1;
  } else if (EEPROMAddr == -1) {
    Serial.println("There is no free space in the EEPROM...");
    return -1;
  } else if (Size > STORAGESIZE) {
    Serial.println("File size is too big...");
    return -1;
  }
  FAT newFATEntry = {"", Size, EEPROMAddr};
  strcpy(newFATEntry.Name, Name);
  EEPROM.put(FATAddr, newFATEntry);
  return EEPROMAddr;
}
/*---------------------------------------------------------------------------*/
FAT readFATEntry(char Name[]) {
  FAT FATEntry;
  for (int i = 0; i < noOfFiles; i++) {                        //Cycle through every FATEntry and compare given name with FAT name
    EEPROM.get(i * sizeOfFAT, FATEntry);
    if (strncmp(Name, FATEntry.Name, 12) == 0) return FATEntry;
  }
  FAT emptyFAT;
  return emptyFAT;
}

int getFATPos(char Name[]) {
  FAT FATEntry;
  for (int i = 0; i <= noOfFiles; i++) {                        //Cycle through every FATEntry and compare given name with FAT name
    EEPROM.get(i * sizeOfFAT, FATEntry);
    if (strncmp(Name, FATEntry.Name, 12) == 0) return i;
  }
  return -1;
}

void moveFATEntries(int FATNum) {                                             //Function that sorts FAT
  for (int i = FATNum; i <= noOfFiles - 1; i++) {
    for (int j = 0; j < sizeOfFAT; j++) {
      EEPROM[(i * sizeOfFAT) + j] = EEPROM[((i + 1) * sizeOfFAT) + j];      //Set next fat to current
    }
  }
}

bool deleteFATEntry(char Name[]) {
  FAT FATEntry = readFATEntry(Name);
  if (strncmp(Name, FATEntry.Name, 12) != 0) return false;
  int FATPos = getFATPos(Name);
  if (FATPos != -1) moveFATEntries(FATPos);
  noOfFiles--;
  Serial.print(FATEntry.Name);
  Serial.println(" deleted.");
  return true;
}
/*---------------------------------------------------------------------------*/
bool writeEEPROM(char Name[], int Size) {
  int pos = writeFATEntry(Name, Size);
  if (pos == -1) return false;
  byte data[Size];
  Serial.println("What would you like to store:");
  Serial.readBytes(data, Size);
  for (byte b : data) {
    EEPROM.write(pos++, b);
  }
  noOfFiles++;
  Serial.println("Data stored.");
  while (Serial.available() > 0) Serial.read();                                                                      //Catch the \n
  return true;
}
/*---------------------------------------------------------------------------*/
byte readEEPROM(char Name[], int index) {
  FAT FATEntry = readFATEntry(Name);
  byte b = EEPROM.read(FATEntry.beginPos + index);
  //  Serial.print(b);
  //  Serial.print(":");
  //  Serial.println((char)b);
  return b;
}
/*---------------------------------------------------------------------------*/
void printFile(char Name[]) {
  FAT FATEntry = readFATEntry(Name);
  byte byteArray[FATEntry.Size];
  if (strncmp(Name, FATEntry.Name, 12) != 0) {
    Serial.println("Name doesn't exist in memory, try again.");
    return;
  }
  for (int i = FATEntry.beginPos; i < (FATEntry.beginPos + FATEntry.Size); i++) Serial.print((char)EEPROM.read(i));
  Serial.println();
}
/*---------------------------------------------------------------------------*/
void checkFreeSpace() {
  FAT* tempFAT = sortFAT();
  int largestSpaceFound = 0;
  int temp = 0;
  for (int i = 1; i < noOfFiles; i++) {
    temp = (*(tempFAT + i)).beginPos - ((*(tempFAT + (i - 1))).beginPos + (*(tempFAT + (i - 1))).Size);       //Begin position, next entry, minus the begin position + size of the current entry gives space in between
    if (temp > largestSpaceFound) largestSpaceFound = temp;
  }
  temp = EEPROM.length() - ((*(tempFAT + (noOfFiles - 1))).beginPos + (*(tempFAT + (noOfFiles - 1))).Size);
  if (temp > largestSpaceFound) largestSpaceFound = temp;                                                     //Check if there's space between last FAT and size of EEPROM

  Serial.print("Largest available file space: ");
  Serial.print(largestSpaceFound);
  Serial.println(" byte(s).");
}

void printList() {
  Serial.println("---Name---Size---");
  FAT FATEntry;
  for (int i = 0; i < noOfFiles; i++) {
    FATEntry = readFATEntry(i);
    Serial.print(" | ");
    Serial.print(FATEntry.Name);
    Serial.print(",  ");
    Serial.print(FATEntry.Size);
    if (FATEntry.Size < 10) Serial.print("  ");
    else if (FATEntry.Size < 99) Serial.print(" ");
    Serial.println(" |");
  }
  Serial.print("-- NoOfFiles:");
  Serial.print((int)noOfFiles);
  Serial.println(" --");
}
/*---------------------------------------------------------------------------*/
void clearFAT() {
  for (int i = 0; i <= 159; i++) EEPROM.put(i, -1);
  noOfFiles = 0;
}

void fillFAT() {
  noOfFiles = sizeof(values) / sizeof(FAT);
  for (int i = 0; i < noOfFiles; i++) EEPROM.put(sizeOfFAT * i, values[i]);
}
