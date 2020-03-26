#include <EEPROM.h>

typedef struct {
  char Name[12];
  int Size;
  int beginPos;
} FAT;

int compare(const void * a, const void * b) {
  FAT *FATA = (FAT *)a;
  FAT *FATB = (FAT *)b;
  return (FATA->beginPos - FATB->beginPos);
}

FAT test1 = {"test1", 0, 812};   //6
FAT test2 = {"test2", 16, 250};   //2
FAT test3 = {"test3", 32, 380};   //4
FAT test4 = {"test4", 48, 1005};  //8
FAT test5 = {"test5", 64, 161};   //1
FAT test6 = {"test6", 80, 956};   //7
FAT test7 = {"test7", 96, 275};   //3
FAT test8 = {"test8", 112, 591};   //5

FAT values[] = {test1, test2, test3, test4, test5, test6, test7, test8};

static bool flag = true;

EERef noOfFiles = EEPROM[160];
int sizeOfFAT = sizeof(FAT);

void setup() {
  Serial.begin(9600);
#ifdef DEBUG
  Serial.println("Welcome to SeboOS V3, Debug mode.");
#else
  Serial.println("Welcome to SeboOS V3.");
#endif
}

int writeFATEntry(char Name[], int Size) {
  int adressFAT = checkFATSize();
  int adressEEPROM = checkAllocationSpace(Size);
  if (adressFAT == -1) {
    Serial.println("There is no free space in the FAT...");
    return -1;
  } else if (checkFATDuplicates(Name)) {
    Serial.println("This filename is already in use...");
    return -1;
  } else if (adressEEPROM == -1) {
    Serial.println("There is no free space in the EEPROM...");
    return -1;
  }
  FAT newFATEntry = { Name, Size, adressEEPROM };
  EEPROM.put(noOfFiles++ * sizeOfFAT, newFATEntry);
  return adressEEPROM;
}

int checkFATSize() {
  int temp;
  for (int i = 0; i <= noOfFiles; i++) {
    EEPROM.get(i * sizeOfFAT, temp);
    if (temp == -1) return i * sizeOfFAT;                        //Return adress of first space in the fAT
  }
  return -1;
}

void moveFATEntry(int FATNum) {
  for (int i = FATNum; i < noOfFiles - 1; i++) {
    for (int j = 0; j < sizeOfFAT; j++) {
      EEPROM[(i * sizeOfFAT) + j] = EEPROM[((i + 1) * sizeOfFAT) + j];
    }
  }
  noOfFiles--;
}

bool checkFATDuplicates(char Name[]) {
  FAT FATEntry;
  for (int i = 0; i <= noOfFiles; i++) {                        //Cycle through every FATEntry and compare given name with FAT name
    EEPROM.get(i * sizeOfFAT, FATEntry);
    if (strncmp(Name, FATEntry.Name, 12) == 0) return true;
  }
  return false;
}

FAT readFATEntry(int pos) {                                     //Return FATEntry based on adress of the FAT
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

int checkAllocationSpace(byte fileSize) {
  FAT* tempFAT = sortFAT();                                                                                //Use sorted FAT
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
/*------------------------------------------------------------------------------------------------*/
bool deleteFATEntry(char Name[]) {
  FAT FATEntry = readFATEntry(Name);
  if (strncmp(Name, FATEntry.Name, 12) != 0) {                  //Check if correct FAT was returned (Could be returned empty if it didn't excist)
    Serial.println("This file doesn't exist...");
    return false;
  }
  for (int i = FATEntry.beginPos; i < FATEntry.Size; i++) {
    EEPROM.put(i, -1);
  }
  int FATpos = getFATPos(FATEntry.Name);
  if (FATpos != -1) moveFATEntry(FATpos);
  return true;
}

FAT readFATEntry(char Name[]) {
  FAT FATEntry;
  for (int i = 0; i <= noOfFiles; i++) {                        //Cycle through every FATEntry and compare given name with FAT name
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
/*------------------------------------------------------------------------------------------------*/
void checkFreeSpace() {
  FAT* tempFAT = sortFAT();
  int largestSpaceFound = 0;
  int temp = 0;
  for (int i = 1; i < noOfFiles; i++) {
    temp = (*(tempFAT + i)).beginPos - ((*(tempFAT + (i - 1))).beginPos + (*(tempFAT + (i - 1))).Size);
    if (temp > largestSpaceFound) largestSpaceFound = temp;
  }
  temp = EEPROM.length() - ((*(tempFAT + (noOfFiles - 1))).beginPos + (*(tempFAT + (noOfFiles - 1))).Size);
  if (temp > largestSpaceFound) largestSpaceFound = temp;                   //Check if there's space between last FAT and size of EEPROM

  Serial.print("Largest available file space: ");
  Serial.print(largestSpaceFound);
  Serial.print(" byte(s).");
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
  Serial.println("-----------------");
}
/*------------------------------------------------------------------------------------------------*/
void writeEEPROM(char Name[], int Size) {
  int pos = writeFATEntry(Name, Size);
  if (pos == -1) return;
  byte data[Size];
  Serial.readBytes(data, Size);
  for (int i = 0; i < Size; i++) EEPROM.write(pos+i, data[i]);
}

void readEEPROM(char Name[]) {
  FAT FATEntry = readFATEntry(Name);
  if (strncmp(Name, FATEntry.Name, 12) != 0) {
    Serial.println("Name doesn't exist in memory, try again.");
    return;
  }
  for (int i = FATEntry.beginPos; i < (FATEntry.beginPos + FATEntry.Size); i++) Serial.println(EEPROM.read(i));
}

void loop() {
  if (flag) {
  }
  flag = false;
}
