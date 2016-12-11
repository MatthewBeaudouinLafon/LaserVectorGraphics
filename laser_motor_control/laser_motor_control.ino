
#include <Servo.h> 
 
Servo panServo;
Servo tiltServo;
 
float x = 1500;  // stores panServo position
float y = 1500;  // stores tiltServo position

// determines which switch case is executed in loop()
volatile byte mode = 0;

// for incoming serial data
byte data = 0;

// for size of incoming data list
volatile byte listSize = 0;

// stores position data from svg
byte *xs = (byte*) malloc(16);
byte *ys = (byte*) malloc(16);


void setup() 
{ 
  // set up servo objects
  panServo.attach(9, 1000, 2000);
  tiltServo.attach(10, 1000, 2000);
  // initialize serial communication at 9600 bps
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  //Serial.println("<Arduino is ready>");
}


void loop() 
{ 
  switch(mode)
  {
    // do nothing
    case 0:
      //Serial.println("fuck ");
      if (Serial.available() > 0) {
        cerealEvent();
      }
      break;
      
    // write mode
    case 1:
//      Serial.println("ok ");
      // loop through elements of list
      for (int i = 0; i < listSize; i++)
      {
        // scale elements of dxs and dys to servo microsecond range
        x = xs[i]*(500/200) + 1250.5;
        y = ys[i]*(500/100) + 1250.5;
        // write to servos, delay to ensure accurate tracing
        //Serial.println((int)x);
        //Serial.println((int)y);
        panServo.writeMicroseconds((int)x);
        tiltServo.writeMicroseconds((int)y);
        delay(20);
      }
      if (Serial.available() > 0) {
        cerealEvent();
      }
      break;
  }
  Serial.println(listSize);

}

void cerealEvent()
{
  mode = 1;
  if(Serial.peek() > 255)
  {
    listSize = 255;
    Serial.read();
  }
  else
  {
    listSize = Serial.read();
  }
  free(ys);
  free(xs);
  xs = (byte*) malloc(listSize);
  ys = (byte*) malloc(listSize);
  for (int i = 0; i < listSize; i++)
  {
    xs[i] = Serial.read();
    ys[i] = Serial.read();
  }
  while (Serial.available() > 0)
  {
    Serial.read();
  }
}
