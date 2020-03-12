#include <EEPROM.h>

typedef struct {
  char Name[12] = {};
  int Size;
  int beginPos;
} FAT;

EERef noOfFiles = EEPROM[160];
int sizeOfFAT = sizeof(FAT);

void setup() {
  Serial.begin(9600);
#ifdef DEBUG
  Serial.println("Welcome to SeboOS V0.1, Debug mode.");
#else
  Serial.println("Welcome to SeboOS V0.1.");
#endif
}

bool writeFATEntry(char Name[], int Size) {                    //1.Check name 2. Check space 3. Profit
  int adress = checkFATSize();
  if (adress == -1) {
    Serial.println(There is no space in the FAT...);
    return false;
  } else if (checkFATDuplicates(Name)) {
    Serial.println("This filename is already in use...");
    return false;
  } else if () {

  }

}

int checkFATSize() {
  int temp;
  for (int i = 0; i < 10; i++) {
    EEPROM.get(i * sizeOfFAT, temp);
    if (temp == -1) return i * sizeOfFAT;                     //return adress
  }
  return -1;
}

bool checkFATDuplicates(char Name[]) {
  FAT FATEntry;
  for (int i = 0; i < noOfFiles; i++) {
    EEPROM.get(i * sizeOfFAT, FATEntry);
    if (strncmp(Name, FATEntry.Name, 12) == 0) return true;
  }
  return false;
}

int checkAllocationSpace(byte Size) {
  
}

FAT* sortFAT() {
  
}

void loop() {

}
