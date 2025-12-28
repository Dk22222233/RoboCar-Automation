// Motor Driver Pins
#define AN1 2     // Left motor direction 1
#define AN2 3     // Left motor direction 2 (PWM)
#define BN1 4     // Right motor direction 1
#define BN2 5     // Right motor direction 2 (PWM)
#define PWMA 6    // Left motor speed (PWM)
#define PWMB 7    // Right motor speed
#define STBY 10   // Standby (PWM)

// IR Sensor Pins
#define LEFT_IR A0
#define RIGHT_IR A2

// Ultrasonic Sensor Pins
#define TRIG 8
#define ECHO 9

// Thresholds
#define OBSTACLE_DISTANCE 15  // cm (avoid if closer)
#define IR_THRESHOLD 500      // Adjust based on sensor readings

void setup() {
  // Motor Control Pins
  pinMode(AN1, OUTPUT);
  pinMode(AN2, OUTPUT);
  pinMode(BN1, OUTPUT);
  pinMode(BN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(STBY, OUTPUT);
  
  // IR Sensors (Analog Input)
  pinMode(LEFT_IR, INPUT);
  pinMode(RIGHT_IR, INPUT);
  
  // Ultrasonic Sensor
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  
  // Enable Motor Driver
  digitalWrite(STBY, HIGH);
  
  Serial.begin(9600);  // For debugging
}

void loop() {
  int leftIR = analogRead(LEFT_IR);
  int rightIR = analogRead(RIGHT_IR);
  float distance = getDistance();
  
  // Check for obstacles first
  if (distance < OBSTACLE_DISTANCE && distance > 0) {
    avoidObstacle();
  } else {
    // Line Following Logic
    if (leftIR < IR_THRESHOLD && rightIR < IR_THRESHOLD) {
      // Both sensors on the line → FORWARD
      moveForward();
    } else if (leftIR >= IR_THRESHOLD && rightIR < IR_THRESHOLD) {
      // Left sensor off → TANK TURN LEFT
      tankTurnLeft();
    } else if (leftIR < IR_THRESHOLD && rightIR >= IR_THRESHOLD) {
      // Right sensor off → TANK TURN RIGHT
      tankTurnRight();
    } else {
      // Both sensors off → STOP (or search)
      stopMotors();
    }
  }
  delay(50);  // Small delay to prevent erratic behavior
}

// ==== Motor Control Functions ====
void moveForward() {
  digitalWrite(AN1, HIGH);
  digitalWrite(AN2, LOW);
  digitalWrite(BN1, HIGH);
  digitalWrite(BN2, LOW);
  analogWrite(PWMA, 150);  // Left speed
  analogWrite(PWMB, 150);  // Right speed
}

void tankTurnLeft() {
  digitalWrite(AN1, LOW);
  digitalWrite(AN2, HIGH);
  digitalWrite(BN1, HIGH);
  digitalWrite(BN2, LOW);
  analogWrite(PWMA, 150);
  analogWrite(PWMB, 150);
}

void tankTurnRight() {
  digitalWrite(AN1, HIGH);
  digitalWrite(AN2, LOW);
  digitalWrite(BN1, LOW);
  digitalWrite(BN2, HIGH);
  analogWrite(PWMA, 150);
  analogWrite(PWMB, 150);
}

void stopMotors() {
  digitalWrite(AN1, LOW);
  digitalWrite(AN2, LOW);
  digitalWrite(BN1, LOW);
  digitalWrite(BN2, LOW);
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
}

// ==== Ultrasonic Functions ====
float getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  
  long duration = pulseIn(ECHO, HIGH);
  float distance = duration * 0.034 / 2;  // Convert to cm
  
  return distance;
}

void avoidObstacle() {
  stopMotors();
  delay(500);
  
  // Check left and right distances (optional)
  tankTurnLeft();
  delay(500);
  float leftDist = getDistance();
  
  tankTurnRight();
  delay(1000);  // Turn right further
  float rightDist = getDistance();
  
  // Decide which way to turn
  if (leftDist > rightDist && leftDist > OBSTACLE_DISTANCE) {
    tankTurnLeft();
    delay(1000);  // Turn left to avoid
  } else {
    tankTurnRight();
    delay(1000);  // Turn right to avoid
  }
  
  moveForward();
  delay(1000);  // Move past obstacle
}
