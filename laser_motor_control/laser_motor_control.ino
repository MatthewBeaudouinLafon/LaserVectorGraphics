
#include <Servo.h> 
 
Servo panServo;
Servo tiltServo;
 
float x = 1485;  // stores panServo position
float y = 1485;  // stores tiltServo position

// determines which switch case is executed in loop()
volatile byte mode = 0;

// for trash
char j[1];

// for size of incoming data list
byte sizeInput[2];
volatile int listSize;

// stores position data from svg
byte xs[800];
byte ys[800];
//byte *xs = (byte*) malloc(16);
//byte *ys = (byte*) malloc(16);


void setup() 
{ 
  // set up servo objects
  panServo.attach(3, 1000, 2000);
  tiltServo.attach(9, 1000, 2000);
  // initialize serial communication at 9600 bps
  Serial.begin(9600);
}


void loop() 
{ 
  switch(mode)
  {
    // do nothing
    case 0:
      while(Serial.available()<1);
      break;
      
    // write mode
    case 1:
      // loop through elements of list
      for (int i = 0; i < listSize; i++)
      {
        if(xs[i] == 201) // code for break between curves in x byte
        {
          // *turn laser off here*
          // write the following point to servos
          x = xs[i+1]*1.5 + 1335.5;
          y = ys[i+1]*1.5 + 1335.5;
          panServo.writeMicroseconds((int)x);
          tiltServo.writeMicroseconds((int)y);
          delay(ys[i]); // delay should be encoded in corresponding y byte
          // *turn laser on here*
          // increment counter cause we already wrote the next point
          i++;
        }
        else // all other cases
        {
          // scale elements of dxs and dys to servo microsecond range (approx 60 to 115 degrees)
          x = xs[i]*1.5 + 1335.5;
          y = ys[i]*1.5 + 1335.5;
          // write to servos, delay to ensure "accurate" tracing
          panServo.writeMicroseconds((int)x);
          tiltServo.writeMicroseconds((int)y);
          delay(8);
        }
      }
      break;
  }
  
//Serial.println(listSize);

}

void serialEvent()
// read mode
{
  // reads two listSize bytes. expected: listSize/255, listSize%255
  Serial.readBytes((char*)sizeInput,2);
  
  // reconstructs listSize from div and mod bytes
  listSize = sizeInput[0]*255 + sizeInput[1];
  
  // caps at 800, we don't have more memory
  if(listSize > 800 || listSize < 0) {
    listSize = 800;
  }
  
  // read listSize blocks of bytes. expected: xxxxx...,yyyyy...
  Serial.readBytes((char*)xs,listSize);
  Serial.readBytes((char*)ys,listSize);
  
  // flush any remaining data from buffer
  while (Serial.available() > 0) {
    Serial.readBytes(j,1);
  }
  
  // switch to write mode
  mode = 1;
}
