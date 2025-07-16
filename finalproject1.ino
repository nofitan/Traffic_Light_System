// Slave Unit (Distance Scanner)
// Measures vehicle distance from 4 directions using HC-SR04 sensors
// Sends comma-separated distance data over Serial to the Master Arduino

// Define trigger and echo pin mappings for 4 ultrasonic sensors
const int trigPins[4] = {4, 6, 8, 10};
const int echoPins[4] = {3, 5, 7, 9};
// Array to store distance readings (in cm)
int distances[4];

void setup() {
  
  Serial.begin(9600); // initializes UART baud rate

  //Defining pins directions
  for (int i = 0; i < 4; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }
}

void loop() {
  for (int i = 0; i < 4; i++) {
    //Trigger the ultrasonic pulse
    digitalWrite(trigPins[i], LOW);
    delayMicroseconds(2);
    digitalWrite(trigPins[i], HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPins[i], LOW);

    //Measuring the duration of the HIGH from the ECHO
    long duration = pulseIn(echoPins[i], HIGH);

    //Distance calculation
    int distance = (int)(duration * 0.034 / 2);

    //Saving the distance to an array
    distances[i] = distance;
    //Small delay between readings to stabilize sensor
    delay(50);
  }
  //Build the transmition string in the format: "distE,distN,distS,distW"
  String data = "";
  
  for (int i = 0; i < 4; i++) {
    data += String(distances[i]);
    if (i < 3) data += ","; 
  }
  data += "\n";  
  //Send distance data to Master Arduino with Serial(UART)
  Serial.print(data);  
  //Delay before taking the next set of measurements
  delay(5000); 
}