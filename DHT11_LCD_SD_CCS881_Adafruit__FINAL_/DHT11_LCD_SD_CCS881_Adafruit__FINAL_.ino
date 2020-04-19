//----------Temporary Stuff----------

//----------Setup SD Card Module----------
// Connect pin GND to GND
// Connect pin VCC to 5V (NOT 3.3V!)
// Connect pin MISO to D12 (Static)   (Blue wire)
// Connect pin MOSI to D11 (Static)   (Orange wire)
// Connect pin SCK to D13 (Static)    (White wire)
// Connect pin CS to D10 (Variable)   (Gray wire)

// Include SD Card libraries
#include <SD.h>
#include <SPI.h>

// Setup SD Card
File SDcard;
const byte pinCS = 10; // Pin 10 on Arduino Uno

//----------Setup DHT11 sensor----------
// Connect pin 1 (Left) of the sensor to whatever your DHTPIN is (D2)   (White wire)
// Connect pin 2 (Middle) of the sensor to 3.3V
// Connect pin 3 (Right) of the sensor to GROUND

// Inlcude DHT11 libraries
#include <DHT.h>

// Setup DHT11
#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

//----------Setup CCS881 sensor----------
// Connect GND to GND
// Connect 3.3V to 3.3V (NOT 5V!)
// Connect SDA to A4 or SDA   (Yellow wire)
// Connect SCL to A5 or SCL   (Green wire)
// Connect WAK to GND (nWake = -1) or defined nWAKE Pin   (Brown wire)

// Include CCS881 libraries
#include <Wire.h>    // I2C library
#include "Adafruit_CCS811.h"  // CCS811 library

// Setup CCS881
Adafruit_CCS811 ccs;
int co2;
int tvoc;

//----------Setup LCD Screen (I2C protocol)----------
// Connect GND to GND
// Connect VCC to 5V
// Connect SDA to SDA or A4   (Yellow wire)
// Connect SCL to SCL or A5   (Green wire)

// Inlcude LCD libraries
//#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Setup LCD
// 0x3F LCD I2C adress, if not working run I2C Scanner
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Change the 0x3F  i2c address to your i2c address

// lcd.noBacklight(); // turn off backlight
// lcd.backlight();   // turn on backlight.

// Setup Timer
long timer = 0;
int delaytime = 2000; // Delay between readings 1s = 1000




void setup() {
  // Setup Serial monitor 9600 baud
  Serial.begin(9600);
  Serial.println(F("Beginning Checkup..."));

  // Start LCD Display
  lcd.begin(16,2);  // Set LCD Type 16x2
  lcd.clear();      // Clears LCD display

  // SD Card Initialization
  if (SD.begin()) {
    Serial.println(F("SD card is ready to use."));
    lcd.print(F("SD Card Ready"));
    delay(2000);
    }
  else {
    Serial.println(F("SD card initialization failed"));
    lcd.print(F("SD Card Failed"));
    lcd.setCursor(0,1);   // Second line
    lcd.print(F("Check wiring!"));
    delay(2000);
    }

//----------Start SD Card----------
  // Create/Open file on SD Card
  SDcard = SD.open("results.txt", FILE_WRITE);      // Name.txt --> "Name" limited to 8 characters
  
  // if the file opened okay, write to it:
  if (SDcard) {
    Serial.println(F("Writing to file results.txt"));
    lcd.clear();          // Clears LCD display
    lcd.setCursor(0,0);   // First line
    lcd.print(F("Now opening"));
    lcd.setCursor(0,1);   // Second line
    lcd.print(F("results.txt"));
    
    SDcard.println();     // Linebreak between different Experiments
    SDcard.println(F("---------------;-----------------;----------------------;----------------;----------------"));
    SDcard.println(F("Beginning;New;Experiment"));
    SDcard.println(F("Timer;Humidity (%);Temperature (°C);CO2 (PPM);TVOC (PPB)"));
    SDcard.close(); // close the file
    delay(2000);
  }
  
  // if the file didn't open, print an error:
  else {
    Serial.println(F("Error opening results.txt"));
    lcd.clear();          // Clears LCD display
    lcd.setCursor(0,0);   // First line
    lcd.print(F("Error opening"));
    lcd.setCursor(0,1);   // Second line
    lcd.print(F("results.txt"));
    delay(2000);
  }

//----------Start DHT11 Sensor----------
  dht.begin();

//----------Start CCS881 Sensor----------
  ccs.begin();
  // Set mode CCS811_DRIVE_MODE_IDLE, CCS811_DRIVE_MODE_1SEC, CCS811_DRIVE_MODE_10SEC, CCS811_DRIVE_MODE_60SEC, CCS811_DRIVE_MODE_250MS
  ccs.setDriveMode(CCS811_DRIVE_MODE_1SEC);
  //calibrate temperature sensor
  while(!ccs.available());
  float temp = ccs.calculateTemperature();
  ccs.setTempOffset(temp - 25.0);
  
  
//----------Print starting message on LCD screen----------
  lcd.clear();          // Clears LCD display
  lcd.setCursor(0,0);   // First line
  lcd.print(F("Start measuring:"));
  lcd.setCursor(0,1);   // Second line
  lcd.print(F("H, T, CO2, TVOC"));
}




void loop() {
//----------DHT11 Readings----------
  // Wait a 2 seconds between measurements.
  delay(delaytime);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds
  int h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  int t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

//----------DHT11 To CCS881----------
  ccs.setEnvironmentalData(h, t);

//----------CCS881 Readings----------
    if(ccs.available()){
      if(!ccs.readData()){
        co2 = ccs.geteCO2();
        tvoc = ccs.getTVOC();
      }
      else{
        Serial.println(F("ERROR"));
      }
  }
  
//----------Time Calculation----------
  timer = timer + 2;

//----------Serial Monitor----------
  Serial.println(timer);
  //DHT11
  Serial.print(F("Humidity: "));  Serial.print(h);  Serial.print(F("%\t"));
  Serial.print(F("Temperature: "));  Serial.print(t);  Serial.println(F("C\t"));
  //CCS881
  Serial.print(F("CO2: "));  Serial.print(co2);  Serial.print(F("PPM\t"));
  Serial.print(F("TVOC: ")); Serial.print(tvoc);  Serial.print(F("PPB\t"));
  Serial.println();

//----------LCD Display----------
  lcd.clear();            // Clears LCD display
  lcd.setCursor(0,0);     // First line
  
  lcd.print(F("H: "));  lcd.print(h);  lcd.print(F("%"));       // Print Humidity in %
  lcd.print(F("    "));   // Space (4 blocks)
  lcd.print(F("T: "));  lcd.print(t);  lcd.print(F("C"));       // Print Temperature in °C
  
  lcd.setCursor(0,1);     // Second line
  lcd.print(F("CO2:"));  lcd.print(co2);  lcd.print(F("PPM"));  // Print CO2 in PPM
  lcd.print(F(" "));   // Space (1 block)
  lcd.print(F("t:"));  lcd.print(timer);  lcd.print(F("s"));    // Print timer count

//----------SD Card----------
  SDcard = SD.open("results.txt", FILE_WRITE);   // Open the file in write mode
  SDcard.print(timer);  SDcard.print(F(";"));    // Write timer
  SDcard.print(h);  SDcard.print(F(";"));        // Write Humidity
  SDcard.print(t);  SDcard.print(F(";"));        // Write Temerature
  SDcard.print(co2);  SDcard.print(F(";"));      // Write CO2
  SDcard.print(tvoc);                            // Write TVOC
  SDcard.println();    // New line
  SDcard.close();      // close the file
}
