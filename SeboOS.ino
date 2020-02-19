//#define DEBUG
static char inputBuffer[12];
int inputBufferCounter = 0;

void setup() {
  Serial.begin(9600);
#ifdef DEBUG
  Serial.println("Welcome to SeboOS V0.1, Debug mode.");
#else
  Serial.println("Welcome to SeboOS V0.1.");
#endif
}

void readInput() {
  if (Serial.available() > 0) {
    inputBuffer[inputBufferCounter] = Serial.read();
    if (inputBuffer[inputBuffer] == " ") s
#ifdef DEBUG
#else
        inputBufferCounter++;
#endif
  } else {
    inputBufferCounter = 0;
  }
}

void loop() {
  if (Serial.available() > 0) {
    readInput();
#ifdef DEBUG
    Serial.println(inputBuffer[inputBufferCounter]);
    inputBufferCounter++;
#endif
  }
}
