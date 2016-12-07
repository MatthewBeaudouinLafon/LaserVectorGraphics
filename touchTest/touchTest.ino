/*=================================
  This code demostrates 4-Wire Touch screen
  interfacing with Arduino
  blog.circuits4you.com
  www.circuits4you.com
  4- Wire Touchscreen Connections
  A0=====X+
  A1=====X-
  A2=====Y+
  A3=====Y-
  =================================*/

#include <Servo.h>
//Define your Touch screen connections
const byte Y1 = A0;
const byte X1 = A1;
const byte Y2 = A2;
const byte X2 = A3;
const byte TOUCH = A5;
const byte LASER = 12;
int Ymax = 340;
int Ymin = 35;
int Xmax = 540;
int Xmin = 20;
int X;
int Y;
int X_measured;
int Y_measured;
Servo panservo;
Servo tiltservo;

//Define your screen resolution as per your Touch screen (Max: 1024)
#define Xresolution 1024 //128
#define Yresolution 512 //64

//All of the variables needed to take a rolling average
const byte LM_SIZE = 10;


void setup()
{
  pinMode(LASER, OUTPUT);
  panservo.attach(10);
  tiltservo.attach(11);
  Serial.begin(9600);
  while (!Serial) {
    // wait for serial point to connect
  }
    Serial.println("BEGINNING");
    while (Serial.read()!='1'){
      //Wait for MATLAB to send a byte over serial to the Arduino
      //Allows us to hit Run on MATLAB and have the Arduino collect data again
    }
}

void loop() {
  if (isTouched()) {
    delay(5);
    while (isTouched()) {
      digitalWrite(LASER, HIGH);
      pinMode(TOUCH, INPUT);

      pinMode(Y1, INPUT);
      pinMode(Y2, INPUT);
      digitalWrite(Y2, LOW);
      pinMode(X1, OUTPUT);
      digitalWrite(X1, HIGH);
      pinMode(X2, OUTPUT);
      digitalWrite(X2, LOW);
      X_measured = (analogRead(Y2) - Xmin) * 1.0 / Xmax * 180; //Reads X axis touch position

      pinMode(X1, INPUT);
      pinMode(X2, INPUT);
      digitalWrite(X2, LOW);
      pinMode(Y1, OUTPUT);
      digitalWrite(Y1, HIGH);
      pinMode(Y2, OUTPUT);
      digitalWrite(Y2, LOW);
      Y_measured = 90 - (analogRead(X1) - Ymin) * 1.0 / Ymax * 90; //Reads Y axis touch position

      if (X_measured > 0 && Y_measured > 0)
      {
        X = runningAverageX(X_measured);
        Y = runningAverageY(Y_measured);
      }

      //If we've taken enough data points for the average to be full
      //        panservo.write(X);
      //        tiltservo.write(Y);
      String message = String(X);
      message += ',' + String(Y);
      Serial.println(message);
//      Serial.println(X);
//      Serial.println(Y);
//      Serial.println();

      delay(20);
    }
  }
  digitalWrite(LASER, LOW);


  runningAverageX(-2);
  runningAverageY(-2);
  panservo.write(X);
  tiltservo.write(Y);
}
boolean isTouched() {
  pinMode(Y1, INPUT);
  pinMode(X1, INPUT);
  pinMode(Y2, OUTPUT);
  digitalWrite(Y2, LOW);
  pinMode(X2, INPUT);
  pinMode(TOUCH, OUTPUT);
  digitalWrite(TOUCH, HIGH);
  int touch_reading = analogRead(X2);
  if (touch_reading < 100) {
    return true;
  }
  else {
    return false;
  }
}

//Taken from the Arduino Playground's Running Average page
//Has a different function each for X and Y
int runningAverageY(int M)
{
  static int LM_Y[LM_SIZE];
  static int index_y = 0;
  static long sum_y = 0;
  static int count_y = 0;

  if (M == -2)
  {
    memset(LM_Y, 0, sizeof(LM_Y)); //Clear out LM
    index_y = 0;
    sum_y = 0;
    count_y = 0;
    return 0;
  }

  else
  {
    sum_y -= LM_Y[index_y];
    if (sum_y < 0) sum_y = 0;
    LM_Y[index_y] = M;
    sum_y += LM_Y[index_y];
    index_y += 1;
    index_y = index_y % LM_SIZE;
    if (count_y < LM_SIZE) count_y += 1;
    return sum_y / count_y;
  }
}

int runningAverageX(int M)
{
  static int LM_X[LM_SIZE];
  static int index_x = 0;
  static long sum_x = 0;
  static int count_x = 0;

  if (M == -2)
  {
    memset(LM_X, 0, sizeof(LM_X)); //Clear out LM
    count_x = 0;
    sum_x = 0;
    index_x = 0;
    return 0;
  }

  else
  {
    sum_x -= LM_X[index_x];
    if (sum_x < 0) sum_x = 0;
    LM_X[index_x] = M;
    sum_x += LM_X[index_x];
    index_x += 1;
    index_x = index_x % LM_SIZE;
    if (count_x < LM_SIZE) count_x += 1;

    return sum_x / count_x;
  }
}

