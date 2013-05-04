// Globals
const int nodes[] = {2, 4, 7, 8}; // pin numbers of input from nodes

int flag[] = {0, 0, 0, 0};
int reading[] = {LOW, LOW, LOW, LOW};
int first = -1;

unsigned long readtimes[] = {0, 0, 0, 0};
unsigned long tapButton[4];

unsigned long calib_Button[4];


//Setup
void setup() {
  Serial.begin(9600);

  for (int i=0; i<4; i++) {
    pinMode(nodes[i], INPUT);
  }

//  calibrateButtons();
}

void loop() {
  calibrateButtons();
  
}

void calibrateButtons() {
  Serial.println("Please tap 0 button.");
  captureData();
  for (int i=0; i<4; i++){
    Serial.println(calib_Button[i]);
  }
  delay(500);
}

void captureData() {
  while ((flag[0] == 0) || (flag[1] == 0) || (flag[2] == 0) || (flag[3] == 0)) {
    for (int i=0; i<4; i++) {
      reading[i] = digitalRead(nodes[i]);
      if (flag[i] == 0) {
        readtimes[i] = micros();
      }
    }

    
    if ((flag[0] == 0) && (reading[0] == HIGH)) {  
      flag[0] = 1;
      tapButton[0] = readtimes[0];
      if (first == -1) {
        first = 0;
      }
    }
    
    if ((flag[1] == 0) && (reading[1] == HIGH)) {  
      flag[1] = 1;
      tapButton[1] = readtimes[1];
      if (first == -1) {
        first = 1;
      }
    }
    
    if ((flag[2] == 0) && (reading[2] == HIGH)) {  
      flag[2] = 1;
      tapButton[2] = readtimes[2];
      if (first == -1) {
        first = 2;
      }  
    }
    
    if ((flag[3] == 0) && (reading[3] == HIGH)) {  
      flag[3] = 1;
      tapButton[3] = readtimes[3];
      if (first == -1) {
        first = 3;
      }
    }
  }

  
  Serial.println(first);
  
  for (int i=0; i<4; i++) {
    calib_Button[i] = tapButton[i] - readtimes[first];
    flag[i] = 0;
    tapButton[i] = 0;
  }
  first = -1;
}  
