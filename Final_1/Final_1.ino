// Speed Constants
#define straightSpeed 200
#define turnSpeed 130
// Distance Constants
#define closeObject 20
#define detectFire 1021
#define closeFire 1018
// Front Ultrasonics
#define FL_T 24
#define FL_E 25
#define FR_T 26
#define FR_E 27
// Side Ultrasonics
#define L_T 28
#define L_E 29
#define R_T 30
#define R_E 31
// Back Ultrasonics
#define B_T 32
#define B_E 33
// Corner IR
#define FRONT_L !digitalRead(22)
#define FRONT_R !digitalRead(23)
// Line Tracking
#define LT_R !digitalRead(10)
#define LT_M !digitalRead(4)
#define LT_L !digitalRead(2)
// Flame Detection
#define fire_LL analogRead(A8)
#define fire_L analogRead(A9)
#define fire_M analogRead(A10)
#define fire_R analogRead(A11)
#define fire_RR analogRead(A12)
// DC Driver pins
#define AH 14
#define AL 15
#define BH 16
#define BL 17
// Wheel Driver pins
#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11

// Object Detection Constants
#define frontClose ((getDistance(FL_T, FL_E) <= closeObject) or (getDistance(FR_T, FR_E) <= closeObject))
#define leftClose (getDistance(L_T, L_E) <= closeObject)
#define rightClose (getDistance(R_T, R_E) <= closeObject)
#define backClose (getDistance(B_T, B_E) <= closeObject)

// Fire Detection Constants
#define fireRight ((fire_RR < detectFire) or (fire_R < detectFire))
#define fireFront (fire_M < detectFire)
#define fireLeft ((fire_LL < detectFire) or (fire_L < detectFire))
#define fireIsDetected ((fire_LL < detectFire) or (fire_L < detectFire) or (fire_M < detectFire) or (fire_R < detectFire) or (fire_RR < detectFire))
#define fireIsClose ((fire_LL < closeFire) or (fire_L < closeFire) or (fire_M < closeFire) or (fire_R < closeFire) or (fire_RR < closeFire))

// Globabl Variables
unsigned long int prevMillis = millis();
int prevState = 0;

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

void fansON() {
  digitalWrite(AL, LOW);
  digitalWrite(AH, HIGH);
  digitalWrite(BL, LOW);
  digitalWrite(BH, HIGH);
}

void fansOFF() {
  digitalWrite(AL, LOW);
  digitalWrite(AH, LOW);
  digitalWrite(BL, LOW);
  digitalWrite(BH, LOW);
}

int getDistance(int TRIG, int ECHO) {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  int duration = pulseIn(ECHO, HIGH);

  return (duration * 0.034) / 2;
}

void fireDetection() {
  if (fireIsClose) {
    stop(300);
    Serial.println("CLOSE TO FIRE");
    while (fireIsDetected) {
      fansON();
      Serial.println("Spinning Fans");
    }
    fansOFF();
    Serial.println("Fans OFF");
    back(2000);
    Serial.println("Backing Up");
    right(500);
  }

  else if (fireFront) {
    forward();
    Serial.println("Fire Front");
  }

  else if (fireLeft) {
    left(50);
    Serial.println("Fire Left");
  }

  else if (fireRight) {
    right(50);
    Serial.println("Fire Right");
  }
}

void objectDetection() {
  if (frontClose) {
    back(300);
    right(500);
    Serial.println("Object in the middle");
  }
  else if (leftClose) {
    right(200);
    Serial.println("Object on the left");
  }
  else if (rightClose) {
    left(200);
    Serial.println("Object on the right");
  }
}

void lineDetection() {
  // If fire is detected while following a line
  if (fireIsDetected) fireDetection();
  // Detecting a crossroad or perpendiclar to a line
  else if (LT_L and LT_M and LT_R) {
    if (prevState == 0) {
      right(400);
    }
    else {
      forward();
    }
  }
  // Line on the left
  else if (LT_L and LT_M) {
    if (prevState == 0) {
      right(400);
    }
    else {
      left(0);
    }
  }
  // Line on the right
  else if (LT_R and LT_M) {
    if (prevState == 0) {
      left(400);
    }
    else {
      right(0);
    }
  }
  // following the line
  else if (LT_M) {
    forward();
  }
  // line hard left
  else if (LT_L) {
    left(0);
    Serial.println("line left");
  }
  // line hard right
  else if (LT_R) {
    right(0);
    Serial.println("line right");
  }
}

void move() {
  // Detect Fire
  if (fireIsDetected) fireDetection();
  // Detect Object
  else if (leftClose or frontClose or rightClose) objectDetection();
  // Detect Line
  else if (LT_L or LT_M or LT_R) lineDetection();
  // Nothing Detected
  else forward();
}

void setup() {
  Serial.begin(9600);
  // Wheel Driver
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  // DC Driver
  pinMode(AL, OUTPUT);
  pinMode(AH, OUTPUT);
  pinMode(BL, OUTPUT);
  pinMode(BH, OUTPUT);
  // Front Ultrasonic
  pinMode(FL_T, OUTPUT);
  pinMode(FL_E, INPUT);
  pinMode(FR_T, OUTPUT);
  pinMode(FR_E, INPUT);
  // Side Ultrasonic
  pinMode(L_T, OUTPUT);
  pinMode(L_E, INPUT);
  pinMode(R_T, OUTPUT);
  pinMode(R_E, INPUT);
  // Back Ultrasonic
  pinMode(B_T, OUTPUT);
  pinMode(B_E, INPUT);
  // IR Sensors
  pinMode(22, INPUT);
  pinMode(23, INPUT);
  // Line Tracker
  pinMode(10, INPUT);
  pinMode(4, INPUT);
  pinMode(2, INPUT);
  // Flame Sensors
  pinMode(A8, INPUT);
  pinMode(A9, INPUT);
  pinMode(A10, INPUT);
  pinMode(A11, INPUT);
  pinMode(A12, INPUT);
}

void loop() {
  unsigned long int currMillis = millis();
  if (currMillis - prevMillis >= 2000) {
    prevMillis = currMillis;
    if(LT_L or LT_M or LT_R) {
      prevState = 1;
    }
    else {
      prevState = 0;
    }
  }
  move();
}
