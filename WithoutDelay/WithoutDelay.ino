// Setup Variables
const int trigger = HIGH; // unused
// Sets Node Pin #'s
const int nodes[] = {2,4,7,8};
// record processing variables
boolean reading[4] = {0,0,0,0}; // storage for values recorded by digital pins
int time[4] = {0,0,0,0}; // storage for times of triggers
int flag[4] = {0,0,0,0}; // storage for trigger state indicator
int first = -1; // storage for seed node number (first triggered in cycle)
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
  // Read values of digital pins
  // Check if any new nodes have exceeded trigger voltage
  read_and_check_all();
  // Enters signal processing if all nodes have reached trigger (sensed a tap)
  if ((flag[0]==1)&&(flag[1]==1)) { // &&(flag[2]==3)&&(flag[3]==1)
    process();
  }
}

void read_and_check_all() {
  // Reads values of digital pins
  for (int i=0; i<4; i++) {
    reading[i] = digitalRead(nodes[i]);
  }
  // TAKE OUT WHEN USING ALL NODES
  reading[2] = LOW;
  reading[3] = LOW;
  // Checks if any new nodes have exceeded trigger voltage
  for (int i=0; i<4; i++) {
    read_node(i);
  }
}

// Checks if a node has triggered this cycle
void read_node(int num) {
  if ((reading[num] == HIGH)&&(flag[num]==0)) {
    time[num] = micros(); // records time of trigger
    flag[num] = 1; // indicates that this node has triggered
    if (first==-1) {
      first = num; // checks if this is the first node to trigger (seed)
    }
  }
}

void process() {
  // More detailed signal processing required for more nodes
  if (first==0) {
    Serial.println("Node 0");
  }
  if (first==1) {
    Serial.println("Node 1");
  }
  // Reset Flag Values (to untriggered)
  for (int i=0; i<4; i++) {
    flag[i] = 0; // resets all the flags
  }
  first = -1; // resets the first seed
}

void calibrate() {
  int calibrate[4][3]; // Creates a 4x3 array to store calibration data (speeds)
  int cal_index[4][3] = {{1,2,3},{0,2,3},{0,1,3},{0,1,2}}; // keeps track of first index of calibrate to write to
  int cal_counter[4]; // keeps track of second index of calibrate to write to
  int cal_distance[4][3] = {{1,sqrt(2),1},{1,1,sqrt(2)},{sqrt(2),1,1},{1,sqrt(2),1}}; 
  int caltimes[4][4]; // Creates a 4x4 array to store calibration data (times)
  int equalized_times[4][4]; // Storage for equalized times (with respect to seed time)
  for (int i=0; i<4; i++) {
    char c = i;
    String out = "Please tap node "+c;
    Serial.println(out);
    while ((flag[0]==0)||(flag[1]==0)) { // ||(flag[2]==0)||(flag[3]==0)
      read_and_check_all();
    }
    for (int j=0; j<4; j++) {
      caltimes[i][j] = time[j];
    }
    // need to reset flags, first
  }
  for (int i=0; i<4; i++) {
    for (int j=0; j<4; j++) {
      equalized_times[i][j] = caltimes[i][j] - caltimes[i][i];
    }
  }
  for (int x=0; x<4; x++) {
    for (int i=0; i<3; i++) {
      int dist = cal_distance[cal_index[x][i]][cal_counter[cal_index[x][i]]]*pixel;
      int t = equalized_times[x][cal_index[x][i]];
      int val = dist/t; // might have to be a float
      calibrate[cal_index[x][i]][cal_counter[cal_index[x][i]]];
      cal_counter[cal_index[x][i]]++;
    }
  }
  // Reset Flag Values (to untriggered)
  for (int i=0; i<4; i++) {
    flag[i] = 0; // resets all the flags
  }
  first = -1;
}
