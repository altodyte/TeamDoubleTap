// Globals
const int nodes[] = {2, 4, 7, 8}; // pin numbers of input from nodes

int flag[] = {0, 0, 0, 0};
int all_flagged[] = {1, 1, 1, 1};
int reading[] = {LOW, LOW, LOW, LOW};
int first = -1;

unsigned long tapButton[4];



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
  while (flag != all_flagged) {
    for (int i=0; i<4; i++) {
      reading[i] = digitalRead(nodes[i]);
    }
    
    if ((flag[0] == 0) && (reading[0] == HIGH)) {  
      flag[0] = 1;
      tapButton[0] = micros();
      if (first == -1) {
        first = 0;
      }
    }
    
    if ((flag[1] == 0) && (reading[1] == HIGH)) {  
      flag[1] = 1;
      tapButton[1] = micros();
      if (first == -1) {
        first = 1;
      }
    }
    
    if ((flag[2] == 0) && (reading[2] == HIGH)) {  
      flag[2] = 1;
      tapButton[2] = micros();
      if (first == -1) {
        first = 2;
      }  
    }
    
    if ((flag[3] == 0) && (reading[3] == HIGH)) {  
      flag[3] = 1;
      tapButton[3] = micros();
      if (first == -1) {
        first = 3;
      }
    }
  }
  long unsigned first_time = tapButton[first];
  Serial.println(first);
  for (int i=0; i<4; i++) {
    calib_Button[i] = tapButton[i] - tapButton[first];
    flag[i] = 0;
  }
  first = -1;
}  
