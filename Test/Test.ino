#include <EEPROM.h>

typedef struct {
  char Name[12];
  int Size;
  int beginPos;
} FAT;

static int FATSize = sizeof(FAT);

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

int compare(const void * a, const void * b) {
  FAT *FATA = (FAT *)a;
  FAT *FATB = (FAT *)b;
  return (FATA->beginPos - FATB->beginPos);
}

FAT* sortFAT() {
  FAT FATEntry[8];
  for (int i = 0; i < 8; i++) FATEntry[i] = readFATEntry(i);                                     //Copy the Original FAT into array, so no accidents happen & easier to work with
  qsort(FATEntry, 8, FATSize, compare);                                                         //Call sort function 'compare'
  Serial.println(FATEntry[1].Name);
  return FATEntry;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (flag) {
    
  }
  flag = false;
}
