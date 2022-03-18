/*==============================================================================

  Chiron Cable Tester Firmware

  (c) 2022 Wabbit Wanch Design - Mel Patrick
  All code complies with GPLv2 and/or GPLv3

================================================================================
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display
#define sensePIN0 A0
uint16_t theVal;
//
byte controlPins[] = {B00000000,
                      B00010000,
                      B00100000,
                      B00110000,
                      B01000000,
                      B01010000,
                      B01100000,
                      B01110000,
                      B10000000,
                      B10010000,
                      B10100000,
                      B10110000,
                      B11000000,
                      B11010000,
                      B11100000,
                      B11110000
                     };
//
byte testValue[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//
uint8_t goodVAL[8] = {B0, B1, B11, B10110, B11100, B1000, B0};

byte maxVal = 200;// anything this high or higher is an open circuit
byte minVal = 20;// anything this low or lower is a closed circuit
int8_t whichStep = 99;// used to stop flashing display
const int8_t inNoCable = 2;
const int8_t *inDataMode = 1;
//
void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, goodVAL);
  handleDisplayHeading();// go show the heading.
  //
  DDRD = B11111111; // (digital 7~0) to outputs
  pinMode(sensePIN0, INPUT_PULLUP);  // set pull-up on analog pin 0
}

void loop() {
  runTest();
  //  displayDataSerial();
  displayDataLCD();
  delay(1000);
}
//
void runTest() {
  for (int8_t whichPin = 0; whichPin < 16; whichPin++) {
    PORTD = controlPins[whichPin];// set the pin to read
    delay(5);// settle time
    testValue[whichPin] = analogRead(sensePIN0); // read the vlaue on that pin and store in array
    delay(5);
  }
}
//
void displayDataSerial() {
  Serial.println();
  Serial.println("Values from multiplexer:");
  Serial.println("========================");
  for (int8_t whichPin = 0; whichPin < 16; whichPin++)
  {
    Serial.print("input I: ");
    Serial.print(whichPin);
    Serial.print(" = ");
    Serial.println(testValue[whichPin]);
  }
  Serial.println("========================");
}
//
void displayDataLCD() {
  boolean gotData = false;// default to no cable
  int8_t testVal;
  for (int8_t loop = 0; loop < 16; loop++) {// this checks to see if there's even a cable plugged in
    if (testValue[loop] < minVal) gotData = true;
  }
  if (gotData) {
    if (whichStep != inDataMode) {
      whichStep = inDataMode;
      handleDisplayClear();
    }
    displayCableLCD();// go show the data if we have a cable
  } else {
    if (whichStep != inNoCable) {
      whichStep = inNoCable;// now we are in this routine
      handleDisplayClear();
    }
    lcd.setCursor(6 , 2);// 2nd row
    lcd.print("No Cable");
  }
}
//
void displayCableLCD() {
  lcd.setCursor(1, 2);
  lcd.print("A");
  lcd.setCursor(1, 1);
  lcd.print("B");
  for (int8_t loop = 0; loop < 8; loop++) {
    lcd.setCursor((loop * 2) + 3, 1);// top row
    displayResult(testValue[loop + 8]);
    lcd.setCursor((loop * 2) + 3, 2);// top row
    displayResult(testValue[loop]);
    lcd.setCursor((loop * 2) + 3, 3);
    lcd.print(loop + 1);
  }
}
//
void displayResult(byte testVal) {
  Serial.println(testVal);
  if (testVal > maxVal) {
    lcd.print("X");// we have an open wire
  } else {
    if (testVal < minVal) {
      lcd.printByte(0);// wire is OK
    } else {
      lcd.print("?");// something wrong with this wire
    }
  }
}
//
void handleDisplayHeading() {
  lcd.clear();
  lcd.home();
  lcd.print("Chiron Cable Tester");
}
//
void handleDisplayClear() {
  for (int8_t loop = 1; loop < 4; loop++) {
    lcd.setCursor(0, loop);
    lcd.print("                    ");// clear the line
  }
}
