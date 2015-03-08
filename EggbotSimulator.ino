#include <GSerialCommand.h>
#include <AccelStepper.h>
#include <Servo.h> 
 
// Define the stepper and the pins it will use
#define M1_DIR 3
#define M1_STP 2
#define M2_DIR 5
#define M2_STP 4
#define MS1 11        // PIN 11 = MS1
#define MS2 12        // PIN 12 = MS2
#define SERVO 8
AccelStepper motorA(1, M1_STP, M1_DIR);
AccelStepper motorB(1, M2_STP, M2_DIR);

Servo myservo;

long interval = 1000;  
GSerialCommand sCmd;     // The demo SerialCommand object
boolean turning = false;
long previousMillis = 0;
int hardwareStepsPerRev = 200;
int modeType = 4;
int STEPSPERREVOLUTION = hardwareStepsPerRev*modeType;
double MAXSPEED = 400.0;
double MAXACCELERATION = 400.0;
float ratio;

void setup() {
  
   Serial.begin(9600);
   Serial.println("OKinitialised");
   
   sCmd.addCommand("SM",     processCommand);  // Converts two arguments to integers and echos them back 
   sCmd.addCommand("SP", processCommand2);
   sCmd.addCommand("QB", returnposition);
   sCmd.addCommand("v", returnversion);
   sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")
   
   pinMode(MS1, OUTPUT);   // set pin 11 to output
   pinMode(MS2, OUTPUT);   // set pin 12 to output
   
   myservo.attach(SERVO);
   
    motorA.setMaxSpeed(MAXSPEED);
  motorA.setAcceleration(MAXACCELERATION);
  motorB.setMaxSpeed(MAXSPEED);
  motorB.setAcceleration(MAXACCELERATION);
  turning = false; 
  
}

void loop() {
  //define the step mode
  digitalWrite(MS1, MS1_MODE(modeType));  // Set state of MS1 based on the returned value from the MS1_MODE() switch statement.
  digitalWrite(MS2, MS2_MODE(modeType));  // Set state of MS2 based on the returned value from the MS2_MODE() switch statement.
  
    if(turning == false)
    {
      unsigned long currentMillis = millis();
      sCmd.readSerial();    
      if(currentMillis - previousMillis > interval) 
      {
        previousMillis = currentMillis;  
      }
    }
    while(turning == true)
    {
      
      motorA.run();
      motorB.run();
       if (motorA.distanceToGo() == 0 && motorB.distanceToGo() == 0 && turning == true)
       {
       
          Serial.println("OK\n\r");
          turning = false;
       }
    }
    
}

void unrecognized(const char *command) 
{
  Serial.println("OK\n\r");
}


void processCommand() {
  float x;
  float d; 
  char *arg;
  arg = sCmd.next();
  if (arg != NULL) 
  {
    d = atof(arg);    
  }
  
  arg = sCmd.next();
  if (arg != NULL) 
  {
    x = atof(arg);    
  }
  
  float y;
  arg = sCmd.next();
  if (arg != NULL) 
  {
    y = atof(arg);
    if( x == 0 && y == 0)
    {
      delay(d);
      Serial.println("OK\n\r");
    }
    else
    {
      ratio = x/y;
       
  
      if (ratio >=1 || y==0 && x>0)
      {
        //x>y
        motorA.setMaxSpeed(MAXSPEED);
        motorA.setAcceleration(MAXACCELERATION);
        motorB.setMaxSpeed(MAXSPEED/ratio);
        motorB.setAcceleration(MAXACCELERATION/ratio);
      }
      if (ratio <1 && ratio >0)
      {
        //y>x
        motorA.setMaxSpeed(MAXSPEED*ratio);
        motorA.setAcceleration(MAXACCELERATION*ratio);
        motorB.setMaxSpeed(MAXSPEED);
        motorB.setAcceleration(MAXACCELERATION);
      }
      if (ratio >=-1 && ratio <0)
      {
        //x>y
        motorA.setMaxSpeed(MAXSPEED*-ratio);
        motorA.setAcceleration(MAXACCELERATION*-ratio);
        motorB.setMaxSpeed(MAXSPEED);
        motorB.setAcceleration(MAXACCELERATION);
      }
      if (ratio< -1)
      {
        //y>x
        motorA.setMaxSpeed(MAXSPEED*-ratio);
        motorA.setAcceleration(MAXACCELERATION*-ratio);
        motorB.setMaxSpeed(MAXSPEED);
        motorB.setAcceleration(MAXACCELERATION);
      }
      if (y==0 && x<0)
      {
        
        motorA.setMaxSpeed(MAXSPEED);
        motorA.setAcceleration(MAXACCELERATION);
      }
      if(y != 0)
      {
        motorB.move(y*5.688*360/STEPSPERREVOLUTION);
      }
      if (x!= 0)
      {
        motorA.move(x*5.688*360/STEPSPERREVOLUTION);
  
      }
      turning = true;
    }
  }
}

 void returnposition()
 {
   Serial.println("0");
   Serial.println("OK\n\r");
 }
 
 void returnversion()
 {
   Serial.println("EBB Version 2.1-");
   Serial.println("OK\n\r");
 }


 void processCommand2() {
   char *arg;
   arg = sCmd.next();
   int iNumber = atoi(arg);
   
   if (arg != NULL) 
   {
     iNumber = atoi(arg); 
       if (iNumber == 1)
       {
         myservo.write(0);
       }
       else
       {
        myservo.write(180);
       }
   }
   Serial.println("OK\n\r");
}

int MS1_MODE(int MS1_StepMode){              // A function that returns a High or Low state number for MS1 pin
  switch(MS1_StepMode){                      // Switch statement for changing the MS1 pin state
  case 1:
    MS1_StepMode = 0;
    break;
  case 2:
    MS1_StepMode = 1;
    break;
  case 4:
    MS1_StepMode = 0;
    break;
  case 8:
    MS1_StepMode = 1;
    break;
  }
  return MS1_StepMode;
}

int MS2_MODE(int MS2_StepMode){              // A function that returns a High or Low state number for MS2 pin
  switch(MS2_StepMode){                      // Switch statement for changing the MS2 pin state
                                             // Different input states allowed are 1,2,4 or 8
  case 1:
    MS2_StepMode = 0;
    break;
  case 2:
    MS2_StepMode = 0;
    break;
  case 4:
    MS2_StepMode = 1;
    break;
  case 8:
    MS2_StepMode = 1;
    break;
  }
  return MS2_StepMode;
} 

