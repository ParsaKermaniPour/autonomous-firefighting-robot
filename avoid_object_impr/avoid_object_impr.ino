#define turnSpeed 220  // 200 recommended
#define straightSpeed 125
#define safeTurn 20
#define safeMidDistance 950
#define safeSideDistance 970
// line tracking pins
// #define LT_R !digitalRead(10)
// #define LT_M !digitalRead(4)
// #define LT_L !digitalRead(2)
// IR Sensor pins
#define GROUND_L digitalRead(30)
#define GROUND_R digitalRead(31)
#define FRONT_L analogRead(A12)
#define FRONT_R analogRead(A13)
//define the ultrasonic pin
#define TRIG_L A8
#define ECHO_L A9
#define TRIG_R A10
#define ECHO_R A11
//define the L298n IO pin
#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11

// distances detected by the ultrasonic sensor
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

int getDistanceLeft() {
  digitalWrite(TRIG_L, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_L, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_L, LOW);
  int duration = pulseIn(ECHO_L, HIGH);
  
  return (duration * 0.034) / 2;
}

int getDistanceRight() {
  digitalWrite(TRIG_R, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_R, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_R, LOW);
  int duration = pulseIn(ECHO_R, HIGH);
  
  return (duration * 0.034) / 2;
}

void move() {
  if (GROUND_R and GROUND_L)  {
    back(300);
    if (getDistanceRight() > safeTurn) {
      right(700);
      Serial.println("Safe to turn Right");
    }
    else {
      back(300);
      right(700);
      Serial.println("NOT Safe to turn Right");
    }
    Serial.println("No ground at all");
  }

  else if (GROUND_L) {
    if ((getDistanceRight() < safeTurn) || (FRONT_L < safeSideDistance)) {    
      back(300);
      right(500);
      Serial.println("NOT Safe to turn Right");
    }
    else {
      right(500);
      Serial.println("Safe to turn Right");
    }
    Serial.println("No left ground");
  }

  else if (GROUND_R) {
    if ((getDistanceLeft() < safeTurn) || (FRONT_R < safeSideDistance)) {
      back(300);
      left(500);
      Serial.println("NOT Safe to turn Left");
    }
    else {
      left(500);
      Serial.println("Safe to turn Left");
    }
    Serial.println("No right ground");
  }

  else if (FRONT_L < safeMidDistance or FRONT_R < safeMidDistance) {
    back(350);
    right(500);
    Serial.println("Object in the middle");
  }

  else if (FRONT_L < safeSideDistance) {
    left(500);
    Serial.println("Object on the right");
  }

  else if (FRONT_R < safeSideDistance) {
    right(500);
    Serial.println("Object on the left");
  }

  else {
    forward();
  }
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
  pinMode(30, INPUT); //IR
  pinMode(31, INPUT); //IR
  pinMode(A12, INPUT); //IR
  pinMode(A13, INPUT); //IR
  pinMode(TRIG_L, OUTPUT);  //Ultrasonic
  pinMode(ECHO_L, INPUT);   //Ultrasonic
  pinMode(TRIG_R, OUTPUT);  //Ultrasonic
  pinMode(ECHO_R, INPUT);   //Ultrasonic
}

void loop() {
  move();
}
