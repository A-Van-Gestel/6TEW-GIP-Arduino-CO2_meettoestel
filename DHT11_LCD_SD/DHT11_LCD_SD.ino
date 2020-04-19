//----------Temporary Stuff----------
byte ppm = 0;
//----------Setup SD Card Module----------
// Connect pin GND to GND
// Connect pin VCC to 5V (NOT 3.3V)
// Connect pin MISO to 12 (Static)
// Connect pin MOSI to 11 (Static)
// Connect pin SCK to 13 (Static)
// Connect pin CS to 10 (Variable)

// Include SD Card libraries
#include <SD.h>
#include <SPI.h>

// Setup SD Card
File myFile;
int pinCS = 10; // Pin 10 on Arduino Uno

//----------Setup DHT11 sensor----------
// Connect pin 1 (Left) of the sensor to whatever your DHTPIN is (D2)
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
// Connect 3.3V to 3.3V
// Connect SDA to A4 or SDA
// Connect SCL to A5 or SCL
// Connect WAK to GND (nWake = -1) or defined nWAKE Pin

// Include CCS881 libraries
#include <Wire.h>    // I2C library
#include "ccs811.h"  // CCS811 library

// Setup CCS881
CCS811 ccs811(-1); // nWAKE, GND = -1; Digital 3 = 3

//----------Setup LCD Screen (I2C protocol)----------
// Connect GND to GND
// Connect VCC to 5V
// Connect SDA to SDA or A4
// Connect SCL to SCL or A5

// Inlcude LCD libraries
//#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Setup LCD
// 0x3F LCD I2C adress, if not working run I2C Scanner
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Change the 0x3F  i2c address to your i2c address

// lcd.noBacklight(); // turn off backlight
// lcd.backlight();   // turn on backlight.

// Setup Timer
int timer = 0;
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

  // Create/Open file on SD Card
  myFile = SD.open("measurements.txt", FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.println(F("Writing to file measurements.txt"));
    lcd.clear();          // Clears LCD display
    lcd.setCursor(0,0);   // First line
    lcd.print(F("Now opening"));
    lcd.setCursor(0,1);   // Second line
    lcd.print(F("measurements.txt"));
    myFile.println();
    myFile.println(F("Beginning New experiment"));
    myFile.println(F("Timer,Humidity (%),Temperature (°C),CO2 (PPM)"));
    myFile.close(); // close the file
    delay(2000);
  }
  
  // if the file didn't open, print an error:
  else {
    Serial.println(F("Error opening measurements.txt"));
    lcd.clear();          // Clears LCD display
    lcd.setCursor(0,0);   // First line
    lcd.print(F("Error opening"));
    lcd.setCursor(0,1);   // Second line
    lcd.print(F("measurements.txt"));
    delay(2000);
  }

  // Start DHT11 Sensor
  dht.begin();

  // Start CCS881 Sensor
  //Wire.begin();           // Enables I2C
  bool ok= ccs811.begin();
  if( !ok ) Serial.println(F("setup: CCS811 begin FAILED"));
  
  // Start measuring
  ok= ccs811.start(CCS811_MODE_1SEC);
  if( !ok ) Serial.println(F("setup: CCS811 start FAILED"));
  
  
  // Print starting message on LCD screen
  lcd.clear();          // Clears LCD display
  lcd.setCursor(0,0);   // First line
  lcd.print(F("Start measuring:"));
  lcd.setCursor(0,1);   // Second line
  lcd.print(F("H, T, HI, CO2"));
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

//----------CCS881 Readings----------
  uint16_t eco2, etvoc, errstat, raw;
  ccs811.read(&eco2,&etvoc,&errstat,&raw);
  
  
//----------Time Calculation----------
  timer = timer + 2;

//----------Serial Monitor----------
  Serial.print(timer);
  Serial.print(F("  Humidity: "));
  Serial.print(h);
  Serial.print(F("%\t"));
  Serial.print(F("Temperature: "));
  Serial.print(t);
  Serial.println(F("C\t"));

  Serial.print(F("eco2="));  Serial.print(eco2);     Serial.print(F(" ppm  "));
  Serial.print(F("etvoc=")); Serial.print(etvoc);    Serial.print(F(" ppb  "));

//----------LCD Display----------
  lcd.clear();            // Clears LCD display
  lcd.setCursor(0,0);     // First line
  
  lcd.print(F("H: "));       // Print Humidity in %
  lcd.print(h);
  lcd.print(F("%"));
  
  lcd.print(F("    "));      // Space (4 blocks)
  
  lcd.print(F("T: "));       // Print Temperature in °C
  lcd.print(t);
  lcd.print(F("C"));

  lcd.setCursor(0,1);     // Second line

  lcd.print(F("Timer: "));   // Print timer count
  lcd.print(timer); 
  lcd.print(F("sec"));

//----------SD Card----------
  myFile = SD.open("measurements.txt", FILE_WRITE);
  myFile.println(timer);
  myFile.println(",");
  myFile.println(h);
  myFile.println(",");
  myFile.println(t);
  myFile.println(",");
  myFile.println(ppm);
  myFile.close(); // close the file
}
