// speed macros
#define turnSpeed 130  // 200 recommended
#define straightSpeed 70 // 125 max speed
// distance macros
#define safeTurn 10
#define detectFlame 1021
#define closeToFlame 1018
// IR Sensor pins
#define GROUND_L digitalRead(22)
#define GROUND_R digitalRead(31)
#define FRONT_L !digitalRead(32)
#define FRONT_R !digitalRead(33)
// ultrasonic pins
#define TRIG_L A8
#define ECHO_L A9
#define TRIG_R A10
#define ECHO_R A11
// line tracking pins
#define LT_R !digitalRead(10)
#define LT_M !digitalRead(4)
#define LT_L !digitalRead(2)
// flame detection pins
#define fire_L analogRead(A12)
#define fire_M analogRead(A13)
#define fire_R analogRead(A14)
// L298n IO pin
#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11

//Previous state for line detection
int prevState;
int update = millis();

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

void groundDetection() {
  // If both edge detectors are over the edge
  if (GROUND_R and GROUND_L)  {
    stop(300);
    back(250);
    //NOT safe to turn
    if (getDistanceRight() < safeTurn) {
      // TO-DO: FIND SOLUTION
      left(200);
      back(500);
      right(500);
      Serial.println("NOT Safe to turn Right");
    }
    //Safe to turn
    else {
      right(400);
      Serial.println("Safe to turn Right");
    }
    Serial.println("No ground at all");
  }

  else if (GROUND_L) {
    //NOT safe to turn
    if ((getDistanceRight() < safeTurn) || FRONT_L) {
      // TO-DO: FIND SOLUTION
      left(200);
      back(500);
      right(500);
      Serial.println("NOT Safe to turn Right");
    }
    //Safe to turn
    else {
      right(200);
      Serial.println("Safe to turn Right");
    }
    Serial.println("No left ground");
  }

  else if (GROUND_R) {
    //NOT safe to turn
    if ((getDistanceLeft() < safeTurn) || FRONT_R) {
      // TO-DO: FIND SOLUTION
      right(200);
      back(500);
      left(500);
      Serial.println("NOT Safe to turn Left");
    }
    else {
      left(200);
      Serial.println("Safe to turn Left");
    }
    Serial.println("No right ground");
  }
}

void objectDetection() {
  if (FRONT_L and FRONT_R) {
    // TO-DO: FIND SOLUTION
    back(300);
    right(500);
    Serial.println("Object in the middle");
  }
  else if (FRONT_R) {
    right(200);
    Serial.println("Object on the left");
  }

  else if (FRONT_L) {
    left(200);
    Serial.println("Object on the right");
  }
}

void lineDetection() {
  // flame is detected while avoiding objects, just default to flame detection
  if ((fire_L < detectFlame) or (fire_M < detectFlame) or (fire_R < detectFlame)) {
    fireDetection();
  }
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

void fireDetection() {
  if (fire_M < closeToFlame || fire_L < closeToFlame || fire_R < closeToFlame) {
    stop(300);
    Serial.println("CLOSE TO FIRE");
    while (fire_M < detectFlame || fire_L < detectFlame || fire_R < detectFlame) {
      stop(0);
      Serial.println("Still stopped");
    }
    Serial.println("Fire blown out");
    back(2000);
    Serial.println("Backing up");
    right(500);
    
  }
  else if (fire_M < detectFlame) {
    forward();
    Serial.println("Fire in front");
  }
  else if (fire_L < detectFlame) {
    left(50);
    Serial.println("Fire on the left");
  }
  else if (fire_R < detectFlame) {
    right(50);
    Serial.println("Fire on the right");
  }
}

void move() {
  if (GROUND_L or GROUND_R) {
    groundDetection();
  }
  else if ((fire_L < detectFlame) or (fire_M < detectFlame) or (fire_R < detectFlame)) {
    fireDetection();
  }
  else if (FRONT_L or FRONT_R) {
    objectDetection();
  }
  else if (LT_L or LT_M or LT_R) {
    lineDetection();
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
  pinMode(22, INPUT); //IR
  pinMode(31, INPUT); //IR
  pinMode(32, INPUT); //IR
  pinMode(33, INPUT); //IR
  pinMode(TRIG_L, OUTPUT);  //Ultrasonic
  pinMode(ECHO_L, INPUT);   //Ultrasonic
  pinMode(TRIG_R, OUTPUT);  //Ultrasonic
  pinMode(ECHO_R, INPUT);   //Ultrasonic
  pinMode(A12, INPUT); //flame
  pinMode(A13, INPUT); //flame
  pinMode(A14, INPUT); //flame
}

void loop() {
  //Serial.println(fire_L);
  //Serial.println(fire_R);

  if (update % 300 == 0) {
    if(!LT_L and !LT_M and !LT_R) {
      prevState = 0;
    }
    else {
      prevState = 1;
    }
    update = millis();
  }
  move();
  Serial.print(fire_L); Serial.print(" "); Serial.print(fire_M); Serial.print(" "); Serial.println(fire_R);
}
