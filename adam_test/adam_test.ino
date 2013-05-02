int node0 = 2;
int node1 = 4;
int node2 = 7;
int node3 = 8;
int vs[4];
int time = 0;
unsigned long times[4];
int flags[4] = {0, 0, 0, 0};
int n;
int pertap = 1;
int first = -1;
 
 
void setup(){
  Serial.begin(9600);
  pinMode(node0, INPUT);
  pinMode(node1, INPUT);
  pinMode(node2, INPUT);
  pinMode(node3, INPUT);
}
 
void loop(){
  if (pertap == 1){
    vs[0] = digitalRead(node0);
    vs[1] = digitalRead(node1);
    vs[2] = digitalRead(node2);
    vs[3] = digitalRead(node3);
 
    int i;
    for (i=0; i<4; i++){
      if (vs[i] == HIGH && flags[i] == 0){
        times[i] = micros();
        flags[i] = 1;
        if (first == -1){
          first = i;
        }
      }
    }
    if (flags[0] == 1 && flags[1] == 1 && flags[2] == 1 && flags[3] == 1){
      int n;
      for (n=0; n<4; n++){
        times[n] += -times[first];
        Serial.print(n);
        Serial.print(" ");
        Serial.println(times[n]);
      }
      flags[0] = 0;
      flags[1] = 0;
      flags[2] = 0;
      flags[3] = 0;
      first = -1;
      Serial.println();
      delay(1000);
    }
  }
  if (pertap == 0){
    Serial.println(digitalRead(node0));
    Serial.print(digitalRead(node1));
    Serial.print(digitalRead(node2));
    Serial.print(digitalRead(node3));
  }
}

