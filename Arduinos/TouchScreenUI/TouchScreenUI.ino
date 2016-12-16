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

////Screen boundaries
int Ymax = 340;
int Ymin = 35;
int Xmax = 540;
int Xmin = 20;

////Button Boundaries
//Upload
int UYmin = 0;
int UYmax = 0;
int UXmin = 0;
int UXmax = 0;

//Cancel
int CYmin = 0;
int CYmax = 0;
int CXmin = 0;
int CXmax = 0;

//Next
int NYmin = 0;
int NYmax = 0;
int NXmin = 0;
int NXmax = 0;

//Previous
int PYmin = 0;
int PYmax = 0;
int PXmin = 0;
int PXmax = 0;

int X;
int Y;
int X_measured;
int Y_measured;
bool READX = false;
bool READY = true;

int toSerial[800];
int serialIdx = 0;
int serialSize = 0;

//Define your screen resolution as per your Touch screen (Max: 1024)
#define Xresolution 1024 //128
#define Yresolution 512 //64

//All of the variables needed to take a rolling average
const byte LM_SIZE = 10;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    // wait for serial connection to take place.
  }
}

void loop() {
  if (isTouched())
  {
    delay(5);
    while (isTouched())
    {
      // Read X and Y
      X_measured = measure(READX);
      Y_measured = measure(READY);
      
      //// drawing
      if (isInBounds(X_measured, Xmin, Xmax) && isInBounds(Y_measured, Ymin, Ymax)) // Inside the drawing bounds
      {
        // Add Point
        toSerial[serialIdx] = runningAverageX(X_measured);
        toSerial[serialIdx+1] = runningAverageX(Y_measured);
        serialIdx += 2;
      }
      
      //// upload 
      else if (isInBounds(X_measured, UXmin, UXmax) && isInBounds(Y_measured, UYmin, UYmax))
      {
        // send points over serial
        for (int i = 0; i <= serialIdx; i++)
        {
          Serial.println(toSerial[i]);
        }
      }
      //// cancel
      else if (isInBounds(X_measured, CXmin, CXmax) && isInBounds(Y_measured, CYmin, CYmax))
      {
        int serialIdx = 0; // This will allow us to overwrite previous data
      }
      //// next
      else if (isInBounds(X_measured, NXmin, NXmax) && isInBounds(Y_measured, NYmin, NYmax))
      {
        // send instruction over serial
        Serial.println(-2);
      }

      //// prev
      else if (isInBounds(X_measured, PXmin, PXmax) && isInBounds(Y_measured, PYmin, PYmax))
      {
        // send instruction over serial
        Serial.println(-3);
      }

      //// Python serial awake
      else 
      {
        Serial.println();
      }

    }
    // When the user no longer is touching the screen, we can add a stop character
    if (toSerial[serialIdx] != -1)
    {
      toSerial[serialIdx] = -1;
      serialIdx += 1;
    }
  }
}

bool isTouched() { // Returns whether the touch screen is receiving input
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

// Reads X if readX is passed, reads Y if readY is passed
int measure(bool axis) 
{  
  pinMode(TOUCH, INPUT);
  if (axis) // if READX
  {
    pinMode(Y1, INPUT);
    pinMode(Y2, INPUT);
    digitalWrite(Y2, LOW);
    pinMode(X1, OUTPUT);
    digitalWrite(X1, HIGH);
    pinMode(X2, OUTPUT);
    digitalWrite(X2, LOW);
    return ((analogRead(Y2) - Xmin) * 1.0 / Xmax * 180); //Reads X axis touch position
  } 
  else  // if READY
  {
    pinMode(X1, INPUT);
    pinMode(X2, INPUT);
    digitalWrite(X2, LOW);
    pinMode(Y1, OUTPUT);
    digitalWrite(Y1, HIGH);
    pinMode(Y2, OUTPUT);
    digitalWrite(Y2, LOW);
    return (90 - (analogRead(X1) - Ymin) * 1.0 / Ymax * 90); //Reads Y axis touch position
  }
}

bool isInBounds(int x, int a, int b) // returns whether a<=x<=b
{
  if (a <= x && x <= b) {
    return true;
  }
  return false;
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

















