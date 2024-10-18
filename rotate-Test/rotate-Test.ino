// Pin definitions
#define STEP_PIN 9   // Pulse (Step) pin connected to TB6600 PUL+
#define DIR_PIN  8   // Direction pin connected to TB6600 DIR+
#define ENABLE_PIN 7 // Enable pin connected to TB6600 ENA+ (optional)

// Stepper motor parameters
const int stepsPerRevolution = 200;  // Adjust according to your stepper motor (200 for 1.8 degree stepper)
const int microsteps = 4;  // Set to match your TB6600 microstepping setting
const float degreesPerStep = 360.0 / (stepsPerRevolution * microsteps); // Degree per step

const unsigned long maxMoveTime = 10000;

// Speed settings
const int fullSpeedDelay = 1600;      // Delay in microseconds for full speed (smaller delay = faster)
const int slowSpeedDelay = 2600;     // Delay in microseconds for slow speed (bigger delay = slower)
const float decelerationThreshold = 30.0;  // Start slowing down when within 30 degrees of target

// Variables
int targetAngle = 0;   // Target angle from the serial input
int currentAngle = 0;  // Home angle set to 0 initially
String inputString = "";  // A string to hold the incoming input
bool inputComplete = false;  // Whether the input is complete

void setup() {
  // Initialize the stepper driver control pins
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);

  // Enable the stepper driver
  digitalWrite(ENABLE_PIN, LOW); // Low to enable TB6600 (depends on your driver configuration)
  
  // Initialize Serial Communication
  Serial.begin(9600);
  inputString.reserve(20); // Reserve some space for the input string to avoid memory fragmentation

  Serial.println("Stepper motor initialized. Home angle set to 0 degree.");
  Serial.println("Enter an angle (positive or negative) or type 'reset' to reset angle to 0:");
}

void loop() {
  // Check if input is complete
  if (inputComplete) {
    if (inputString.equalsIgnoreCase("reset")) {
      // Reset functionality
      resetAngle();
    } else {
      // Convert the input to an integer for angle
      int angleInput = inputString.toInt();

      // Calculate the target angle based on the current angle and the input
      targetAngle = currentAngle + angleInput;

      // Rotate to the target angle
      rotateToAngle(targetAngle);
    }

    // Clear the input string and flag for next input
    inputString = "";
    inputComplete = false;
  }
}

void serialEvent() {
  // Read incoming data
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n' || inChar == '\r') {
      inputComplete = true;
    } else {
      inputString += inChar;
    }
  }
}

void rotateToAngle(int angle) {
  // Calculate the number of steps to move (always positive)
  int stepsToMove = abs((angle - currentAngle) / degreesPerStep);

  // Set direction of rotation
  if (angle > currentAngle) {
    digitalWrite(DIR_PIN, LOW); // Set direction to clockwise
  } else if (angle < currentAngle) {
    digitalWrite(DIR_PIN, HIGH);  // Set direction to counterclockwise
  }

  delay(10);
  unsigned long startTime = millis();
  bool motorStuck = false;

  // Move the motor by the calculated number of steps
  for (int i = 0; i < stepsToMove; i++) {
    // Calculate how close we are to the target and adjust speed accordingly
    float stepsRemaining = stepsToMove - i;
    float degreesRemaining = stepsRemaining * degreesPerStep;

    // Smoothly decelerate as we approach the target
    int delayTime;
    if (degreesRemaining <= decelerationThreshold) {
      // Calculate smooth deceleration based on proximity to target
      float decelFactor = degreesRemaining / decelerationThreshold;  // Fraction of how close to the target
      delayTime = fullSpeedDelay + decelFactor * (slowSpeedDelay - fullSpeedDelay); // Gradually increase delay
    } else {
      // Full speed when far from target
      delayTime = fullSpeedDelay;
    }

    // Move the motor one step
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(delayTime);   // Adjust the delay based on distance to the target
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(delayTime);   // Adjust the delay based on distance to the target

    // Check for timeout (if the motor is stuck or not responding)
    if (millis() - startTime > maxMoveTime) {
      motorStuck = true;
      break;
    }
  }

  // Handle motor stuck situation
  if (motorStuck) {
    Serial.println("Error: Motor not responding");
  } else {
    currentAngle = angle;  // Update the current angle after movement is completed
    Serial.print("Target to angle: ");
    Serial.println(currentAngle);
    Serial.println("Enter angle:");
  }
}

void resetAngle() {
  currentAngle = 0;  // Reset the current angle to 0
  Serial.println("Angle reset to 0.");
}
