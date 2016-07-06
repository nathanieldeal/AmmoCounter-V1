// AmmoCounter V1 With IR Beam - www.ammocounter.com   
// Updated 7/6/2016
// Created by: Nathaniel Deal
//
// Define the LED digit patterns, from 0 to 9
// Note that these patterns are for 2 digit common anode displays
// 0 = LED on, 1 = LED off:

// Include Libraries
#include <Button.h>

// Setup Counter Variables
int toggleArray[] = {35,25,18,15,12,6}; // Setup array of magazine sizes
int toggleCount = (sizeof(toggleArray)/sizeof(int))-1; // Find size of array
int togglePosition = toggleCount; //Start at max capacity.
int displayCount = toggleArray[toggleCount];  // Set intial count to highest capacity.
int firstDigit, secondDigit;

// IR Beam Setup
int irSensorPin = A2;
boolean hasCleared = false;  // Check for cleared dart
 
// Toggle/Reset/Counter Setup
Button clipBtn = Button (3, PULLDOWN);     // Use digital pin 3 for the clip pin
Button toggleBtn = Button (4, PULLDOWN);   // Use digital pin 4 for the toggle pin
Button resetBtn = Button (5, PULLDOWN);    // Use digital pin 5 for the reset pin
Button counterBtn = Button (6, PULLDOWN);  // Use digital pin 6 for the counter pin

// Shift Register Setup
int SER_Pin = 7;   // Serial-In pin 14 on the 75HC595 (Blue)
int RCLK_Pin = 8;  // Latch Clock pin 12 on the 75HC595 (Yellow)
int SRCLK_Pin = 9; // Clock pin 11 on the 75HC595 (Green)

//Define register pins
#define numOfRegisterPins 16
boolean registers[numOfRegisterPins];

void setup() {
  
  pinMode(SER_Pin, OUTPUT);
  pinMode(RCLK_Pin, OUTPUT);
  pinMode(SRCLK_Pin, OUTPUT);

  //Serial.begin(9600);
  
  // Reset all register pins
  clearRegisters();
  writeRegisters();
  
  // Show Initial Count
  changeNumber(displayCount);
  
}               

void loop(){
  
  // Monitor IR Beam
  //----------------------------------------------------//

    int sensorValue = analogRead(irSensorPin); // Read the analog in value
    int outputValue = map(sensorValue, 0, 1023, 0, 10);  // Map it to the range of the analog output

    // Check to see if dart has fired
    if (outputValue > fireValue)
    {
      if (hasCleared == true) // If barrel is clear and beam is broken then countdown 
      {
        changeNumber(--displayCount); 
        hasCleared = false;

        // Print the results to the serial monitor for testing
        // Serial.print("\t output = ");      
        // Serial.println(outputValue);
      }
    }

    // Check to see if dart has cleared
    if (outputValue == idleValue)
    {
      hasCleared = true;
    }
  
  // Monitor Counter Button
  //----------------------------------------------------//

    //Check if the pushbutton is pressed.
    //if (counterBtn.uniquePress()) {       
    //  changeNumber(--displayCount);  
    //}
  
  // Monitor Toggle Button
  //----------------------------------------------------//
  
    // Check if the togglebutton is pressed.
    if (toggleBtn.uniquePress()) {       
        
      if (togglePosition == 0) {
        togglePosition = toggleCount; //Reset to max.
      } else {
        togglePosition--; //Deincrement capacity one step 
      } 

      displayCount = toggleArray[togglePosition];
      changeNumber(displayCount); //Send to display
    }
  
  
  // Monitor Reset Button
  //----------------------------------------------------//
  
    // Check if resetbutton is pressed.
    if (resetBtn.uniquePress()) {  
      resetCount();
    }

  // Monitor Clip Reset
  //----------------------------------------------------//
    
    // Check if clip is released
    //if (clipBtn.uniquePress()) {  
    //  resetCount();
    //}
  
}

void resetCount(){
  displayCount = toggleArray[togglePosition];  
  changeNumber(displayCount); //Send to display
}

void changeNumber(int displayCount) {

  if( (displayCount < 100) && (displayCount > 9) ) {

    firstDigit = displayCount / 10; // Find the first digit
    secondDigit = displayCount % 10; 
    
    displayNumber(firstDigit,secondDigit); //Display the digits  
    displayCount--;
      
    writeRegisters();  //Send data to the 75HC595
  }

  else if( (displayCount < 10) && (displayCount > 0) ) {

    firstDigit = 0; // Set the first digit to 0
    secondDigit = displayCount;
    
    displayNumber(firstDigit,secondDigit); //Display the section 
    displayCount--;
       
    writeRegisters();
  }
  
  else if( displayCount == 0) {
    
    firstDigit = 0; // Set the first digit to 0
    secondDigit = 0; // Set the second digit to 0
    
    displayNumber(firstDigit,secondDigit); //Display the section
    writeRegisters(); 
    
    // TODO: add double zero blink and buzzer function
  }
  
  //delay(250); // May be needed to debounce button
}

void displayNumber(int digit1, int digit2) {
    
  //Turn on the right segments for each digit
  firstNumber(digit1);
  secondNumber(digit2);

}

//Given a number, turns on those segments
//If number == 10, then turn off number
void firstNumber(int numberToDisplay) {
  
  switch (numberToDisplay){
    
  // Display number 0
  case 0:
    setRegisterPin(0, LOW);
    setRegisterPin(1, LOW);
    setRegisterPin(2, LOW);
    setRegisterPin(3, LOW);
    setRegisterPin(4, LOW);
    setRegisterPin(5, LOW);
    setRegisterPin(6, HIGH);
    break;
  
  // Display number 1
  case 1:
    setRegisterPin(0, HIGH);
    setRegisterPin(1, LOW);
    setRegisterPin(2, LOW);
    setRegisterPin(3, HIGH);
    setRegisterPin(4, HIGH);
    setRegisterPin(5, HIGH);
    setRegisterPin(6, HIGH);
    break; 
  
  // Display number 2
  case 2:
    setRegisterPin(0, LOW);
    setRegisterPin(1, LOW);
    setRegisterPin(2, HIGH);
    setRegisterPin(3, LOW);
    setRegisterPin(4, LOW);
    setRegisterPin(5, HIGH);
    setRegisterPin(6, LOW);
    break;
  
  // Display number 3
  case 3:
    setRegisterPin(0, LOW);
    setRegisterPin(1, LOW);
    setRegisterPin(2, LOW);
    setRegisterPin(3, LOW);
    setRegisterPin(4, HIGH);
    setRegisterPin(5, HIGH);
    setRegisterPin(6, LOW);
    break;   

  // Display number 4
  case 4:
    setRegisterPin(0, HIGH);
    setRegisterPin(1, LOW);
    setRegisterPin(2, LOW);
    setRegisterPin(3, HIGH);
    setRegisterPin(4, HIGH);
    setRegisterPin(5, LOW);
    setRegisterPin(6, LOW);
    break;

  // Display number 5
  case 5:
    setRegisterPin(0, LOW);
    setRegisterPin(1, HIGH);
    setRegisterPin(2, LOW);
    setRegisterPin(3, LOW);
    setRegisterPin(4, HIGH);
    setRegisterPin(5, LOW);
    setRegisterPin(6, LOW);
    break;
  
  // Display number 6
  case 6:
    setRegisterPin(0, LOW);
    setRegisterPin(1, HIGH);
    setRegisterPin(2, LOW);
    setRegisterPin(3, LOW);
    setRegisterPin(4, LOW);
    setRegisterPin(5, LOW);
    setRegisterPin(6, LOW);
    break;

  // Display number 7
  case 7:
    setRegisterPin(0, LOW);
    setRegisterPin(1, LOW);
    setRegisterPin(2, LOW);
    setRegisterPin(3, HIGH);
    setRegisterPin(4, HIGH);
    setRegisterPin(5, HIGH);
    setRegisterPin(6, HIGH);
    break;

  // Display number 8
  case 8:
    setRegisterPin(0, LOW);
    setRegisterPin(1, LOW);
    setRegisterPin(2, LOW);
    setRegisterPin(3, LOW);
    setRegisterPin(4, LOW);
    setRegisterPin(5, LOW);
    setRegisterPin(6, LOW);
    break;
  
  // Display number 9
  case 9:
    setRegisterPin(0, LOW);
    setRegisterPin(1, LOW);
    setRegisterPin(2, LOW);
    setRegisterPin(3, HIGH);
    setRegisterPin(4, HIGH);
    setRegisterPin(5, LOW);
    setRegisterPin(6, LOW);
    break;
    
  // Turn off display
  case 10:
    setRegisterPin(0, HIGH);
    setRegisterPin(1, HIGH);
    setRegisterPin(2, HIGH);
    setRegisterPin(3, HIGH);
    setRegisterPin(4, HIGH);
    setRegisterPin(5, HIGH);
    setRegisterPin(6, HIGH);
    break;
  
  }
}

//Given a number, turns on those segments
//If number == 10, then turn off number
void secondNumber(int numberToDisplay) {
  
  switch (numberToDisplay){
    
  // Display number 0
  case 0:
    setRegisterPin(8, LOW);
    setRegisterPin(9, LOW);
    setRegisterPin(10, LOW);
    setRegisterPin(11, LOW);
    setRegisterPin(12, LOW);
    setRegisterPin(13, LOW);
    setRegisterPin(14, HIGH);
    break;
  
  // Display number 1
  case 1:
    setRegisterPin(8, HIGH);
    setRegisterPin(9, LOW);
    setRegisterPin(10, LOW);
    setRegisterPin(11, HIGH);
    setRegisterPin(12, HIGH);
    setRegisterPin(13, HIGH);
    setRegisterPin(14, HIGH);
    break; 
  
  // Display number 2
  case 2:
    setRegisterPin(8, LOW);
    setRegisterPin(9, LOW);
    setRegisterPin(10, HIGH);
    setRegisterPin(11, LOW);
    setRegisterPin(12, LOW);
    setRegisterPin(13, HIGH);
    setRegisterPin(14, LOW);
    break;
  
  // Display number 3
  case 3:
    setRegisterPin(8, LOW);
    setRegisterPin(9, LOW);
    setRegisterPin(10, LOW);
    setRegisterPin(11, LOW);
    setRegisterPin(12, HIGH);
    setRegisterPin(13, HIGH);
    setRegisterPin(14, LOW);
    break;   

  // Display number 4
  case 4:
    setRegisterPin(8, HIGH);
    setRegisterPin(9, LOW);
    setRegisterPin(10, LOW);
    setRegisterPin(11, HIGH);
    setRegisterPin(12, HIGH);
    setRegisterPin(13, LOW);
    setRegisterPin(14, LOW);
    break;

  // Display number 5
  case 5:
    setRegisterPin(8, LOW);
    setRegisterPin(9, HIGH);
    setRegisterPin(10, LOW);
    setRegisterPin(11, LOW);
    setRegisterPin(12, HIGH);
    setRegisterPin(13, LOW);
    setRegisterPin(14, LOW);
    break;
  
  // Display number 6
  case 6:
    setRegisterPin(8, LOW);
    setRegisterPin(9, HIGH);
    setRegisterPin(10, LOW);
    setRegisterPin(11, LOW);
    setRegisterPin(12, LOW);
    setRegisterPin(13, LOW);
    setRegisterPin(14, LOW);
    break;

  // Display number 7
  case 7:
    setRegisterPin(8, LOW);
    setRegisterPin(9, LOW);
    setRegisterPin(10, LOW);
    setRegisterPin(11, HIGH);
    setRegisterPin(12, HIGH);
    setRegisterPin(13, HIGH);
    setRegisterPin(14, HIGH);
    break;

  // Display number 8
  case 8:
    setRegisterPin(8, LOW);
    setRegisterPin(9, LOW);
    setRegisterPin(10, LOW);
    setRegisterPin(11, LOW);
    setRegisterPin(12, LOW);
    setRegisterPin(13, LOW);
    setRegisterPin(14, LOW);
    break;
  
  // Display number 9
  case 9:
    setRegisterPin(8, LOW);
    setRegisterPin(9, LOW);
    setRegisterPin(10, LOW);
    setRegisterPin(11, HIGH);
    setRegisterPin(12, HIGH);
    setRegisterPin(13, LOW);
    setRegisterPin(14, LOW);
    break;
    
  // Turn off display
  case 10:
    setRegisterPin(8, HIGH);
    setRegisterPin(9, HIGH);
    setRegisterPin(10, HIGH);
    setRegisterPin(11, HIGH);
    setRegisterPin(12, HIGH);
    setRegisterPin(13, HIGH);
    setRegisterPin(14, HIGH);
    break;
  
  }
}

//set all register pins to HIGH
void clearRegisters(){
  for(int i = numOfRegisterPins - 1; i >=  0; i--){
     registers[i] = HIGH;
  }
} 

//Set and display registers
//Only call AFTER all values are set how you would like (slow otherwise)
void writeRegisters(){

  digitalWrite(RCLK_Pin, LOW);

  for (int i = numOfRegisterPins - 1; i >=  0; i--) {
    digitalWrite(SRCLK_Pin, LOW);

    int val = registers[i];
    
    // Serial.println(registers[i]);

    digitalWrite(SER_Pin, val);
    digitalWrite(SRCLK_Pin, HIGH);

  }
  digitalWrite(RCLK_Pin, HIGH);

}

//set an individual pin HIGH or LOW
void setRegisterPin(int index, int value){
  registers[index] = value;
}

