
#include <Servo.h> 
 
Servo panServo;
Servo tiltServo;
 
float x = 1500;  // stores panServo position
float y = 1500;  // stores tiltServo position

// determines which switch case is executed in loop()
byte mode = 0;

// for incoming serial data
float data = 0;

// stores position data from svg
float xs[] = {};
float ys[] = {};

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
      for (int i = 0; i < (sizeof(xs)/sizeof(int)); i++)
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
  while(Serial.available())
  {
    data = Serial.read();
  }
}
