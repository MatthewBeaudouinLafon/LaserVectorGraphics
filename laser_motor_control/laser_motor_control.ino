
#include <Servo.h> 
 
Servo panServo;
Servo tiltServo;
 
float x = 1500;  // stores panServo position
float y = 1500;  // stores tiltServo position

// determines which switch case is executed in loop()
byte mode = 0;

// for incoming serial data
byte data = 0;

// for size of incoming data list
byte listSize = 0;

// stores position data from svg
byte* xs = (byte*) malloc(16);
byte* ys = (byte*) malloc(16);


void setup() 
{ 
  // set up servo objects
  panServo.attach(9, 1000, 2000);
  tiltServo.attach(10, 1000, 2000);
  // initialize serial communication at 9600 bps
  Serial.begin(9600);
}


void loop() 
{ 
  switch(mode)
  {
    // do nothing
    case 0:
      ;
      break;
      
    // write mode
    case 1:
      // loop through elements of xs
      for (int i = 0; i < sizeof(xs); i++)
      {
        // scale elements of dxs and dys to servo microsecond range
        x = xs[i]*(500/200) + 1250.5;
        y = ys[i]*(500/100) + 1250.5;
        // write to servos, delay to ensure accurate tracing
        panServo.writeMicroseconds((int)x);
        tiltServo.writeMicroseconds((int)y);
        delay(5);
      }
      break;
  }
}

void serialEvent()
{
  if(Serial.peek() > 256)
  {
    listSize = 256;
    Serial.read();
  }
  else
  {
    listSize = Serial.read();
  }
  free(ys);
  free(xs);
  xs = (byte*) calloc(listSize, 1);
  ys = (byte*) calloc(listSize, 1);
  for (int i = 0; i < listSize; i++)
  {
    xs[i] = Serial.read();
    ys[i] = Serial.read();
  }
  while (Serial.available() > 0)
  {
    Serial.read();
  }
  mode = 1;
}
