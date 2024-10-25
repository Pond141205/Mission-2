#define MOTOR_INT1 13
#define MOTOR_INT2 12
#define LIMIT_SWITCH_PIN2 3





void setup() {
  pinMode(MOTOR_INT1, OUTPUT);
  pinMode(MOTOR_INT2, OUTPUT);
  
  Serial.begin(9600);
  

}

void loop() {
  
  PenClick();
  delay(1000);

}

void PenClick(){

  driveMotorDown();
  Serial.println("Down");
  delay(200);

  motorStop();
  Serial.println("Stop");
  delay(2000);
  driveMotorUp();
  Serial.println("Up");
  delay(200);
}

void driveMotorDown(){
  digitalWrite(MOTOR_INT1, HIGH);
  digitalWrite(MOTOR_INT2, LOW);
}

void driveMotorUp(){
  digitalWrite(MOTOR_INT1, LOW);
  digitalWrite(MOTOR_INT2, HIGH);
}

void motorStop() {
  digitalWrite(MOTOR_INT1, LOW);
  digitalWrite(MOTOR_INT2, LOW);
}