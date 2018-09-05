#include <AccelStepper.h>

// Stepper Motor Variables and Setup
#define HALFSTEP 4                                                            // Defines the type of motor as a 4 wire motor
#define motorPin1  9                                                          // IN1 on the ULN2003 driver 1
#define motorPin2  10                                                         // IN2 on the ULN2003 driver 1
#define motorPin3  11                                                         // IN3 on the ULN2003 driver 1
#define motorPin4  12                                                         // IN4 on the ULN2003 driver 1
AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);  // Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48 Motor


//Diode
int sensePin=0;
int brightest = 0;
int stepCount = 0;
int brightestPos=0;
int totalSteps=30000;



// Setup Loop
void setup() {
  //DiodeSetUp
  analogReference(DEFAULT); //isn't necessary
  Serial.begin(9600);

  
  
  // Set motor's maximum speed and acceleration
  stepper1.setMaxSpeed(100000);                                               // Set the maximum speed of the motor
  stepper1.setAcceleration(300000.0);                                         // Set the maximum acceleration of the motor
  
// Setup the Arudino's on board LED pin so it can be used to indicate when 
// the system is ready to process a signal
  pinMode(LED_BUILTIN, OUTPUT);                                               // Set the pin connected to the arduino's on board LED to an output so it can turn that LED on or off. The Pin is Digital Pin #13

// Open the hand at startup to ensure a known starting position
  digitalWrite(13,HIGH);                                                      // Turn on the LED to indicate that the hand is busy
  stepper1.enableOutputs();                                                   // Enable the motor controller's outputs
  for (int k = 0; k < 5000; k++) {                                             // Start For Loop: Move the hand clockwise one step at a time
    stepper1.move(1);                                                         //   Set desired motor position one step ahead of the current position. 1 = one step clockwise from current location
    stepper1.run();                                                           //   Move motor one step towards desired location
    delay (1);                                                                //   Delay 1 ms to give the motor time to complete the move
  }                                                                           // End For Loop: Move the hand clockwise one step at a time
  stepper1.disableOutputs();                                                  // Disable the motor controller's outputs to save power
  digitalWrite(13,LOW);                                                       // Turn off the LED to indicate that the hand is ready

}
void loop() {   
  //forDiode
  Serial.println(analogRead(sensePin));
  delay(500);

  while (stepCount<totalSteps) {
    if (analogRead(sensePin)>brightest){
      brightest=analogRead(sensePin);
      brightestPos=stepCount;
    }
    //move 1;
   
    digitalWrite(13,HIGH);                                            //           Turn on on-board LED to indicate that the system is moving the hand
    stepper1.enableOutputs();                                         //           Enable the motor controller's outputs
    stepper1.move(1);                                              //             Set desired position relating to current position -1 = one step counterclockwise from current location
    stepper1.run();                                                 //             Move motor one step towards desired location
    delay (1);   
    stepCount++;
  }
  
  stepper1.enableOutputs();
  while(stepCount>brightestPos)
  {
    stepper1.move(-1);
    stepper1.run();
    stepCount--;
    delay(1);
  }
  
  
  digitalWrite(13,LOW);                                             //           Turn off on-board LED to indicate that the hand has stopped moving
  stepper1.disableOutputs();                                        //           Shut off all power to the motor to extend battery life  
  goto bailout;
  
  
  
  /*stepper1.disableOutputs();                                                  // Shut off all power to the motor to extend battery life 
  if(handOpen == true) {                                              //         Start If statement: If hand is open close it
        // Close the hand
        digitalWrite(13,HIGH);                                            //           Turn on on-board LED to indicate that the system is moving the hand
        stepper1.enableOutputs();                                         //           Enable the motor controller's outputs
        for (int k = 0; k < 5000; k++) {                                   //           Start For loop: Move the hand counterclockwise one step at a time
          stepper1.move(-1);                                              //             Set desired position relating to current position -1 = one step counterclockwise from current location
          stepper1.run();                                                 //             Move motor one step towards desired location
          delay (1);                                                      //             Delay 1 ms to give the motor time to complete the move
        }                                                                 //           End For loop: Move the hand counterclockwise one step at a time
        digitalWrite(13,LOW);                                             //           Turn off on-board LED to indicate that the hand has stopped moving
        handOpen = false;                                                 //           Set the handOpen variable to 'false' to indicate that the hand is now closed
        stepper1.disableOutputs();                                        //           Shut off all power to the motor to extend battery life  
        goto bailout;                                                     //           Exit out of If statement: If the sound data is loud enough to warrant further investigation
  }                                                                   //         End If statement: If hand is open close it
  else if(handOpen == false) {                                        //         Start Else If statement: If hand is closed open it
// Open the hand
    digitalWrite(13,HIGH);                                            //           Turn on on-board LED to indicate that the system is moving the hand
    stepper1.enableOutputs();                                         //           Enable the motor controller's outputs
    for (int k = 0; k < 5000; k++) {                                   //           Start For loop: Move the hand clockwise one step at a time
      stepper1.move(1);                                               //             Set desired position relating to current position 1 = one step clockwise from current location
      stepper1.run();                                                 //             Move motor one step towards desired location
      delay (1);                                                      //             Delay 1ms to give the motor time to complete the move
    }                                                                 //           End For loop: Move the hand clockwise one step at a time   
    digitalWrite(13,LOW);                                             //           Turn off on-board LED to indicate that the hand has stopped moving
    handOpen = true;                                                  //           Set the handOpen variable to 'true' to indicate that the hand is now open
    stepper1.disableOutputs();                                        //           Shut off all power to the motor to extend battery life  
    goto bailout;                                                     //           Exit out of If statement: If the sound data is loud enough to warrant further investigation
  }*/                                                      //     End For loop: Determine if a muscle movement has occurred, see line 127
    bailout:                                                                  //     If a bailout condition has been met continue executing code from this point
    delay(100);                                                               //     Delay 100 ms before reading a new signal  
    digitalWrite(13,LOW);                                                     //     Turn off on-board LED to indicate that the hand has stopped moving
}  

