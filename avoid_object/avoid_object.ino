#include <Servo.h>

#define turnSpeed 200  // 200 recommended
#define straightSpeed 75
#define servoDelay 300
#define safeDistance 20
// line tracking pins
#define LT_R !digitalRead(10)
#define LT_M !digitalRead(4)
#define LT_L !digitalRead(2)
// IR Sensor pins
#define S_R digitalRead(2)
#define S_L digitalRead(4)
//define the ultrasonic pin
#define TRIG A5
#define ECHO A4
//define the L298n IO pin
#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11


// creates servo object
Servo myServo;
// distances detected by the ultrasonic sensor
int middleDistance = 0;
int rightDistance = 0;
int leftDistance = 0;
// saves time since last scan
int lastScanTime = millis();


void forward() {
  analogWrite(ENA, straightSpeed);
  analogWrite(ENB, straightSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void back(int duration) {
  analogWrite(ENA, straightSpeed);
  analogWrite(ENB, straightSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  delay(duration);
}

void left(int duration) {
  analogWrite(ENA, turnSpeed);
  analogWrite(ENB, turnSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  delay(duration);
}

void right(int duration) {
  analogWrite(ENA, turnSpeed);
  analogWrite(ENB, turnSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  delay(duration);
}

void stop(int duration) {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);

  delay(duration);
}


void setup() {
  Serial.begin(9600);
  // Motor stuff
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  // Sensor Stuff
  pinMode(2, INPUT); //IR
  pinMode(4, INPUT); //IR
  pinMode(TRIG, OUTPUT);  //Ultrasonic
  pinMode(ECHO, INPUT);   //Ultrasonic
  // Servo Stuff
  myServo.attach(3);
  
}

int getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  int duration = pulseIn(ECHO, HIGH);
  
  return (duration * 0.034) / 2;
}

void move() {
  if (S_R and S_L)  {
    back(300);
    right(700);
  }
  else if (S_R) {
    left(500);
  }
  else if (S_L) {
    right(500);
  }
  else {
    forward();
  }
}

void scan() {
  myServo.write(135);
  delay(servoDelay);
  leftDistance = getDistance();

  myServo.write(90);
  delay(servoDelay);
  middleDistance = getDistance();

  myServo.write(45);
  delay(servoDelay);
  rightDistance = getDistance();

  lastScanTime = millis();
}

void loop() {
  if (millis() - lastScanTime > 500) {
    scan();
    myServo.write(90);
    if (leftDistance < safeDistance) {
      right(250);
      Serial.println("LEFT DANGER");
    }
    if (middleDistance < safeDistance) {
      back(250);
      right(300);
      Serial.println("MIDDLE DANGER");
    }
    if (rightDistance < safeDistance) {
      left(250);
      Serial.println("RIGHT DANGER");
    }
  }
  move();
}
