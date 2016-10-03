// AmmoCounter V1 Universal Kit
// Updated 10/3/2016
// Created by: Nathaniel Deal
// More Info: www.ammocounter.com 

// Include Libraries
#include <Bounce2.h>
#include <AmmoCounter.h>

// Setup Toggle/Counter Variables
int toggleArray[] = {35,25,22,18,15,12,10,6}; // Setup array of magazine sizes
int toggleCount = (sizeof(toggleArray)/sizeof(int))-1; // Find size of array
int togglePosition = toggleCount; // Start at max capacity.
int count = toggleArray[toggleCount];  // Set intial count to highest capacity.
AmmoCounter counter = AmmoCounter(); // Setup AmmoCounter instance

// IR Beam Setup
int irSensorPin = A2;
int idleValue = 1;
int fireValue = 3;
boolean hasCleared = false;  // Check for cleared dart
 
// Setup Switches with Bounce2 Library
#define toggleBtnpin 4
#define resetBtnpin 5
//#define counterSwitchpin 6
Bounce toggleBtn = Bounce(); 
Bounce resetBtn = Bounce(); 
//Bounce counterSwitch = Bounce(); 

void setup() {

  // Toggle/Counter Setup
  pinMode(toggleBtnpin, INPUT); 
  pinMode(resetBtnpin, INPUT); 
  //pinMode(counterSwitchpin, INPUT); 

  digitalWrite(toggleBtnpin, LOW);  // Activate internal pullup
  digitalWrite(resetBtnpin, LOW);  // Activate internal pullup
  //digitalWrite(counterSwitchpin, LOW);  // Activate internal pullup

  toggleBtn.attach(toggleBtnpin);
  resetBtn.attach(resetBtnpin);
  //counterSwitch.attach(counterSwitchpin);

  toggleBtn.interval(5);
  resetBtn.interval(5);
  //counterSwitch.interval(5); 
  
  // Show Initial Count
  counter.displayNumber(count);

  // Serial.begin(9600); // Uncomment for testing
  
}               

void loop(){

  // Monitor Counter Switch
  //----------------------------------------------------//

    // Check if the counter switch is pressed.    
    // if (counterSwitch.update() ) 
    // {
    //   if (counterSwitch.read() == HIGH)  
    //   {
    //     counter.displayNumber(--count);  
    //   }
    // }  
  
  // Monitor IR Beam
  //----------------------------------------------------//

    int sensorValue = analogRead(irSensorPin); // Read the analog in value
    int outputValue = map(sensorValue, 0, 1023, 0, 10);  // Map it to the range of the analog output

    // Check to see if dart has fired
    if (outputValue > fireValue)
    {
      if (hasCleared == true) // If barrel is clear and beam is broken then countdown 
      {
        counter.displayNumber(--count); 
        hasCleared = false;

        // Print the results to the serial monitor for testing
        // Serial.print("\t output = ");      
        // Serial.println(outputValue);
      }
    }

    // Check to see if dart has cleared
    if (outputValue <= idleValue)
    {
      hasCleared = true;
    }
 
  // Monitor Toggle Button
  //----------------------------------------------------//
  
    // Check if the togglebutton is pressed.
    if (toggleBtn.update() ) 
    {
      if (toggleBtn.read() == HIGH)  
      {
        if (togglePosition == 0) {
          togglePosition = toggleCount; //Reset to max.
        } else {
          togglePosition--; //Deincrement capacity one step 
        } 
  
        count = toggleArray[togglePosition];
        counter.displayNumber(count); //Send to display  
      }
    }

  // Monitor Reset Button
  //----------------------------------------------------//
  
    // Check if the decrease button is pressed.
    if (resetBtn.update() ) 
    {
      if (resetBtn.read() == HIGH)  
      {        
        count = toggleArray[togglePosition];
        counter.displayNumber(count); //Send to display  
      }
    }

  // Auto-Reset
  //----------------------------------------------------//

    // Check if count has finished
    if (count == 0) {
      count = toggleArray[togglePosition];  
      counter.displayNumber(count); //Send to display
    }
}
