
#include <Servo.h> 
 
Servo panServo;
Servo tiltServo;
 
float x = 1500;  // stores panServo position
float y = 1500;  // stores tiltServo position

// determines which switch case is executed in loop()
volatile byte mode = 0;

// for incoming serial data
byte data = 0;
// for trash
byte j[1];

// for size of incoming data list
byte sizeInput[2];
volatile byte listSize = 0;

// stores position data from svg
byte xs[1024];
byte ys[1024];
//byte *xs = (byte*) malloc(16);
//byte *ys = (byte*) malloc(16);


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
      while(Serial.available<1);
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
        panServo.writeMicroseconds((int)x);
        tiltServo.writeMicroseconds((int)y);
        delay(10);
      }
      break;
  }
  
Serial.println(listSize);

}

void serialEvent()
{
  if(Serial.peek() > 255)
  {
    listSize = 255;
    Serial.readBytes(j,1);
  }
  else
  {
    Serial.readBytes(sizeInput,1);
    listSize = sizeInput[0];
  }
//  free(ys);
//  free(xs);
//  xs = (byte*) malloc(listSize);
//  ys = (byte*) malloc(listSize);
//  for (int i = 0; i < listSize; i++)
//  {
//    xs[i] = Serial.read();
//    ys[i] = Serial.read();
//  }

  Serial.readBytes(xs,listSize);
  Serial.readBytes(ys,listSize);
  
  while (Serial.available() > 0)
  {
    Serial.readBytes(j,1);
  }
  mode = 1;
}
