#include "math.h"
/*    ____________________________________________________________________________________________________
 *    ____ClearPath_Provided_Mode_Information_____________________________________________________________
 *    Selected Mode - Pulse Burst Positioning
 *      Description - ClearPath will move a distance proportional to the number of pulses sent to Input B.
 *                    The direction of movement is determined by the state of Input A. The speed is user-
 *                    definable. This mode offers most of the flexibility of a "step and direction" system,
 *                    but doesn't require an expensive indexer to create smoothly ramped move trajectories.
 *                    A simple counter/timer of a PLC or a software loop can generate the pulses.
 *   
 *    Input Actions... 
 *      Enable  - Asserting this input energizes the motor shaft. A short pulse (user-definable) on this
 *                input tells ClearPath to use the alternate speed limit setting for the next move.
 *      Input A - This input selects the direction of rotation.
 *      Input B - This input is connected to pulse train where the number of pulses represents the desired
 *                incremental distance for the move.

 *    Helpful Hints             
 *    * The frequency of the pulse train is generally kept constant. The acceleration and deceleration is
 *      handled by ClearPath; there is no need for the frequency to ramp up and down like in a step and
 *      direction system.
 *    * The frequency of the pulse train applied to Input B must always be at a frequency higher than your 
 *      specified Speed Limit so that the motor is never "using" the pulses faster than they're being
 *      supplied. Otherwise, the motor will prematurely ramp to zero speed.
 *    * If a move is in progress and Input A (the direction) is changed, the pulses already sent will still
 *      be interpreted as being in the former direction. If more pulses are then sent, they will be 
 *      accumulated, and  ClearPath will make a move in the opposite direction after the current move 
 *      finishes.
 *    * Homing can be set up to occur only once (after the first enable), or each time ClearPath is enabled.
 *
 *    ____________________________________________________________________________________________________
 *    ____Arduino_Demo_Information________________________________________________________________________
 *    This demo enables the motor and then commands the motor to move following a specific pattern 
 *        (see below)
 *        
 *        The specific pattern of moves is as follows: 
 *                      + 10           *
 *                      - 20  (alt spd)          
 *                      + 30           
 *                      - 40           
 *                      + 50  (alt spd)         
 *                      - 60             
 *                      + 70           
 *                      - 80  (alt spd)         
 *                      + 90           
 *                      -100           
 *                      + 90  (alt spd) --> - 80              (One pulse stream immediately following the previous)               
 *                      + 70            --> - 60  (alt spd)         
 *                      + 50            --> - 40           
 *                      + 30  (alt spd) --> - 20         
 *                      + 10           
 *                                
 *    HLFB Implementation to be added later.
 *    
 *    ____________________________________________________________________________________________________
 */

//Define Arduino pin numbers for the connection to the ClearPath controller cable.
//Wire colors refer to a standard Teknic ClearPath controller cable 
const int Enable = 6; // ClearPath Enable Input;  +enable = BLU wire
const int Eneg = 7;   //                          -enable = ORN wire
const int InputA = 8; // ClearPath Input A;       +InputA = WHT wire
const int Aneg = 10;  //                          -InputA = BRN wire
const int InputB = 9; // ClearPath Input B;       +InputB = BLK wire
const int Bneg = 11;  //                          -InputB = YEL wire
const int HLFB = 4;   // ClearPath HLFB Output;   +HLFB   = GRN wire
const int Hneg = 5;   //                          -HLFB   = RED wire
// Alternately, you could connect all of the negative wires (ORN, BRN, YEL, RED) to ground (GND)
// This would free up pins 7, 10, 11, and 5 to be used for other purposes.

void MoveCommand(int distance,bool AltSpeed = 0);          // Function used to command increments

int DwellTime = 1000;     // desired time (ms) to wait before commanding another move
int HomeTime = 10000;     // time to wait for homing to complete
int MinAltPulse = 10;    // THIS MUST MATCH THE SETTINGS IN MSP
bool ReverseDirection = 0;    // THIS MUST MATCH THE SETTING IN MSP


// ============================================================================================
// put your setup code here, to run once:
void setup() 
{
  //Define I/O pins
  pinMode(Enable, OUTPUT);
  pinMode(InputA, OUTPUT);
  pinMode(InputB, OUTPUT);
  pinMode(HLFB, INPUT_PULLUP);
  //pinMode(HLFB, INPUT);
  pinMode(Eneg, OUTPUT);
  pinMode(Aneg, OUTPUT);
  pinMode(Bneg, OUTPUT);
  pinMode(Hneg, OUTPUT);
  
  //Start Serial output for dubugging
  Serial.begin(9600);
  
  // start off by ensuring that the motor is disabled before proceeding
  digitalWrite(Enable, LOW);
  delay(DwellTime);

  //create ground pins for negative wires
  digitalWrite(Eneg, LOW);
  digitalWrite(Aneg, LOW);
  digitalWrite(Bneg, LOW);
  digitalWrite(Hneg, LOW);
  
  // set the two outputs to their initial states
  digitalWrite(InputA, LOW);
  digitalWrite(InputB, LOW);
  
  // Enable motor to start sequence
  digitalWrite(Enable, HIGH);
  // Command Homing sequence
  delay(HomeTime);
  delay(DwellTime);
}
// end of setup code
// ========================================================================================================


// ========================================================================================================
// put your main code here, to loops indefinitely:
void loop()
{
  MoveCommand(  1000);     
  delay(DwellTime);
  MoveCommand( -2000,1);     
  delay(DwellTime);
  MoveCommand(  3000);     
  delay(DwellTime);
  MoveCommand( -4000);     
  delay(DwellTime);
  MoveCommand(  5000,1);     
  delay(DwellTime);
  MoveCommand( -6000);     
  delay(DwellTime);
  MoveCommand(  7000);     
  delay(DwellTime);
  MoveCommand( -8000,1);     
  delay(DwellTime);
  MoveCommand(  9000);     
  delay(DwellTime);
  MoveCommand(-10000);     
  delay(DwellTime);
  MoveCommand( 9000,1);     
  MoveCommand(-8000);     
  delay(DwellTime);
  MoveCommand( 7000);     
  MoveCommand(-6000,1);     
  delay(DwellTime);
  MoveCommand( 5000);     
  MoveCommand(-4000);     
  delay(DwellTime);
  MoveCommand( 3000,1);     
  MoveCommand(-2000);     
  delay(DwellTime);
  MoveCommand( 1000);         
  delay(DwellTime);
  
}
// end of main code, will loop indefinitely:
// ========================================================================================================


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Implementation of MoveCommand function
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void MoveCommand(int distance, bool AltSpeed)
{
  /*  MoveCommand - takes in the distance you would like to command, adjusts input A to accomodate the direction,
   *    and then pulses the Input B line a number of times equal to the magnitude of distance. AltSpeed is
   *    an optional TRUE/FALSE input that when true commands the motor to run at the alternate speed.
   */
  if(AltSpeed){
    digitalWrite(Enable,LOW);
    delay(MinAltPulse*1.5);
    digitalWrite(Enable, HIGH);
    delay(MinAltPulse*1.5);
  }
  if(distance<0 && !ReverseDirection || distance>0 && ReverseDirection){
    digitalWrite(InputA,HIGH);
  }
  else{
    digitalWrite(InputA,LOW);
  }

  for(int count=0;count<abs(distance);count++){
    digitalWrite(InputB, HIGH);
    digitalWrite(InputB, LOW);
  }
    
}

