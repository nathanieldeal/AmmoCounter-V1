// Define the LED digit patterns, from 0 to 9    
// Updated 2/15/2016
// Created by: Nathaniel Deal
//
// Define the LED digit patterns, from 0 to 9
// Note that these patterns are for common anode displays
// 0 = LED on, 1 = LED off:

int countSet = 18; // Set initial count
int displayCount = countSet;  // Store Intial count
int toggleArray[] = {6,12,18,25,35}; // Setup array of clip sizes

int firstDigit;
int secondDigit;
boolean toggleState;
boolean resetState; 
boolean counterState; 

// IR Beam Setup
const int analogInPin = A2;  // Analog input pin that the ir reciever is attached to
int sensorValue = 0;        // Value read from the ir beam
int outputValue = 0;        // Value output to the PWM (analog out)
boolean hasCleared = true;  // Check for cleared dart
 
// Toggle/Reset/Counter Pins
const int togglePin = 4;     // Use digital pin 4 for the reset pin
const int resetPin = 5;     // Use digital pin 5 for the reset pin
const int counterPin = 6;     // Use digital pin 6 for the counter pin

// Shift Register Pins
int SER_Pin = 7;   // Serial-In pin 14 on the 75HC595 (Blue)
int RCLK_Pin = 8;  // Latch Clock pin 12 on the 75HC595 (Yellow)
int SRCLK_Pin = 9; // Clock pin 11 on the 75HC595 (Green)

//How many of the shift registers - change this
#define number_of_74hc595s 2 

//do not touch
#define numOfRegisterPins number_of_74hc595s * 8

boolean registers[numOfRegisterPins];

void setup() {

  pinMode(togglePin, INPUT);
  digitalWrite(togglePin, LOW); // Pull Down
  
  pinMode(resetPin, INPUT);
  digitalWrite(resetPin, LOW); // Pull Down
  
  pinMode(counterPin, INPUT);
  digitalWrite(counterPin, LOW); // Pull Down
  
  pinMode(SER_Pin, OUTPUT);
  pinMode(RCLK_Pin, OUTPUT);
  pinMode(SRCLK_Pin, OUTPUT);

  Serial.begin(9600);
  
  // Reset all register pins
  clearRegisters();
  writeRegisters();
  
  // Show Initial Count
  changeNumber(displayCount);
  
}               

void loop(){
  
  // Monitor IR Beam
  //----------------------------------------------------//
  
    sensorValue = analogRead(analogInPin); // Read the analog in value
    outputValue = map(sensorValue, 0, 1023, 0, 255);  // Map it to the range of the analog output

    // Check to see if dart has cleared
    if (outputValue < 75) { // This value can be changed depending on IR beam distance
      hasCleared = true;
    }
  
    // If barrel is clear and beam is broken then countdown
    if (hasCleared == true && outputValue > 100) {  // This value can be changed depending on dart speed  
      changeNumber(displayCount--); 
      hasCleared = false;
      //delay(2);
    }
  
    // Print the results to the serial monitor for testing
    if (outputValue > 100) {
      Serial.print("\t output = ");      
      Serial.println(outputValue);
    }

  
  // Monitor Counter Button
  //----------------------------------------------------//
  
    counterState = digitalRead(counterPin);

    // Check if the pushbutton is pressed.
    if (counterState == HIGH) {       
      changeNumber(displayCount--);  
    }
  
  
  // Monitor Toggle Button
  //----------------------------------------------------//
 
    toggleState = digitalRead(togglePin);
  
    // Check if the togglebutton is pressed.
    if (toggleState == HIGH) {       
        
      if (countSet == toggleArray[4]) {
        countSet = toggleArray[0];
      }
      
      else if (countSet == toggleArray[3]) {
        countSet = toggleArray[4];
      }
      
      else if (countSet == toggleArray[2]) {
        countSet = toggleArray[3];
      }
      
      else if (countSet == toggleArray[1]) {
        countSet = toggleArray[2];
      }
      
      else if (countSet == toggleArray[0]) {
        countSet = toggleArray[1];
      }

      displayCount = countSet;
      changeNumber(displayCount);
    }
    
  
  // Monitor Reset Button
  //----------------------------------------------------//
    
    resetState = digitalRead(resetPin);
  
    // Check if resetbutton is pressed.
    if (resetState == HIGH) {  
      displayCount = countSet;    
      changeNumber(displayCount);
    }

}

void changeNumber(int displayCount) {

  if( (displayCount < 100) && (displayCount > 9) ) {
    
    firstDigit = displayCount / 10; // Find the first digit
    secondDigit = displayCount % 10; 
    
    displayNumber(firstDigit,secondDigit); //Display the digits
    
    // Serial.println(secondDigit);
    
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
    
    // displayNumber(10); //Turn off all segments
    
    // add double zero blink and buzzer function here instead
    
    // clearRegisters();
    writeRegisters(); 
  }
  
  //delay(250); // May be needed to debounce switch
}


void displayNumber(int digit1, int digit2) {

  for (int digit = numOfRegisterPins ; digit > 0 ; digit--) {
    
    //Turn on the right segments for this digit
    firstNumber(digit1);
    secondNumber(digit2);
    
  }

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


