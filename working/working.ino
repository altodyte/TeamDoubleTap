int pertap = 1; // raw high/lows = 0; delta_times = 1; triangulate = 2;

int nodes[4] = {2,4,7,8}; // Node pin numbers
int readings[4]; // Storage for digital reads from nodes
unsigned long times[4]; // Storage for triggered times
unsigned long eqtimes[4]; // Storage for equalized (delta t) times
int flags[4] = {0, 0, 0, 0}; // Storage for triggered indicators
int first = -1; // Storage for first triggered node #
float speeds[4]; // storage for calibrated speeds
int pixel = 100; // number of pixels along an edge of the surface
 
void setup() {
  Serial.begin(9600); // Initializes communication with the serial port
  delay(1500);
  for (int i=0; i<4; i++) {
    pinMode(nodes[i], INPUT); // Sets each node to input
  }
  //calibrate();
  Serial.println("Calibration Complete");
  //float old_speeds[4] = {0.00233059, 0.00224623, 0.00250228, 0.00419676};
  float old_speeds[4] = {0.0023, 0.0023, 0.0023, 0.0023};
  for (int i=0; i<4; i++) {
    speeds[i] = old_speeds[i];
  }
  Serial.println("Fake Calibration Complete!");
}
 
void loop() {
  if (pertap == 2) {
    read_and_check_all();
    equalize_and_reset();
  } else if (pertap == 1){
    read_and_check_all();
    //equalize_and_reset();
    eq_and_re();
  } else if (pertap == 0){
    for (int i=0; i<4; i++) {
      Serial.println(digitalRead(nodes[i]));
    }
  }
}

// Reads all the nodes and gets times if a trigger is crossed
void read_and_check_all() {
  for (int i=0; i<4; i++) {
    readings[i] = digitalRead(nodes[i]); // Record the digital read of each node
  }
  for (int i=0; i<4; i++){
    if (readings[i] == HIGH && flags[i] == 0){
      times[i] = micros(); // Records the time if a node is triggered
      flags[i] = 1; // Sets the triggered flag
      if (first == -1){
        first = i; // Sets the first flag if it is not already
      }
    }
  }
}

// Calculates Delta Times, Prints, and Resets Flags
void equalize_and_reset() {
  // only runs if ALL FLAGS HAVE BEEN THROWN
  if (flags[0] == 1 && flags[1] == 1 && flags[2] == 1 && flags[3] == 1){
    for (int n=0; n<4; n++){
      eqtimes[n] = times[n]-times[first]; // calculates delta times
      Serial.print(n);
      Serial.print(" ");
      Serial.print(eqtimes[n]);
      Serial.print(" ");
      Serial.print(times[n]);
      Serial.print(" ");
      Serial.println(times[first]);
    }
    for (int i=0; i<4; i++) {
      flags[i] = 0; // resets all the node flags
    }
    first = -1; // resets the first flag
    Serial.println();
    if (pertap==2) {
      triangulate();
    }   
    delay(1000); // delays to prevent reverberation pickup
  }
}

void eq_and_re() {
  if (flags[0] == 1 && flags[1] == 1 && flags[2] == 1 && flags[3] == 1){
    for (int n=0; n<4; n++){
      eqtimes[n] = times[n]-times[first]; // calculates delta times
    }
    Serial.print(first);
    for (int i=0; i<4; i++) {
      Serial.print(" ");
      Serial.print(eqtimes[i]);
    }
    for (int i=0; i<4; i++) {
      flags[i] = 0; // resets all the node flags
    }
    first = -1; // resets the first flag
    Serial.println();
    if (pertap==2) {
      triangulate();
    }   
    delay(1000); // delays to prevent reverberation pickup
  }
}
  
void calibrate() {
  int OLD_PERTAP = pertap;
  pertap = 1;
  float calibrate[4][3]; // Creates a 4x3 array to store calibration data (speeds)
  int cal_index[4][3] = {{1,2,3},{0,2,3},{0,1,3},{0,1,2}}; // keeps track of first index of calibrate to write to
  int cal_counter[4] = {0,0,0,0}; // keeps track of second index of calibrate to write to
  float cal_distance[4][3] = {{1,sqrt(2),1},{1,1,sqrt(2)},{sqrt(2),1,1},{1,sqrt(2),1}}; 
  int calibration_times[4][4]; // is equivalent here to equalized_times[4][4] in WithoutDelay
  int successful_tap[4] = {0,0,0,0};
  for (int a; a<4; a++) {
    while (successful_tap[a] == 0) {
      Serial.print("Please tap node ");
      char c = num_to_char(a);
      Serial.println(c);
      while ((flags[0]==0)||(flags[1]==0)||(flags[2]==0)||(flags[3]==0)) {
        read_and_check_all(); // read and check all nodes against trigger until all flags are thrown
      }
      int KEEP_FIRST = first;
      equalize_and_reset();
      if (eqtimes[a]==0) {
        successful_tap[a] = 1; // if Node A triggered first, consider the calibration tap successful
        Serial.println("Successful tap stored.");
        for (int b=0; b<4; b++) {
          calibration_times[a][b] = eqtimes[b]; // stores eqtime values from successful tap
          Serial.print(calibration_times[a][b]);
          Serial.print(" ");
        }
        Serial.println("");
        Serial.println("");
      } else {
        Serial.print("Node ");
        char d = num_to_char(KEEP_FIRST);
        Serial.print(d);
        Serial.println(" detected first. Examine system and try again.");
        Serial.println("");
      }
    }
  }
  // at this point, calibration_times should be full of unprocessed times
  Serial.println("Calibration times has been filled!");
  // CANNOT PROCEED UNLESS CALIBRATION_TIMES IS FULL
  for (int x=0; x<4; x++) {
    for (int i=0; i<3; i++) {
      Serial.print(calibration_times[x][i]);
      Serial.print(" ");
    }
    Serial.println(calibration_times[x][3]);
  }
  Serial.println("");
  Serial.println("X I DIST T VAL");
  for (int x=0; x<4; x++) {
    for (int i=0; i<3; i++) {
      float dist = cal_distance[cal_index[x][i]][cal_counter[cal_index[x][i]]]*pixel;
      int t = calibration_times[x][cal_index[x][i]];
      float val = dist/t;
      Serial.print(x);
      Serial.print(" ");
      Serial.print(i);
      Serial.print(" ");
      Serial.print(dist);
      Serial.print(" ");
      Serial.print(t);
      Serial.print(" ");
      Serial.println(val, 8);
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
    flags[i] = 0; // resets all the flags
  }
  first = -1;
  Serial.println("");
  Serial.print("Speeds: ");
  for (int i=0; i<3; i++) {
    Serial.print(speeds[i],8);
    Serial.print(" ");
  }
  Serial.println(speeds[3],8);
  pertap = OLD_PERTAP;
}

char num_to_char(int input) {
  if (input == 1) {
    return '1';
  } else if (input == 2) {
    return '2';
  } else if (input == 3) {
    return '3';
  } else if (input == 0) {
    return '0';
  } else {
    return '#';
  }
}

void triangulate() {
  // More Detailed Processing:
  int node_distance[4] = {0,0,0,0};
  for (int i=0; i<4; i++) {
    node_distance[i] = eqtimes[i]*speeds[i];
  }
  float minimum[3] = {pixel*sqrt(2), -1, -1}; // {Minimum Value, X_Coord, Y_Coord}
  int center_x[4] = {0, 0, pixel, pixel}; // X-Coordinates of Nodes
  int center_y[4] = {0, pixel, pixel, 0}; // Y-Coordinates of Nodes
  for (int x=0; x<pixel+1; x++) { // x-coordinate of guess
    for (int y=0; y<pixel+1; y++) { // y-coordinate of guess
      float point_guess[4] = {0,0,0,0}; // Storage for distances from each curve
      float point_min = 0; // Storage for average of point_guess
      for (int r=0; r<4; r++) { // radius (index of node_distance[4])
        if (node_distance[r]!=0) { // ignores zero radius (seed or first node triggered)
          point_guess[r] = abs(node_distance[r] - distance(x,y,center_x[r],center_y[r]));
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
    flags[i] = 0; // resets all the flags
  }
  first = -1; // resets the first seed
  Serial.print("Point_Min ");
  Serial.println(minimum[0]);
  Serial.print("X-Coordinate of Tap: ");
  Serial.println(minimum[1]);
  Serial.print("Y-Coordinate of Tap: ");
  Serial.println(minimum[2]);
  Serial.println("");
}
float distance(int x1, int y1, int x2, int y2) {
  return sqrt((x1-x2)^2 + (y1-y2)^2);
}
