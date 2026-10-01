#include <Servo.h>

#define turnSpeed 200  // 200 recommended
#define straightSpeed 75  //how fast does it go when going straight
#define servoDelay 175
#define safeDistance 20    //what is the distance that is consider safe to turn without colliding into something
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

//control the motors to move forward
void forward() {
  analogWrite(ENA, straightSpeed);
  analogWrite(ENB, straightSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

//control the motors to move backwards for a specificed
void back(int duration) {
  analogWrite(ENA, straightSpeed);
  analogWrite(ENB, straightSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  delay(duration);
}

//control the motors to turn right for a specified duration
void left(int duration) {
  analogWrite(ENA, turnSpeed);
  analogWrite(ENB, turnSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  delay(duration);
}

//control the motors to turn right for a specifict dueration
void right(int duration) {
  analogWrite(ENA, turnSpeed);
  analogWrite(ENB, turnSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  delay(duration);
}

// stop the robot for a curtain duration
void stop(int duration) {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);

  delay(duration);
}


//Initialize pins, Serial communication, and servo
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
  pinMode(2, INPUT);      //IR
  pinMode(4, INPUT);      //IR
  pinMode(TRIG, OUTPUT);  //Ultrasonic
  pinMode(ECHO, INPUT);   //Ultrasonic
  // Servo Stuff
  myServo.attach(3);
}


int getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);            //set the Trig pin to high to trigger the ultrasonic sensor
  delayMicroseconds(10);               //keep the trig pin to high for 10 miliseconds
  digitalWrite(TRIG, LOW);             //reset the trig pin back to low
  int duration = pulseIn(ECHO, HIGH);  //Measure the duration of the pulse on the ECHo pin

  return (duration * 0.034) / 2;  //calculate and return the distance based on the duration
}

void move() {
  if (S_R and S_L) {  //if both IR sensors are off go back for 0.3 second and then turn right for 0.7 seconds
    back(300);
    right(700);
  } else if (S_R) {  //if the right sensor is off turn left for .5 seconds
    left(500);
  } else if (S_L) {  //if the left sensor is off turn right for 0.5 seconds
    right(500);
  } else {
    forward();  //if both IR sensors are on then just go straight
  }
}

void scan() {
  myServo.write(125);            //move the servo to the angle 125 degrees
  delay(servoDelay);             //delay the servo movement by servo delay
  leftDistance = getDistance();  //get the distance and store it

  myServo.write(90);               //move the servo to the angle 90 degrees
  delay(servoDelay);               //delay the servo movemnet by servo delay
  middleDistance = getDistance();  //get the distance and store it

  myServo.write(55);              //move the servo to the angle 55 degrees
  delay(servoDelay);              //delay the servo movement by servo delay
  rightDistance = getDistance();  //get the distance and store it

  lastScanTime = millis();  //record the current time as the last scan time
}

void loop() {
  if (millis() - lastScanTime > 500) {
    scan();             //perform a scan every 500 milliseconds (5 seconds)
    myServo.write(90);  //return the servo to the center position

    if (leftDistance < safeDistance) {  //If a obstacle is too close on the left, then first check if right is clear if so turn else go backwards
      stop(300);
      myServo.write(55);  //recheck right distance
      delay(servoDelay);
      rightDistance = getDistance();
      if (rightDistance > safeDistance) {
        right(250);
        forward();
        Serial.println("LEFT DANGER");
      } else {
        back(300);
        scan();
      }
    }

    if (middleDistance < safeDistance) {      //check if there is a object in the middle or straight ahead
      stop(300);                              //stops for 0.3 seconds we can remove this later if it works the way it's suppose to
      myServo.write(55);                      //make the servo look right
      delay(servoDelay);                      //check if the path is clear or all objects are a certain distance away
      rightDistance = getDistance();          //save the updated value
      myServo.write(125);                     //do the same check to see if left is clear
      leftDistance = getDistance();
      if (rightDistance > safeDistance) {     //if right is clear move back and right
        back(250);
        right(300);
        Serial.println("MIDDLE DANGER");
      } else if (leftDistance > safeDistance) {   //if right was not clear and left is clear go back and left
        back(250);
        left(300);
        Serial.println("MIDDLE DANGER");
      } else {                                //if both left and right are not clear just take the risk by going back
        back(300);
        scan();                               //before moving again scan the area
      }
    }

    if (rightDistance < safeDistance) {      //check if there is object to the right 
      stop(300);                             //stops for 0.3 seconds
      myServo.write(125);                    //move the survo to the left 
      leftDistance = getDistance();          //recheck the distance and if the left is clear
      if (leftDistance > safeDistance) {     //if left is clear go back and left
        back(250);
        left(250);
        Serial.println("Right Danger");
      } else {                              //if left is not clear then take the risk and go back and rescan the area
        back(300);
        scan();
      }
    }
    move();  //Execute the move() functions to respond to IR sensor data
  }
}
