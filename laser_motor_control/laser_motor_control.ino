
#include <Servo.h> 
 
Servo panServo;
Servo tiltServo;
 
float x = 1500;  // stores panServo position
float y = 1500;  // stores tiltServo position

// determines which switch case is executed in loop()
volatile byte mode = 0;

// for incoming serial data
//byte data = 0;

// for trash
char j[1];

// for size of incoming data list
byte sizeInput[2];
volatile byte listSize = 0;

// stores position data from svg
byte xs[800];
byte ys[800];
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
//  Serial.println("<Arduino is ready>");
}


void loop() 
{ 
  switch(mode)
  {
    // do nothing
    case 0:
//      Serial.println("case is 0");
      while(Serial.available()<1);
      break;
      
    // write mode
    case 1:
//      Serial.println("case is 1");
      // loop through elements of list
      for (int i = 0; i < listSize; i++)
      {
        // scale elements of dxs and dys to servo microsecond range
        x = xs[i]*5 + 1000;
        y = ys[i]*5 + 1000;
        // write to servos, delay to ensure accurate tracing
        panServo.writeMicroseconds((int)x);
        tiltServo.writeMicroseconds((int)y);
//        Serial.println(xs[i]);
//        Serial.println(ys[i]);
        delay(5);
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
    Serial.readBytes((char*)sizeInput,1);
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

  Serial.readBytes((char*)xs,listSize);
  Serial.readBytes((char*)ys,listSize);
  
  while (Serial.available() > 0)
  {
    Serial.readBytes(j,1);
  }
  mode = 1;
}
