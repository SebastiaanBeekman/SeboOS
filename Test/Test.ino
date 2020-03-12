#include <EEPROM.h>

static int FATaddr = 0;
static int EEPROMaddr = 0;

typedef struct {
  char Name[12];
  int beginPos;
  int Size;
} FAT;

static FAT FATArray[32];

void addFATEntry(FAT file) {
  FATArray[FATaddr++] = file;
}

void deleteFATEntry(int pos) {
  
}

FAT writeFATEntry(char[] Name, int beginPos, int Size) {
  
}

FAT readFATEntry() {
  
}

void setup() {
  Serial.begin(9600);
}

void loop() {
    
}
