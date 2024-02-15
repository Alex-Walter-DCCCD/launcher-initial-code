// Link to simulation
// https://www.tinkercad.com/things/3bzaZug1iTb-testing-last-project?sharecode=tlisHi4zVbZKJEC21iCnApdwPd5BvmY5FmNuMqr12-8

// Code has been modified so that output that would be normally displayed on the 7-segment clock from the simulation will also be displayed on the 16x2 lcd.
// Most code for 7-segment has been commented out and verified to work in Arduino IDE. However, I haven't had a chance to test the code on a circuit with the 7-segment code removed

#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Adafruit_LEDBackpack.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

const byte ROWS = 4; //four rows
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[4] = {13, 12, 11, 10};
byte colPins[4] = {6, 5, 4, 3};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

Adafruit_7segment clock = Adafruit_7segment();

const int maxSize = 4; // Maximum size for the entered numbers array

int countDown[2] = {0, 0}; // {Min, Sec} Countdown requires 4 digits of decimal to be displayed, but countdown loops need to decrement 1 min every 60 sec, not 99 sec.
int inputPSI = 0;

const int maxPotValue = 1023; // Maximum value read from the potentiometer
const int convertedMaxValue = 80;

const int ledPin1 = 8; // Define the pin for the first LED
const int ledPin2 = 2; // Define the pin for the second LED

int initLaunch = 0;
int simInput = 0;


void setup() {
  // Setup for LCD
  lcd.init();
  lcd.backlight();

  // Setup for 7 segment clock

  /****************************
  clock.begin(0x70);
  clock.drawColon(true);
  clock.writeDisplay();
  clock.setBrightness(15);
  *****************************/
  
  pinMode(ledPin1, OUTPUT); // Set the first LED pin as an output
  pinMode(ledPin2, OUTPUT); // Set the second LED pin as an output
  pinMode(7, OUTPUT);
}

void timerSet() {
  char keyIn;

  int enteredNumbers[maxSize] = {0}; // Initialize array to store entered numbers

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("A:Set timerStart");
  lcd.setCursor(0, 1);
  lcd.print("C:cancel");
  
  while (1) {
    keyIn = keypad.getKey();
    if (keyIn == 'C') {
      return;
    }
    else if (keyIn == 'A') {
      countDown[0] = enteredNumbers[0] * 10 + enteredNumbers[1];
      countDown[1] = enteredNumbers[2] * 10 + enteredNumbers[3];
      return;
    }
    else if (keyIn >= '0' && keyIn <= '9') {
      // Shift the values in the array
      for (int i = 0; i < maxSize - 1; ++i) {
        enteredNumbers[i] = enteredNumbers[i + 1];
      }
      // Store the entered number in the last address of the array
      enteredNumbers[maxSize - 1] = keyIn - '0'; // Convert char to int

      // Convert the array of numbers into a single integer

      /******************************************************************************************************************
      int numberToShow = enteredNumbers[0] * 1000 + enteredNumbers[1] * 100 + enteredNumbers[2] * 10 + enteredNumbers[3];
      ******************************************************************************************************************/

      lcd.setCursor(9, 1);
      lcd.print(enteredNumbers[0]);
      lcd.print(enteredNumbers[1]);
      lcd.print(':');
      lcd.print(enteredNumbers[2]);
      lcd.print(enteredNumbers[3]);

      // Display the single integer on the 7-segment 4-digit clock

      /*******************************************
      clock.print(numberToShow);
      clock.drawColon(true);
      clock.writeDisplay(); // Update the display
      *******************************************/
    }
  }
}


void psiSet() {
  char keyIn;
  int currentPSI = 0; // Variable to store the currently entered PSI

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PSI: ");
  lcd.setCursor(8, 0);
  lcd.print("B:Reset");
  lcd.setCursor(0, 1);
  lcd.print("A: Set C: Cancel");
  
  int digitCount = 0; // Counter to keep track of the number of digits entered
  
  while (1) {
    keyIn = keypad.getKey();
    
    // Display PSI value on the first row
    lcd.setCursor(4, 0); // Set cursor to position after "PSI: "
    lcd.print(currentPSI); // Display currentPSI value
    
    if (keyIn == 'C') {
      return; // Exit the function without assigning a value to inputPSI
    }
    else if (keyIn == 'A') {
      // Check if conditions meet for assigning inputPSI
      if (currentPSI >= 0 && currentPSI <= 80) {
        inputPSI = currentPSI;
        return;
      } else if (currentPSI > 80) {
        lcd.setCursor(0, 1);
        lcd.print("                 "); // Clear the second row
        lcd.setCursor(0, 1);
        lcd.print("PSI too high");
        delay(3000); // Display "PSI too high" for 3 seconds
        currentPSI = 0; // Reset currentPSI after displaying the message
        digitCount = 0; // Reset digit count
        lcd.setCursor(0, 1);
        lcd.print("A: Set C: Cancel");
        lcd.setCursor(4, 0); // Set cursor to position after "PSI: "
        lcd.print("   "); // Reset display for PSI
      }
    }
    else if (keyIn == 'B') {
      // Reset currentPSI and digitCount for a new entry
      currentPSI = 0;
      digitCount = 0;
      lcd.setCursor(4, 0); // Set cursor to position after "PSI: "
      lcd.print("   "); // Reset display for PSI
    }
    else if (keyIn >= '0' && keyIn <= '9') {
      // Convert the character input to an integer
      int digit = keyIn - '0';
      
      // Only accept three digits
      if (digitCount < 3) {
        currentPSI = currentPSI * 10 + digit; // Calculate the PSI value
        digitCount++; // Increment the digit count
      }
    }
  }
  delay(250);
}





void loop() {
  //clock status

  /************************************************
  int clockOut = countDown[0] * 100 + countDown[1];
  clock.print(clockOut);
  clock.drawColon(true);
  clock.writeDisplay();
  ************************************************/

  char abortKey;
  
  // Initial Keypad and display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("A:Set timer");
  lcd.setCursor(0, 1);
  lcd.print("B:PSI C:Launch");
  while(true) {
    
  char firstKey = keypad.getKey();
  if (firstKey == 'A') {
    timerSet();
    break;
  }
  else if (firstKey == 'B') {
    psiSet();
    break;
  }
  else if (firstKey == 'C') {
    initLaunch = 1;
    break;
  }
  delay(250); //End of while loop code
    
  }
  
  // Code for countdown and launch
  if(initLaunch == 1) {
    simInput = map(analogRead(A1), 0, maxPotValue, 0, convertedMaxValue); // Read and convert SimInput
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Target PSI: ");
    lcd.print(inputPSI);
    lcd.setCursor(0, 1); // Move to the middle of the second line
    lcd.print("PSI: ");
    lcd.print(simInput);
    lcd.setCursor(8, 1);
    lcd.print(" A:Abort");
    while(true) {

      // Compares PSI values and decides if a signal should be sent to the compressor or the bleed-off valve; also checks if the launch should continue or be aborted
      simInput = map(analogRead(A1), 0, maxPotValue, 0, convertedMaxValue); // Read and convert SimInput
      lcd.setCursor(5, 1);
      lcd.print("   ");
      lcd.setCursor(5, 1);
      lcd.print(simInput);
      abortKey = keypad.getKey();

      // Aborts launch when A is pressed, need to add way for pressure to bleed off when pressed, but stop when starting another launch
      if(abortKey == 'A') {
        digitalWrite(8, LOW);
        digitalWrite(2, LOW);
        initLaunch = 0;
        break;

        // Pressure is too high, bleeding off pressure, delay should be less than increasing pressure
      } else if(simInput < inputPSI) {
        digitalWrite(8, HIGH);
        digitalWrite(2, LOW);

        // Needs more pressure, sends signal to the compressor
      } else if (simInput > inputPSI) {
        digitalWrite(8, LOW);
        digitalWrite(2, HIGH);

        // Continues to next part of launch
      } else if(simInput == inputPSI) {
        digitalWrite(8, LOW);
        digitalWrite(2, LOW);
        initLaunch = 2;
        break;
      }
      delay(350); //delay between PSI checks
    }

    // Loop for the countdown
    while(simInput == inputPSI) { //This could be changed to a while(1) statement, but I wanted another safety measure in case of a potential malfunction
      if(countDown[1] < 0) {
        countDown[1] = 59;
        countDown[0]--;
      }
      
      abortKey = keypad.getKey();
      if(abortKey == 'A') {
        initLaunch = 0;
        break;
      }
      
      lcd.setCursor(8, 1);
      lcd.print("        "); // Used to refresh region used in case countdown uses less char such as when either min or sec decrement from 10
      lcd.setCursor(8, 1);
      lcd.print(countDown[0]);
      lcd.print(':');
      if(countDown[1] < 10) { //Prints leading 0 when seconds < 10, previous refresh used for edge case of minutes decrementing from 10
        lcd.print(0);
      }
      lcd.print(countDown[1]);

      /*********************************************
      clockOut = countDown[0] * 100 + countDown[1];
      clock.print(clockOut);
      clock.drawColon(true);
      clock.writeDisplay();
      **********************************************/
      
      if(countDown[0] == 0 && countDown[1] == 0) {
        digitalWrite(7, HIGH);
        delay(3500);
        digitalWrite(7, LOW);
        initLaunch = 0;
        break;
      }
      delay(1000);
      countDown[1]--;
    }
    
  }
  delay(250);
}
