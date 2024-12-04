#include <Servo.h>
#include <IRremote.h> // Include the IRremote library

Servo myServo;

// Pin configuration
const int trigPin = 7;           // Ultrasonic trigger pin
const int echoPin = 8;           // Ultrasonic echo pin
const int servoPin = 9;          // Servo control pin
const int petOutsideLight = 12;  // Pet outside light
const int doorLockLight = 13;    // Door lock light
const int fanPin = 3;            // Fan pin
const int IRPin = 4;             // IR receiver signal pin

// Speed of sound in air (in cm/Âµs)
const float soundSpeed = 0.0343;

const int servoClosedAngle = 0;
const int servoOpenedAngle = 90;

bool petIsOutside = false; // Tracks if the pet is currently outside
bool doorIsOpen = false;   // Tracks the door's open/closed state

IRrecv irrecv(IRPin);        // Initialize IR receiver object
decode_results results;      // Variable to store decoded IR data

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  myServo.attach(servoPin);

  // Set pin modes
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(petOutsideLight, OUTPUT);
  pinMode(doorLockLight, OUTPUT);
  pinMode(fanPin, OUTPUT);

  // Initial states
  digitalWrite(trigPin, LOW);
  digitalWrite(petOutsideLight, LOW); // Pet outside light OFF
  digitalWrite(doorLockLight, HIGH);  // Door lock light ON (door locked)
  analogWrite(fanPin, 255);           // Ensure fan is OFF initially
  myServo.write(servoClosedAngle);    // Door closed

  irrecv.begin(IRPin); // Start IR receiver

  delay(200); // Adds a short delay for everything to setup
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

  // Check for IR signal
  if (irrecv.decode()) {
    Serial.print("IR Key Received: ");
    Serial.println(irrecv.decodedIRData.decodedRawData, HEX);

    // Check if the received key matches "0xE916FF00"
    if (irrecv.decodedIRData.decodedRawData == 0xE916FF00) {
      Serial.println("IR Signal matched. Triggering servo to open/close door.");

      // Trigger the servo motor logic
      operateDoor();
    }

    irrecv.resume(); // Resume IR receiver for the next signal
  }

  // Ultrasonic sensor logic: Check if something is detected within 10 cm
  if (distance > 0 && distance <= 10) {
    Serial.println("Pet detected by ultrasonic sensor. Rotating servo to open door.");

    // Trigger the servo motor logic
    operateDoor();
  }

  // Small delay before the next reading
  delay(500);
}

// Function to open and close the door
void operateDoor() {
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
}
