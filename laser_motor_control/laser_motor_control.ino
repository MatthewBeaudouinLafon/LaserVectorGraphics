
#include <Servo.h> 
 
Servo panServo;
Servo tiltServo;
 
int x = 0;  // stores panServo position
int y = 0;  // stores tiltServo position

int dxs[] = {1, 3, 5, 7, 9};
int dys[] = {10, 8, 6, 4, 2};
 
void setup() 
{ 
  // set up servo objects
  panServo.attach(9, 1000, 2000);
  tiltServo.attach(10, 1000, 2000);
} 
 
void loop() 
{ 
  for (int i = 0; i < (sizeof(dxs)/sizeof(int)); i++)
  {
    float xInput = dxs[i]*(1000/180) + 1000.5;
    float yInput = dys[i]*(1000/180) + 1000.5;
    panServo.writeMicroseconds((int)xInput);
    tiltServo.writeMicroseconds((int)yInput);
  }
} 
