#include <Servo.h>

Servo myServo;

// Pin configuration
const int trigPin = 7;  // Trigger pin connected to pin 7
const int echoPin = 8;  // Echo pin connected to pin 8
const int servoPin = 9; // Servo control pin connected to pin 9
const int petOutsideLight = 12; // Pet outside light connected to pin 12
const int doorLockLight = 13;   // Door lock light connected to pin 13
const int fanPin = 3;           // Fan connected to pin 3

// Speed of sound in air (in cm/µs)
const float soundSpeed = 0.0343;

const int servoClosedAngle = 0;
const int servoOpenedAngle = 90;

bool petIsOutside = false; // Tracks if the pet is currently outside
bool doorIsOpen = false;   // Tracks the door's open/closed state

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  myServo.attach(servoPin);

  // Set pin modes
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(petOutsideLight, OUTPUT);
  pinMode(doorLockLight, OUTPUT);
  pinMode(fanPin, OUTPUT); // Set pin as output

  // Initial states
  digitalWrite(trigPin, LOW);
  digitalWrite(petOutsideLight, LOW); // Pet outside light OFF
  digitalWrite(doorLockLight, HIGH);  // Door lock light ON (door locked)
  analogWrite(fanPin, 255);             // Ensure fan is OFF initially
  myServo.write(servoClosedAngle);    // Door closed

  // Adds a short delay for everything to setup
  delay(200);
}

void loop() {
  // Triggering the ultrasonic pulse
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measuring the time it takes for the echo to return
  long duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  float distance = (duration * soundSpeed) / 2;

  // Print the distance to the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Check if something is detected within 10 cm
  if (distance > 0 && distance <= 10) {
    Serial.println("Pet detected. Rotating servo to open door.");

    // Activate the fan at 50% speed (0-255 range)
    analogWrite(fanPin, 0);

    // Open door: Rotate servo to 90 degrees
    myServo.write(servoOpenedAngle);
    doorIsOpen = true;

    delay(4000); // Wait 4 seconds for the pet to move through

    // Close door: Rotate servo back to 0 degrees
    myServo.write(servoClosedAngle);
    doorIsOpen = false;

    // Toggle pet state
    petIsOutside = !petIsOutside;

    // Update lights after the door closes
    digitalWrite(petOutsideLight, petIsOutside ? HIGH : LOW); // Toggle petOutsideLight
    digitalWrite(doorLockLight, petIsOutside ? LOW : HIGH);   // Toggle doorLockLight

    // Turn OFF the fan after the pet has moved through
    analogWrite(fanPin, 255);
    delay(500);
  } else {
    // Ensure the fan is OFF when no object is detected
    analogWrite(fanPin, 255);
  }

  // Small delay before the next reading
  delay(500);
}
