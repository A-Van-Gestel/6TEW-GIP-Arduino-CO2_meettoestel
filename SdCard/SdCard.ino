//https://howtomechatronics.com/tutorials/arduino/arduino-sd-card-data-logging-excel-tutorial
// Connect pin GND to GND
// Connect pin VCC to 5V (NOT 3.3V)
// Connect pin MISO to 12 (Static)
// Connect pin MOSI to 11 (Static)
// Connect pin SCK to 13 (Static)
// Connect pin CS to 10 (Variable)

#include <SD.h>
#include <SPI.h>
File myFile;
int pinCS = 10; // Pin 10 on Arduino Uno
void setup() {
    
  Serial.begin(9600);
  pinMode(pinCS, OUTPUT);
  
  // SD Card Initialization
  if (SD.begin())
  {
    Serial.println("SD card is ready to use.");
  } else
  {
    Serial.println("SD card initialization failed");
    return;
  }
  
  // Create/Open file 
  myFile = SD.open("test.txt", FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.println("Writing to file...");
    // Write to file
    myFile.println("Testing text 1, 2 ,3...");
    myFile.close(); // close the file
    Serial.println("Done.");
  }
  // if the file didn't open, print an error:
  else {
    Serial.println("error opening test.txt");
  }
  // Reading the file
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("Read:");
    // Reading the whole file
    while (myFile.available()) {
      Serial.write(myFile.read());
   }
    myFile.close();
  }
  else {
    Serial.println("error opening test.txt");
  }
  
}
void loop() {
  // empty
}
