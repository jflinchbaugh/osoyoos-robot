/*  ___   ___  ___  _   _  ___   ___   ____ ___  ____
   / _ \ /___)/ _ \| | | |/ _ \ / _ \ / ___) _ \|    \
  | |_| |___ | |_| | |_| | |_| | |_| ( (__| |_| | | | |
   \___/(___/ \___/ \__  |\___/ \___(_)____)___/|_|_|_|
                    (____/
   Omni direction Mecanum Wheel Arduino Smart Car Tutorial Lesson 2 Obstacle avoidance auto driving
   Tutorial URL http://osoyoo.com/?p=30022
   CopyRight www.osoyoo.com

   This project will show you how to make Osoyoo robot car in auto drive mode and avoid obstacles


*/
#include <Servo.h>

#define speedPinR 9   //  RIGHT WHEEL PWM pin D45 connect front MODEL-X ENA 
#define RightMotorDirPin1  22    //Front Right Motor direction pin 1 to Front MODEL-X IN1  (K1)
#define RightMotorDirPin2  24   //Front Right Motor direction pin 2 to Front MODEL-X IN2   (K1)                                 
#define LeftMotorDirPin1  26    //Left front Motor direction pin 1 to Front MODEL-X IN3 (  K3)
#define LeftMotorDirPin2  28   //Left front Motor direction pin 2 to Front MODEL-X IN4 (  K3)
#define speedPinL 10   // Left WHEEL PWM pin D7 connect front MODEL-X ENB

#define speedPinRB 11   //  RIGHT WHEEL PWM pin connect Back MODEL-X ENA 
#define RightMotorDirPin1B  5    //Rear Right Motor direction pin 1 to Back MODEL-X IN1 (  K1)
#define RightMotorDirPin2B 6    //Rear Right Motor direction pin 2 to Back MODEL-X IN2 (  K1) 
#define LeftMotorDirPin1B 7    //Rear left Motor direction pin 1 to Back MODEL-X IN3  K3
#define LeftMotorDirPin2B 8  //Rear left Motor direction pin 2 to Back MODEL-X IN4  k3
#define speedPinLB 12    //   LEFT WHEEL  PWM pin D8 connect Rear MODEL-X ENB

#define SERVO_PIN     13  //servo connect to D5
#define Echo_PIN    31 // Ultrasonic Echo pin connect to A5
#define Trig_PIN    30  // Ultrasonic Trig pin connect to A4

#define FAST_SPEED  180 // 120   //both sides of the motor speed
#define SPEED  80     //both sides of the motor speed
#define TURN_SPEED  120 //110   //both sides of the motor speed
#define OBSTACLE_LIMIT 30  //minimum distance in cm to obstacles at both sides (the car will allow a shorter distance sideways)
#define WATCH_DELAY 150 // delay when moving servo
#define LEFT_RIGHT 1
#define RIGHT_LEFT -1
#define HARD_LEFT B001
#define HARD_RIGHT B100

int distance;
int dead_turn = 0;
int scanDir = LEFT_RIGHT;
int hardTurn = B000;

Servo head;

/*motor control*/

void stop_Stop()    //Stop
{
  digitalWrite(RightMotorDirPin1, LOW);
  digitalWrite(RightMotorDirPin2, LOW);
  digitalWrite(LeftMotorDirPin1, LOW);
  digitalWrite(LeftMotorDirPin2, LOW);
  digitalWrite(RightMotorDirPin1B, LOW);
  digitalWrite(RightMotorDirPin2B, LOW);
  digitalWrite(LeftMotorDirPin1B, LOW);
  digitalWrite(LeftMotorDirPin2B, LOW);
  set_Motorspeed(0, 0, 0, 0);
}

/*set motor speed */
void set_Motorspeed(int leftFront, int rightFront, int leftBack, int rightBack)
{
  analogWrite(speedPinL, leftFront);
  analogWrite(speedPinR, rightFront);
  analogWrite(speedPinLB, leftBack);
  analogWrite(speedPinRB, rightBack);
}

void drive(int leftFront, int rightFront, int leftBack, int rightBack) {
  set_Motorspeed(abs(leftFront), abs(rightFront), abs(leftBack), abs(rightBack));

  if (leftFront < 0) {
    FL_bck();
  } else {
    FL_fwd();
  }

  if (rightFront < 0) {
    FR_bck();
  } else {
    FR_fwd();
  }

  if (leftBack < 0) {
    RL_bck();
  } else {
    RL_fwd();
  }

  if (rightBack < 0) {
    RR_bck();
  } else {
    RR_fwd();
  }
}

void fwd(int pin1, int pin2) {
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, HIGH);
}

void back(int pin1, int pin2) {
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, LOW);
}

void FR_fwd()  //front-right wheel forward turn
{
  fwd(RightMotorDirPin1, RightMotorDirPin2);
}

void FR_bck() // front-right wheel backward turn
{
  back(RightMotorDirPin1, RightMotorDirPin2);
}

void FL_fwd() // front-left wheel forward turn
{
  fwd(LeftMotorDirPin1, LeftMotorDirPin2);
}

void FL_bck() // front-left wheel backward turn
{
  back(LeftMotorDirPin1, LeftMotorDirPin2);
}

void RR_fwd()  //rear-right wheel forward turn
{
  fwd(RightMotorDirPin1B, RightMotorDirPin2B);
}

void RR_bck()  //rear-right wheel backward turn
{
  back(RightMotorDirPin1B, RightMotorDirPin2B);
}

void RL_fwd()  //rear-left wheel forward turn
{
  fwd(LeftMotorDirPin1B, LeftMotorDirPin2B);
}

void RL_bck()    //rear-left wheel backward turn
{
  back(LeftMotorDirPin1B, LeftMotorDirPin2B);
}

/*detection of ultrasonic distance*/
int watch() {
  long echo_distance;
  digitalWrite(Trig_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(Trig_PIN, HIGH);
  delayMicroseconds(15);
  digitalWrite(Trig_PIN, LOW);
  echo_distance = pulseIn(Echo_PIN, HIGH);
  echo_distance = echo_distance * 0.01657; //how far away is the object in cm
  //Serial.println((int)echo_distance);
  return round(echo_distance);
}

//Measures distances to the left,center,right return a
int watchLeftRight() {
  /*  obstacle_status is a binary integer, its last 3 digits stands for if there is any obstacles in left front,direct front and right front directions,
       3 digit string, for example 100 means front left front has obstacle, 011 means direct front and right front have obstacles
  */
  int obstacle_status = B000;

  head.write(150); //sensor facing left front
  delay(WATCH_DELAY);
  distance = watch();
  if (distance < OBSTACLE_LIMIT) {
    obstacle_status  = obstacle_status | B100;
  }

  head.write(90); //sensor facing direct front
  delay(WATCH_DELAY);
  distance = watch();
  if (distance < OBSTACLE_LIMIT) {
    obstacle_status  = obstacle_status | B010;
  }

  head.write(30); //sensor faces to right front
  delay(WATCH_DELAY);
  distance = watch();
  if (distance < OBSTACLE_LIMIT) {
    obstacle_status  = obstacle_status | B001;
  }

  return obstacle_status;
}

//Measures distances to the left,center,right return a
int watchRightLeft() {
  /*  obstacle_status is a binary integer, its last 3 digits stands for if there is any obstacles in left front,direct front and right front directions,
       3 digit string, for example 100 means front left front has obstacle, 011 means direct front and right front have obstacles
  */
  int obstacle_status = B000;

  head.write(30); //sensor faces to right front
  delay(WATCH_DELAY);
  distance = watch();
  if (distance < OBSTACLE_LIMIT) {
    obstacle_status  = obstacle_status | B001;
  }

  head.write(90); //sensor facing direct front
  delay(WATCH_DELAY);
  distance = watch();
  if (distance < OBSTACLE_LIMIT) {
    obstacle_status  = obstacle_status | B010;
  }

  head.write(150); //sensor facing left front
  delay(WATCH_DELAY);
  distance = watch();
  if (distance < OBSTACLE_LIMIT) {
    obstacle_status  = obstacle_status | B100;
  }

  return obstacle_status;
}

//Measures distances to the left,center,right return a
int watchAhead() {
  int obstacle_status = hardTurn;

  head.write(90); //sensor facing direct front
  delay(WATCH_DELAY);
  distance = watch();
  if (distance < OBSTACLE_LIMIT) {
    obstacle_status  = obstacle_status | B010;
  } else {
    obstacle_status = B000;
  }

  return obstacle_status;
}

int watchSurround() {
  if (hardTurn != 0) {
    return watchAhead();
  }
  
  if (scanDir == LEFT_RIGHT) {
    scanDir = RIGHT_LEFT;
    return watchLeftRight();
  }

  scanDir = LEFT_RIGHT;
  return watchRightLeft();
}


void auto_avoidance() {
  int obstacle_sign = watchSurround(); // 3 bits of obstacle_sign binary value means the 3 direction obstacle status
  Serial.print("begin str=");
  Serial.println(String(obstacle_sign, BIN));
  if ( obstacle_sign == B100) {
    Serial.println("slight right");
    drive(FAST_SPEED, SPEED, FAST_SPEED, SPEED);
    return;
  }

  if (obstacle_sign == B001) {
    Serial.println("slight left");
    drive(SPEED, FAST_SPEED, SPEED, FAST_SPEED);
    return;
  }

  if (obstacle_sign == B110) {
    Serial.println("hard right");
    drive(-TURN_SPEED, TURN_SPEED, -TURN_SPEED, TURN_SPEED);
    hardTurn = HARD_RIGHT;
    return;
  }

  if (obstacle_sign == B011) {
    Serial.println("hard left");
    drive(TURN_SPEED, -TURN_SPEED, TURN_SPEED, -TURN_SPEED);
    hardTurn = HARD_LEFT;
    return;
  }

  if (obstacle_sign == B010) {
    dead_turn ++;

    if (dead_turn > 5) {
      dead_turn = -5;
    }

    if (dead_turn < 0) {
      Serial.println("hard left");
      drive(TURN_SPEED, -TURN_SPEED, TURN_SPEED, -TURN_SPEED);
      hardTurn = HARD_LEFT;
    } else {
      Serial.println("hard right");
      drive(-TURN_SPEED, TURN_SPEED, -TURN_SPEED, TURN_SPEED);
      hardTurn = HARD_RIGHT;
    }
    return;
  }

  if (obstacle_sign == B111) {
    dead_turn ++;

    if (dead_turn > 5) {
      dead_turn = -5;
    }

    if (dead_turn < 0) {
      Serial.println("hard back right");
      drive(-FAST_SPEED, -SPEED, -FAST_SPEED, -SPEED);
    } else {
      Serial.println("hard back left");
      drive(-SPEED, -FAST_SPEED, -SPEED, -FAST_SPEED);
    }
    return;
  }

  if ( obstacle_sign == B000 || obstacle_sign == B101 ) {
    Serial.println("go ahead");
    drive(SPEED, SPEED, SPEED, SPEED);
    hardTurn = B000;
    return;
  }
}

void setup() {
  /*setup L298N pin mode*/
  pinMode(RightMotorDirPin1, OUTPUT);
  pinMode(RightMotorDirPin2, OUTPUT);
  pinMode(speedPinL, OUTPUT);

  pinMode(LeftMotorDirPin1, OUTPUT);
  pinMode(LeftMotorDirPin2, OUTPUT);
  pinMode(speedPinR, OUTPUT);
  pinMode(RightMotorDirPin1B, OUTPUT);
  pinMode(RightMotorDirPin2B, OUTPUT);
  pinMode(speedPinLB, OUTPUT);

  pinMode(LeftMotorDirPin1B, OUTPUT);
  pinMode(LeftMotorDirPin2B, OUTPUT);
  pinMode(speedPinRB, OUTPUT);
  stop_Stop();//stop move
  /*init HC-SR04*/
  pinMode(Trig_PIN, OUTPUT);
  pinMode(Echo_PIN, INPUT);
  /*init buzzer*/

  digitalWrite(Trig_PIN, LOW);
  /*init servo*/
  head.attach(SERVO_PIN);

  head.write(90);
  delay(2000);

  Serial.begin(9600);

  stop_Stop();//Stop

}

void loop() {
  auto_avoidance();
}
