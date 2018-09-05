#include <AccelStepper.h>

// Stepper Motor Variables and Setup
#define HALFSTEP 4                                                            // Defines the type of motor as a 4 wire motor
#define motorPin1  9                                                          // IN1 on the ULN2003 driver 1
#define motorPin2  10                                                         // IN2 on the ULN2003 driver 1
#define motorPin3  11                                                         // IN3 on the ULN2003 driver 1
#define motorPin4  12                                                         // IN4 on the ULN2003 driver 1
AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);  // Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48 Motor

unsigned int currentRes = 0;
unsigned int prevRes = 0;
int sensePin = 0;
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

// Initialize and set average noise variables and arrays to zero
  /*averageNoise = 0;                                                           // Set the initial average noise to zero
  for (int k=0; k<noiseArraySize; k++) {                                      // Start for loop to initialize the noise array to zero
    noiseArray[k] = 0;                                                        //   Sets current location in the noiseArray to zero
  }                                                                           // Ends for loop that initializes noise array to zero
*/
// Open the hand at startup to ensure a known starting position
  digitalWrite(13,HIGH);                                                      // Turn on the LED to indicate that the hand is busy
  stepper1.enableOutputs();                                                   // Enable the motor controller's outputs
  for (int k = 0; k < 1000; k++) {                                             // Start For Loop: Move the hand clockwise one step at a time
    stepper1.move(1);                                                         //   Set desired motor position one step ahead of the current position. 1 = one step clockwise from current location
    stepper1.run();                                                           //   Move motor one step towards desired location
    delay (1);                                                                //   Delay 1 ms to give the motor time to complete the move
  }                                                                           // End For Loop: Move the hand clockwise one step at a time
  stepper1.disableOutputs();                                                  // Disable the motor controller's outputs to save power
  digitalWrite(13,LOW);                                                       // Turn off the LED to indicate that the hand is ready
  handOpen = true;                                                            // Set the handOpen variable to 'true' to indicate that the hand is now open
  Serial.begin(9600);
/*
// Calculate the average background sound level 

// Fill up noiseArray with background sound values from analog pin 0
  for (int k = 0; k < noiseArraySize; k++) {                                  // Start For loop: Collect background sound data
    noiseArray[k] = analogRead(0);                                            //   Collect sound data from analog pin 0 and place it in noiseArray
    delay(1);                                                                 //   Delay 1 ms between sample collection to give a consistent sample rate
  }                                                                           // End For loop: Collect background sound data

// Calculate the average background noise
  for (int j = 0; j < noiseArraySize; j++) {                                  // Start For loop: Add all sound values together 
    noiseArraySum = noiseArraySum + noiseArray[j];                            //   Add each sound value to the total
  }                                                                           // End For loop: Add all sound values together

// Calculate the current average
  averageNoise = (float) noiseArraySum/noiseArraySize;                        // Calculate the average sound value by dividing the sum of the noise by the number of samples
  stepper1.disableOutputs();                                                  // Shut off all power to the motor to extend battery life

// Setup the override button
  pinMode(buttonActivate, INPUT_PULLUP);                                      // Set the ActivateButton as an input pin that is normally pulled high
  pinMode(buttonGND, OUTPUT);                                                 // Set the GndButton as an output pin, it will be set low to act as a ground
  digitalWrite(buttonGND, LOW);                                               // Write one of the button pins to low, giving a ground pin for the button
  attachInterrupt(0, changePinState, FALLING);                                // Attach an interrupt to pin 2 that will trigger only on a falling edge, when the button is pushed but not released.
                                                                            // End Setup Loop
*/
}
void loop() {                                                                 // Start Main Loop
  stepper1.disableOutputs();                                                  // Shut off all power to the motor to extend battery life 
  currentRes = analogRead(sensePin);
  Serial.println(currentRes);
  delay(2000);
 /* while(buttonPushed == false) {                                              // Start While loop: While the button has not been pushed check for muscle activation and move hand
    soundArray[0] = analogRead(0);                                            //   Record a sound value from analog pin 0, use it to determine is a muscle signal has be detected

// If sound sample is greater than 1.5 times the average noise then collect 
// and analize more data to determine is a muscle signal has been received
    if((float) soundArray[0] > 1.5*averageNoise) {                            //   Start If statement: If the sound data is loud enough to warrant further investigation
      for (int k = 1; k < soundArraySize; k ++) {                             //     Start For loop: Collect sound values
        soundArray[k] = analogRead(0);                                        //       Record a sound sample and save it to the sound array
        delay(5);                                                             //       Delay 5 ms, used to set sample frequency to 200 Hz
      }                                                                       //     End of For loop: Collect sound values
// In this section we find the Average of all samples as well as 
// the Maximum and Minimum sample
      signalMin = 1026;                                                       //     Set/re-set the minimum signal variable to 1026, the highest possible values this variable could be
      signalMax = 0;                                                          //     Set/re-set the maximum signal variable to 0, the lowest possible values this variable could be
      for (int k = 1; k < soundArraySize; k ++) {                             //     Start For loop: Determine signal's Average, Minimum, and Maximum
        signalAverage = signalAverage + soundArray[k];                        //       Sum up all the sound values 
        if(signalMin > soundArray[k]) {                                       //       Start If statement: Is the current signal less than the current minimum value we've seen?
          signalMin = soundArray[k];                                          //         Set this current value as the new minimum value seen
        }                                                                     //       End If statement: Is the current signal less than the current minimum value we've seen?
        if(signalMax < soundArray[k]) {                                       //       Start If statement: Is the current signal greater than the current maximum value we've seen?
          signalMax = soundArray[k];                                          //         Set this current value as the new minimum value seen
        }                                                                     //       End If statement: Is the current signal less than the current maximum value we've seen?
      }                                                                       //     End For loop: Determine signal's Average, Minimum, and Maximum

// Rewrite the sound array classifying each sample as a minimum (0), 
// maximum (100), or as neither (50)
      for(int k = 0; k< soundArraySize; k++) {                                //     Start For loop: Classify array as Max, Min, or Other
        if(soundArray[k] <= (signalMin + 5)) {                                //       Start If statement: Is the current sound sample within 5 of the lowest value?
          soundArray[k] = 0;                                                  //         Array element is a minimum
        }                                                                     //       End If statement: Is the current sound sample within 5 of the lowest values?
        else if(soundArray[k] >= (signalMax - 5)) {                           //       Start Else If statement: Is the current sample within 5 of the highest value?
          soundArray[k] = 100;                                                //         Array element is a maximum
        }                                                                     //       End Else If statement: Is the current sample within 5 of the highest value?
        else {                                                                //       Start of Else statement: Neither a maximum or minimum
          soundArray[k] = 50;                                                 //         Array element is something other than a maximum or minimum
        }                                                                     //       End Else statement: Neither a maximum or minimum
      }                                                                       //     End For loop: Classify array as Max, Min, or Other

//  Look through array to determine if a muscle movement has been 
//  detected, and if so should the hand be opened or closed. A muscle 
//  movement is defined as 4 maximums followed by 4 minimums, or 4 
//  minimums followed by 4 maximums. All other patterns are ignored
      for(int k = 3; k< (soundArraySize -4); k ++) {                          //     Start For loop: Determine if a muscle movement has occurred
        if(soundArray[k-3] == 0 && soundArray[k-2] == 0 && soundArray[k-1] == 0 
         && soundArray[k] == 0 && soundArray[k+1] == 100 && soundArray[k+2] 
         == 100 && soundArray[k+3] == 100 && soundArray[k+4] == 100) {        //       Start If statement: Are the previous 3 and current sample all minimums followed by 4 maximums
        */  if(currentRes>prevRes) {                  //         Start If statement: If hand is open close it
// Close the hand
            digitalWrite(13,HIGH);                                            //           Turn on on-board LED to indicate that the system is moving the hand
            stepper1.enableOutputs();                                         //           Enable the motor controller's outputs
            for (int k = 0; k < 1000; k++) {                                   //           Start For loop: Move the hand counterclockwise one step at a time
              stepper1.move(-1);                                              //             Set desired position relating to current position -1 = one step counterclockwise from current location
              stepper1.run();                                                 //             Move motor one step towards desired location
              delay (1);                                                      //             Delay 1 ms to give the motor time to complete the move
            }                                                                 //           End For loop: Move the hand counterclockwise one step at a time
            digitalWrite(13,LOW);                                             //           Turn off on-board LED to indicate that the hand has stopped moving
            handOpen = false;                                                 //           Set the handOpen variable to 'false' to indicate that the hand is now closed
            stepper1.disableOutputs();                                        //           Shut off all power to the motor to extend battery life  
          //goto bailout;                                                     //           Exit out of If statement: If the sound data is loud enough to warrant further investigation
          }                                                                   //         End If statement: If hand is open close it
          //else if(handOpen == false){                   //    Start Else If statement: If hand is closed open it
          else if(currentRes<prevRes){
// Open the hand
            digitalWrite(13,HIGH);                                            //           Turn on on-board LED to indicate that the system is moving the hand
            stepper1.enableOutputs();                                         //           Enable the motor controller's outputs
            for (int k = 0; k < 1000; k++) {                                   //           Start For loop: Move the hand clockwise one step at a time
              stepper1.move(1);                                               //             Set desired position relating to current position 1 = one step clockwise from current location
              stepper1.run();                                                 //             Move motor one step towards desired location
              delay (1);                                                      //             Delay 1ms to give the motor time to complete the move
            }                                                                 //           End For loop: Move the hand clockwise one step at a time   
            digitalWrite(13,LOW);                                             //           Turn off on-board LED to indicate that the hand has stopped moving
            handOpen = true;                                                  //           Set the handOpen variable to 'true' to indicate that the hand is now open
            stepper1.disableOutputs();                                        //           Shut off all power to the motor to extend battery life  
          //goto bailout;                                                     //           Exit out of If statement: If the sound data is loud enough to warrant further investigation
          } 
          prevRes = currentRes;
          /*                                                                 //         End Else If statement: If hand is closed open it
        }                                                                     //       End If statement: Are the previous 3 and current sample all minimums followed by 4 maximums?
      
      if(soundArray[k-3] == 100 && soundArray[k-2] == 100 && soundArray[k-1] 
       == 100 && soundArray[k] == 100 && soundArray[k+1] == 0 && 
       soundArray[k+2] == 0 && soundArray[k+3] == 0 && soundArray[k+4] == 0){ //       Start If statement: Are the previous 3 and current sample all maximums followed by 4 minimums
        if(handOpen == true) {                                                //         Start If statement: If the hand is open, close it
// Close Hand
          digitalWrite(13,HIGH);                                              //           Turn on on-board LED to indicate that the system is moving the hand
          stepper1.enableOutputs();                                           //           Enable the motor controller's outputs
          for (int k = 0; k < 1000; k++) {                                     //           Start For loop: Move the hand counterclockwise one step at a time
            stepper1.move(-1);                                                //             Set desired position relating to current position -1 = one step counterclockwise from current location
            stepper1.run();                                                   //             Move motor one step towards desired location
            delay (1);                                                        //             Delay 1ms to give the motor time to complete the move
          }                                                                   //           End For loop: Move the hand counterclockwise one step at a time
          digitalWrite(13,LOW);                                               //           Turn off on-board LED to indicate that the hand has stopped moving
          handOpen = false;                                                   //           Set the handOpen variable to 'false' to indicate that the hand is now closed
          stepper1.disableOutputs();                                          //           Shut off all power to the motor to extend battery life  
          goto bailout;                                                       //           Exit out of If statement: If the sound data is loud enough to warrant further investigation
        }                                                                     //         End If statement: If the hand is open, close it
        else if(handOpen == false) {                                          //         Start Else If statement: If the hand is closed, open it
          digitalWrite(13,HIGH);                                              //           Turn on on-board LED to indicate that the system is moving the hand
          stepper1.enableOutputs();                                           //           Enable the motor controller's outputs
          for (int k = 0; k < 5000; k++) {                                     //           Start For loop: Move the hand clockwise one step at a time
            stepper1.move(1);                                                 //             Set desired position relating to current position 1 = one step clockwise from current location
            stepper1.run();                                                   //             Move motor one step towards desired location
            delay (1);                                                        //             Delay 1ms to give the motor time to complete the move
          }                                                                   //           End For loop: move the hand clockwise one step at a time
          digitalWrite(13,LOW);                                               //           Turn off on-board LED to indicate that the hand has stopped moving
            handOpen = true;                                                  //           Set the handOpen variable to 'true' to indicate that the hand is now open
          stepper1.disableOutputs();                                          //           Shut off all power to the motor to extend battery life  
          goto bailout;                                                       //           Exit out of If statement: If the sound data is loud enough to warrant further investigation
        }                                                                     //         End Else If statement: If the hand is closed, open it
      }                                                                       //       End If statement: Are the previous 3 and current sample all maximums followed by 4 minimums
    } */                                                                        //     End For loop: Determine if a muscle movement has occurred, see line 127
    bailout:                                                                  //     If a bailout condition has been met continue executing code from this point
    delay(100);                                                               //     Delay 100 ms before reading a new signal  
    digitalWrite(13,LOW);                                                     //     Turn off on-board LED to indicate that the hand has stopped moving
    }  /*                                                                       //   End If statement: Determine if data is loud enough to warrant further investigation, see line 90
  }                                                                           // End While loop: While the button has not been pushed check for muscle activation and move hand, see line 85
  
  if(buttonPushed == true && handOpen == true) {                              // Start If statement: If the override button has been pushed and the hand is open then close it
// Close Hand
    digitalWrite(13,HIGH);                                                    //   Turn on on-board LED to indicate that the system is moving the hand
    stepper1.enableOutputs();                                                 //   Enable the motor controller's outputs
    for (int k = 0; k < 5000; k++) {                                           //   Start For loop: Move the hand counterclockwise one step at a time
      stepper1.move(-1);                                                      //     Set desired position relating to current position -1 = one step counterclockwise from current location
      stepper1.run();                                                         //     Move motor one step towards desired location
      delay (1);                                                              //     Delay 1ms to give the motor time to complete the move
    }                                                                         //   End For loop: Move the hand counterclockwise one step at a time
    digitalWrite(13,LOW);                                                     //   Turn off on-board LED to indicate that the hand has stopped moving
    handOpen = false;                                                         //   Set the handOpen variable to 'false' to indicate that the hand is now closed
    stepper1.disableOutputs();                                                //   Shut off all power to the motor to extend battery life 
    buttonPushed = false;                                                     //   Clear the 'button has been pushed' flag
  }                                                                           // End If statement: If the override button has been pushed and the hand is open then close it
  
  else if(buttonPushed == true && handOpen == false) {                        // Start Else If statement: If the override button has been pushed and the hand is closed then open it
// Open the hand
    digitalWrite(13,HIGH);                                                    //   Turn on on-board LED to indicate that the system is moving the hand
    stepper1.enableOutputs();                                                 //   Enable the motor controller's outputs
    for (int k = 0; k < 5000; k++) {                                           //   Start For loop: Move the hand clockwise one step at a time
      stepper1.move(1);                                                       //     Set desired motor position one step ahead of the current position. 1 = one step clockwise from current location
      stepper1.run();                                                         //     Move motor one step towards desired location
      delay (1);                                                              //     Delay 1 ms to give the motor time to complete the move
    }                                                                         //   End for loop: Move the hand clockwise one step at a time
    digitalWrite(13,LOW);                                                     //   Turn off on-board LED to indicate that the hand has stopped moving
    handOpen = true;                                                          //   Set the handOpen variable to 'true' to indicate that the hand is now open
    stepper1.disableOutputs();                                                //   Shut off all power to the motor to extend battery life 
    buttonPushed = false;                                                     //   Clear the 'button has been pushed' flag
  }                                                                           // End Else If statement: If the override button has been pushed and the hand is closed then open it
}                                                                             // End Main Loop

// Interrupt handler function used to indicate is the button has been pushed
void changePinState() {                                                       // Start interrupt handler routine, initiated when the override button is pushed
  buttonPushed = true;                                                       //   Set the 'button has been pushed' flag
}                                                                             // End interrupt handler routine, initiated when the override button is pushed
 */
