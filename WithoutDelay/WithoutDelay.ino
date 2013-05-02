// Setup Variables
// Sets Node Pin #'s
const int nodes[] = {2,4,7,8}; // pin numbers of input from nodes
// record processing variables
boolean reading[4] = {0,0,0,0}; // storage for values recorded by digital pins
unsigned long int time[4] = {0,0,0,0}; // storage for times of triggers
int flag[4] = {0,0,0,0}; // storage for trigger state indicator
int first = -1; // storage for seed node number (first triggered in cycle)
// triangulation variables
int pixel = 100; // number of pixels across width
float speeds[4] = {0,0,0,0}; // storage for average speeds discovered through calibration

void setup() {
  // Initializes communication with the Serial at 9600 bits/second
  Serial.begin(9600);
  delay(2000);
  // Turns on digital pins for input
  for (int i=0; i<4; i++) {
    pinMode(nodes[i], INPUT);
  }
  calibrate();
  Serial.println("Calibration is complete.");
}

void loop() {
  // Read values of digital pins
  // Check if any new nodes have exceeded trigger voltage
  read_and_check_all();
  delay(500); // ****************************
  // Enters signal processing if all nodes have reached trigger (sensed a tap)
  if ((flag[0]==1)&&(flag[1]==1)&&(flag[2]==3)&&(flag[3]==1)) {
    process();
  }
}

void read_and_check_all() {
  // Reads values of digital pins
  for (int i=0; i<4; i++) {
    reading[i] = digitalRead(nodes[i]);
  }
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
  // More Detailed Processing:
  int delta_times[4] = {0,0,0,0};
  int pixel_distance[4] = {0,0,0,0};
  for (int i=0; i<4; i++) {
    delta_times[i] = time[i] - time[first];
    pixel_distance[i] = delta_times[i]*speeds[i];
  }
  float minimum[3] = {pixel*sqrt(2), 0, 0}; // {Minimum Value, X_Coord, Y_Coord}
  int center_x[4] = {0, pixel, pixel, 0}; // X-Coordinates of Nodes
  int center_y[4] = {0, 0, pixel, pixel}; // Y-Coordinates of Nodes
  for (int x=0; x<pixel+1; x++) { // x-coordinate of guess
    for (int y=0; y<pixel+1; y++) { // y-coordinate of guess
      float point_guess[4] = {0,0,0,0}; // Storage for distances from each node
      float point_min = 0; // Storage for average of point_guess
      for (int r=0; r<4; r++) { // radius (index of pixel_distance[4])
        if (pixel_distance[r]!=0) { // ignores zero radius (seed or first node triggered)
          point_guess[r] = abs(pixel_distance[r] - distance(x,y,center_x[r],center_y[r]));
        }
      }
      // Averages point_guess into point_min
      for (int i=0; i<4; i++) {
        point_min = point_min + point_guess[i]/3;
      }
      // compares to existing minimum and replaces if smaller
      if (point_min < minimum[0]) {
        minimum[0] = point_min;
        minimum[1] = x;
        minimum[2] = y;
      }
    }
  }
  // Reset Flag Values (to untriggered)
  for (int i=0; i<4; i++) {
    flag[i] = 0; // resets all the flags
  }
  first = -1; // resets the first seed
  char a = minimum[1];
  char b = minimum[2];
  Serial.println("X-Coordinate of Tap:");
  Serial.println(minimum[1]);
  Serial.println("Y-Coordinate of Tap:");
  Serial.println(minimum[2]);
}

void calibrate() {
  float calibrate[4][3]; // Creates a 4x3 array to store calibration data (speeds)
  int cal_index[4][3] = {{1,2,3},{0,2,3},{0,1,3},{0,1,2}}; // keeps track of first index of calibrate to write to
  int cal_counter[4]; // keeps track of second index of calibrate to write to
  float cal_distance[4][3] = {{1,sqrt(2),1},{1,1,sqrt(2)},{sqrt(2),1,1},{1,sqrt(2),1}}; 
  int caltimes[4][4]; // Creates a 4x4 array to store calibration data (times) ; raw measurements
  int equalized_times[4][4]; // Storage for equalized times (with respect to seed time) ; delta T's
  for (int i=0; i<4; i++) {
    char c = i;
    String out = "Please tap node ";
    Serial.println(out);
    while ((flag[0]==0)||(flag[1]==0)||(flag[2]==0)||(flag[3]==0)) {
      read_and_check_all();
    }
    delay(1000); // ****************************************
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
      float dist = cal_distance[cal_index[x][i]][cal_counter[cal_index[x][i]]]*pixel;
      int t = equalized_times[x][cal_index[x][i]];
      float val = dist/t;
      Serial.println(val);
      calibrate[cal_index[x][i]][cal_counter[cal_index[x][i]]] = val;
      cal_counter[cal_index[x][i]]++;
    }
  }
  for (int i=0; i<4; i++) {
    for (int j=0; j<3; j++) {
      speeds[i] = speeds[i] + (calibrate[i][j]/3); // averages calibration speeds
    }
  }
  // Reset Flag Values (to untriggered)
  for (int i=0; i<4; i++) {
    flag[i] = 0; // resets all the flags
  }
  first = -1;
  for (int i=0; i<4; i++) {
    Serial.println(speeds[i]);
  }
}

float distance(int x1, int y1, int x2, int y2) {
  return sqrt((x1-x2)^2 + (y1-y2)^2);
}
