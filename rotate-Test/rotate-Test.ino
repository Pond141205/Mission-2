// Pin definitions
#define STEP_PIN 9   // Pulse (Step) pin connected to TB6600 PUL+
#define DIR_PIN  8   // Direction pin connected to TB6600 DIR+
#define ENABLE_PIN 7 // Enable pin connected to TB6600 ENA+ (optional)

// Stepper motor parameters
const int stepsPerRevolution = 200;  // Adjust according to your stepper motor (200 for 1.8 degree stepper)
const int microsteps = 16;  // Set to match your TB6600 microstepping setting
const float degreesPerStep = 360.0 / (stepsPerRevolution * microsteps); // Degree per step

// Variables
int targetAngle = 0;   // Target angle from the serial input
int currentAngle = 0;  // Track current angle

void setup() {
  // Initialize the stepper driver control pins
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);

  // Enable the stepper driver
  digitalWrite(ENABLE_PIN, LOW); // Low to enable TB6600 (depends on your driver configuration)
  
  // Initialize Serial Communication
  Serial.begin(9600);
  Serial.println("Enter an angle between 0 and 180 degrees:");
}

void loop() {
  // Check if there's input from the serial monitor
  if (Serial.available() > 0) {
    targetAngle = Serial.parseInt();  // Read the target angle input from Serial
    
    // Validate input angle
    if (targetAngle >= 0 && targetAngle <= 180) {
      rotateToAngle(targetAngle);     // Rotate the stepper motor to the desired angle
    } else {
      Serial.println("Please enter a valid angle between 0 and 180.");
    }
  }
}

void rotateToAngle(int angle) {
  int stepsToMove = (angle - currentAngle) / degreesPerStep;  // Calculate the number of steps needed
  
  if (stepsToMove > 0) {
    digitalWrite(DIR_PIN, HIGH); // Set direction to clockwise
  } else {
    digitalWrite(DIR_PIN, LOW);  // Set direction to counterclockwise
    stepsToMove = -stepsToMove;  // Ensure positive number of steps
  }

  // Move the motor by the calculated number of steps
  for (int i = 0; i < stepsToMove; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(500);   // Adjust the delay to control the speed
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(500);   // Adjust the delay to control the speed
  }

  currentAngle = angle;  // Update the current angle
  Serial.print("Moved to angle: ");
  Serial.println(currentAngle);
}
