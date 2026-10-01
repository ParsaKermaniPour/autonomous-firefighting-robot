
#define carSpeed 200  //175 testspeed
// line tracking pins
#define LT_R !digitalRead(10)
#define LT_M !digitalRead(4)
#define LT_L !digitalRead(2)
// IR Sensor pins
#define S_R digitalRead(3)
#define S_L digitalRead(12)
//define the L298n IO pin
#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11



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
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
}

void back_delay(){
  back();
  delay(100);
  stop();
  delay(500);
}

void move() {
  

  if (S_R and S_L)  {
    // back_delay();
    back();
    delay(250);
    if (random(0,1)==0){
          right();
    }else{
      left();
    }
    delay(500);
  }

  else if (S_R) {
    // back_delay();
    // back();
    // delay(250);  
    left();
    delay(500);  

  }

  else if (S_L) {
    // back_delay();
    // back();
    // delay(250);
    right();
    delay(700);  

  }else {
    forward();
  }

}



void loop() {
move();
}
