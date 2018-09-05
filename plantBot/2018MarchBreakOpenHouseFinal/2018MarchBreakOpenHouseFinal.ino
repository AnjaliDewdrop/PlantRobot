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

// Override Button Variables
#define buttonGND 5                                                           // Defines the pin that will be held low, acting as a ground for the override button
#define buttonActivate 2                                                      // Defines the pin that will be pulled low when the button is pushed (to activate the hand)
volatile bool buttonPushed = false;                                           // Boolean Variable used to hold the current state of the override button. 'true' if the button has been pushed, 'false' when the program has handled a button press 

// Average Background Noise Variables
#define noiseArraySize 100                                                    // Defines the size of the array which will calculate the average background noise
unsigned int noiseArray[noiseArraySize];                                      // Positive Integer Array used to collected background sound data, used to calculate average sound level. Read from Analog Pin 0, with data ranging from 0 to 1023 (limit from analog pin)
float averageNoise;                                                           // Floating Point Variable used to hold the calculated average of the background noise. Floating variable values can be between -3.4028235E+38 and 3.4028235E+38
long int noiseArraySum;                                                       // Long Integer Variable to hold the sum all the noise values used to calculate the average background noise. Values can be between -2,147,483,648 and 2,147,483,647

// Muscle Signal Analysis Variables
#define soundArraySize 35                                                     // Defines the size of the array which is used to hold the sounds being evaluated
unsigned int soundArray[soundArraySize];                                      // Positive Integer Array used to collect the sound data being evaluated to determine if a muscle signal has been received. Read from Analog Pin 0, with data ranging from 0 to 1023 (limit from analog pin)
unsigned int signalMax;                                                       // Positive Integer Variable used to hold the maximum value of the signal under consideration. Value range: 0-65535
unsigned int signalMin;                                                       // Positive Integer Variable used to hold the minimum value of the signal under consideration. Value range: 0-65535
float signalAverage;                                                          // Floating Point Variable used to hold the calculated average of the signal under consideration. Floating variable values can be between -3.4028235E+38 and 3.4028235E+38
bool handOpen = true;                                                         // Boolean Variable used to hold the current position of the hand. 'true' hand is open, 'false' hand is closed

// Setup Loop
void setup() {                                                                // Start Setup Loop
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
  handOpen = true;                                                            // Set the handOpen variable to 'true' to indicate that the hand is now open

}
void loop() {                                                                 // Start Main Loop
  stepper1.disableOutputs();                                                  // Shut off all power to the motor to extend battery life 
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
  }                                                      //     End For loop: Determine if a muscle movement has occurred, see line 127
    bailout:                                                                  //     If a bailout condition has been met continue executing code from this point
    delay(100);                                                               //     Delay 100 ms before reading a new signal  
    digitalWrite(13,LOW);                                                     //     Turn off on-board LED to indicate that the hand has stopped moving
}  

