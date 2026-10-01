
#define carSpeed 175  //175 testspeed
// line tracking pins
#define LT_R !digitalRead(10)
#define LT_M !digitalRead(4)
#define LT_L !digitalRead(2)
//define the L298n IO pin
#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11
//define the ultrasonic pin
#define trigPin 9
#define echoPin 10

long distance; 
long duration;



void forward() {
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void back() {
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void left() {
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void right() {
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void right90() {
  right();
  delay(1100);
}

void stop() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
}


void setup() {
  Serial.begin(9600);
  pinMode(10, INPUT);
  pinMode(4, INPUT);
  pinMode(2, INPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(trigPin, INPUT);
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
}

void avoid_edge() {
  back();
  delay(75);
  stop();
  delay(500);
  right90();
  right90();
  stop();
  delay(500);
}

  // while (LT_M == 0) {
  //   //if()

  //   forward();
  // }
  // back();
  // delay(100);
  // stop();
// }
int last = 0;

// void avoid_object(){

// }

void loop() {
  bool detect_ground = !(LT_M or LT_R or LT_L);

  if (detect_ground){
    forward();
  }
  else{
    avoid_edge();
    
  }
  //  Clear 
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  //generating untrasound wave 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration*0.034/2;

  Serial.println(detect_ground);

  if (distance >=6 || distance <= 0){
forward();
  }
  else{
avoid_edge();
}
  
}
