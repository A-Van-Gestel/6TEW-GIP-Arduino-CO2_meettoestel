//----------Temporary Stuff----------
int ppm = 0;
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

//----------Setup LCD Screen (I2C protocol)----------
// Connect GND to GND
// Connect VCC to 5V
// Connect SDA to SDA or A4
// Connect SCL to SCL or A5

// Inlcude LCD libraries
#include <Wire.h>
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
  Serial.println("Beginning Checkup...");

  // Start LCD Display
  lcd.begin(16,2);  // Set LCD Type 16x2
  lcd.clear();      // Clears LCD display

  // SD Card Initialization
  if (SD.begin()) {
    Serial.println("SD card is ready to use.");
    lcd.print("SD Card Ready");
    delay(2000);
    }
  else {
    Serial.println("SD card initialization failed");
    lcd.print("SD Card Failed");
    lcd.setCursor(0,1);   // Second line
    lcd.print("Check wiring!");
    delay(2000);
    }

  // Create/Open file on SD Card
  myFile = SD.open("measurements.txt", FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.println("Writing to file measurements.txt");
    lcd.clear();          // Clears LCD display
    lcd.setCursor(0,0);   // First line
    lcd.print("Now opening");
    lcd.setCursor(0,1);   // Second line
    lcd.print("measurements.txt");
    myFile.println();
    myFile.println("Beginning New experiment");
    myFile.println("Timer,Humidity (%),Temperature (°C),CO2 (PPM)");
    myFile.close(); // close the file
    delay(2000);
  }
  
  // if the file didn't open, print an error:
  else {
    Serial.println("Error opening measurements.txt");
    lcd.clear();          // Clears LCD display
    lcd.setCursor(0,0);   // First line
    lcd.print("Error opening");
    lcd.setCursor(0,1);   // Second line
    lcd.print("measurements.txt");
    delay(2000);
  }

  // Start DHT11 Sensor
  dht.begin();
  
  // Print starting message on LCD screen
  lcd.clear();          // Clears LCD display
  lcd.setCursor(0,0);   // First line
  lcd.print("Start measuring:");
  lcd.setCursor(0,1);   // Second line
  lcd.print("H, T, HI, CO2");
}

void loop() {
  // Wait a 2 seconds between measurements.
  delay(delaytime);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds
  int h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  int t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

//----------Time Calculation----------
  timer = timer + 2;

//----------Serial Monitor----------
  Serial.print(timer);
  Serial.print("  Humidity: ");
  Serial.print(h);
  Serial.print("%\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println("C\t");

//----------LCD Display----------
  lcd.clear();            // Clears LCD display
  lcd.setCursor(0,0);     // First line
  
  lcd.print("H: ");       // Print Humidity in %
  lcd.print(h);
  lcd.print("%");
  
  lcd.print("    ");      // Space (4 blocks)
  
  lcd.print("T: ");       // Print Temperature in °C
  lcd.print(t);
  lcd.print("C");

  lcd.setCursor(0,1);     // Second line

  lcd.print("Timer: ");   // Print timer count
  lcd.print(timer); 
  lcd.print("sec");

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
