// Setup Variables
const int trigger = HIGH;
// Sets Node Pin #'s
const int nodes[] = {1,2,3,4};
// record processing variables
int reading[4] = {0,0,0,0}; // storage for values recorded by digital pins
int time[4] = {0,0,0,0}; // storage for times of triggers
int flag[4] = {0,0,0,0}; // storage for trigger state indicator
int first = 0; // storage for seed node number (first triggered in cycle)
// triangulation variables
int pixel = 100; // number of pixels across width

void setup() {
  // Initializes communication with the Serial at 9600 bits/second
  Serial.begin(9600);
  // Turns on digital pins for input
  for (int i=0; i<4; i++) {
    pinMode(nodes[i], INPUT);
  }
}

void loop() {
  // Reads values of digital pins
  for (int i=0; i<4; i++) {
    reading[i] = digitalRead(nodes[i]);
  }
  // Checks if any new nodes have exceeded trigger voltage
  for (int i=0; i<4; i++) {
    read_node(i);
  }
  // Enters signal processing if all nodes have reached trigger (sensed a tap)
  if ((flag[0]==1)&&(flag[1]==1)) {
    process();
  }
}

// Checks if a node has triggered this cycle
void read_node(int num) {
  if ((reading[num] = HIGH)&&(flag[num]==0)) {
    time[num] = micros(); // records time of trigger
    flag[num] = 1; // indicates that this node has triggered
    if (first==0) {
      first = num; // checks if this is the first node to trigger (seed)
    }
  }
}

void process() {
  // More detailed signal processing required for more nodes
  if (first==1) {
    Serial.println("Node 1");
  }
  if (first==2) {
    Serial.println("Node 2");
  }
  // Reset Flag Values (to untriggered)
  for (int i=0; i<4; i++) {
    flag[i] = 0;
  }
}

void calibrate() {
  int calibrate[4][3];
  Serial.println("Please tap node 1.");
  while ((flag[0]==0)||(flag[1]==0)) {
    for (int i=0; i<4; i++) {
      read_node(i);
    }
  }
  for (int i=0; i<3; i++) {
    calibrate[0][i] = time[i+1] - time[0]; // seed time - node time
  }
}
