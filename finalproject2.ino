// MASTER Unit
// Controls 4-direction intersection using distance data from a Slave Arduino
// Selects green light direction based on closest vehicle distance

//Defining pins
#define NORTH_RED     7
#define NORTH_YELLOW  6
#define NORTH_GREEN   5

#define SOUTH_RED     11
#define SOUTH_YELLOW  12
#define SOUTH_GREEN   13

#define EAST_RED      8
#define EAST_YELLOW   9
#define EAST_GREEN    10

#define WEST_RED      4
#define WEST_YELLOW   3
#define WEST_GREEN    2

//Traffic lights delays
#define GREEN_TIME    5000
#define YELLOW_TIME   3000
#define ALL_RED_TIME  500

// These indexes map to the values[] array from Serial input
#define WEST   0
#define NORTH  1
#define SOUTH  2
#define EAST   3

// Currently active traffic direction (initially -1)
int currentDirection = -1; 

// Define groupings for opposite directions
enum DirectionGroup { GROUP_NS, GROUP_EW };
DirectionGroup getGroup(int dir);

void setup() {
  
  Serial.begin(9600); //initializes UART baud rate

  const int trafficPins[] = {
    NORTH_RED, NORTH_YELLOW, NORTH_GREEN,
    SOUTH_RED, SOUTH_YELLOW, SOUTH_GREEN,
    EAST_RED, EAST_YELLOW, EAST_GREEN,
    WEST_RED, WEST_YELLOW, WEST_GREEN
  };
  // Set all the traffic light pins as OUTPUT
  for (int i = 0; i < sizeof(trafficPins) / sizeof(trafficPins[0]); i++) {
    pinMode(trafficPins[i], OUTPUT);
  }

  transitionToAllRed();
}

void loop() {
  Serial.setTimeout(1500); //Set read timeout
  // Read incoming distance string (example: "120,100,150,200")
  String line = Serial.readStringUntil('\n');
  int values[4], index = 0;
  String temp = "";
  
  if (line.length() == 0) return; 
  
  // Parsing the string of distances into the values[] array
  for (int i = 0; i < line.length(); i++) {
    if (line[i] == ',') {
      values[index++] = temp.toInt();
      temp = "";
    } else {
      temp += line[i];
    }
  }
  values[index] = temp.toInt();
  
  // Find min in each group
  int nsMin = min(values[NORTH], values[SOUTH]);
  int ewMin = min(values[WEST], values[EAST]);
  int minIdx;

  // Decide which group to prioritize
  if (nsMin < ewMin) {
    minIdx = (values[NORTH] < values[SOUTH]) ? NORTH : SOUTH;
  } else if (ewMin < nsMin) {
    minIdx = (values[WEST] < values[EAST]) ? WEST : EAST;
  } else {
    // Both groups equal
    if (currentDirection == NORTH || currentDirection == SOUTH) {
      // Last was NS so pick from EW
      minIdx = (values[WEST] <= values[EAST]) ? WEST : EAST;
    } else {
      // Last was EW so pick from NS
      minIdx = (values[NORTH] <= values[SOUTH]) ? NORTH : SOUTH;
    }
  }

  // First iteration, transition into first green direction
    if (currentDirection == -1) {
    transitionToAllRed();
    delay(ALL_RED_TIME);
    transitionToYellow(minIdx);
    delay(YELLOW_TIME);
    transitionToGreen(minIdx);
    currentDirection = minIdx;
    delay(GREEN_TIME);
    return;
  }

  // If direction group has changed, full light transition
  if (getGroup(minIdx) != getGroup(currentDirection)) {
    transitionToYellow(currentDirection);
    delay(YELLOW_TIME);
    transitionToAllRed();
    delay(ALL_RED_TIME);
    transitionToYellow(minIdx);
    delay(YELLOW_TIME);
    transitionToGreen(minIdx);
    currentDirection = minIdx;
  }

  delay(GREEN_TIME); //Green delay time
}

// Activates green light for a direction pair
void transitionToGreen(int dir) {
  if (dir == NORTH || dir == SOUTH) { //north-south
    digitalWrite(NORTH_RED, LOW);
    digitalWrite(SOUTH_RED, LOW);
    digitalWrite(NORTH_YELLOW, LOW);
    digitalWrite(SOUTH_YELLOW, LOW);
    digitalWrite(NORTH_GREEN, HIGH);
    digitalWrite(SOUTH_GREEN, HIGH);
  } else if (dir == WEST || dir == EAST) { //east-west
    digitalWrite(EAST_RED, LOW);
    digitalWrite(WEST_RED, LOW);
    digitalWrite(EAST_YELLOW, LOW);
    digitalWrite(WEST_YELLOW, LOW);
    digitalWrite(EAST_GREEN, HIGH);
    digitalWrite(WEST_GREEN, HIGH);
  }
}
// Activates yellow light for a direction pair
void transitionToYellow(int dir) {
  if (dir == NORTH || dir == SOUTH) {
    digitalWrite(NORTH_GREEN, LOW);
    digitalWrite(SOUTH_GREEN, LOW);
    digitalWrite(NORTH_YELLOW, HIGH);
    digitalWrite(SOUTH_YELLOW, HIGH);
  } else if (dir == WEST || dir == EAST) {
    digitalWrite(EAST_GREEN, LOW);
    digitalWrite(WEST_GREEN, LOW);
    digitalWrite(EAST_YELLOW, HIGH);
    digitalWrite(WEST_YELLOW, HIGH);
  }
}
// Turns all directions red
void transitionToAllRed() {
  //turning off green&yellow leds
  digitalWrite(NORTH_GREEN, LOW);
  digitalWrite(NORTH_YELLOW, LOW);
  digitalWrite(SOUTH_GREEN, LOW);
  digitalWrite(SOUTH_YELLOW, LOW);
  digitalWrite(EAST_GREEN, LOW);
  digitalWrite(EAST_YELLOW, LOW);
  digitalWrite(WEST_GREEN, LOW);
  digitalWrite(WEST_YELLOW, LOW);

  //turning on red leds
  digitalWrite(NORTH_RED, HIGH);
  digitalWrite(SOUTH_RED, HIGH);
  digitalWrite(EAST_RED, HIGH);
  digitalWrite(WEST_RED, HIGH);
}
// Returns which group (NS or EW) a direction belongs to
DirectionGroup getGroup(int dir) {
  return (dir == NORTH || dir == SOUTH) ? GROUP_NS : GROUP_EW;
}
