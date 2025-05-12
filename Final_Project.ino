#include <Servo.h>
#include <HCSR04.h>

// Pin definitions for the ultrasonic sensor
#define TRIGGER_PIN A0
#define ECHO_PIN A1
#define WET_PIN A2
#define BUZZER_PIN A3
#define SWITCH_PIN 9

Servo waterValve;

int value;
int num_switch = 0;
bool leaking = false;
double distance_cm, depth; // Variables to store measured distance and calculated depth
const int segmentPins[7] = {2, 3, 4, 5, 6, 7, 8}; // LED pins
const int numbers[10][7] = {
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1}  // 9
};

// Ultrasonic sensor
UltraSonicDistanceSensor distanceSensor(TRIGGER_PIN, ECHO_PIN);

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 7; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH);
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  waterValve.attach(A4);
}

void getWaterLevel() {
  // Measure the distance in centimeters
  distance_cm = distanceSensor.measureDistanceCm();
  depth = 14 - distance_cm;
  Serial.print("Measured Distance: ");
  Serial.println(distance_cm);
}

void displayNumber(double num) {
  value = num * 10 / 14;
  Serial.print("depth: ");
  Serial.println(value);
  digitalWrite(segmentPins[0], numbers[value][0]);
  digitalWrite(segmentPins[1], numbers[value][1]);
  digitalWrite(segmentPins[2], numbers[value][2]);
  digitalWrite(segmentPins[3], numbers[value][3]);
  digitalWrite(segmentPins[4], numbers[value][4]);
  digitalWrite(segmentPins[5], numbers[value][5]);
  digitalWrite(segmentPins[6], numbers[value][6]);
}

void leakedDetection() {
  int inValue = analogRead(WET_PIN);
  if (inValue > 750) {
    digitalWrite(BUZZER_PIN, HIGH);
    leaking = true;
  } 
  else {
    digitalWrite(BUZZER_PIN, LOW);
    leaking = false;
  }
}

void press(){
  if(digitalRead(SWITCH_PIN) == LOW) {
    delay(200);
    num_switch++;
    if (num_switch > 8) {
      num_switch = 0;
    }
  }
  Serial.print("Level: ");
  Serial.println(num_switch);
}

void autoFill() {
  if(value < num_switch) {
    Serial.println("Open valve");
    waterValve.write(0);
  }
  else if (value >= num_switch) {
    Serial.println("Close valve");
    waterValve.write(90);
  }
}

void loop() {
  if(leaking) {
    waterValve.write(90);
  }
  autoFill();
  press();
  getWaterLevel();
  displayNumber(depth);
  leakedDetection();
  Serial.println("---------------------------");
  delay(1000);
}