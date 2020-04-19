#include <LiquidCrystal.h>
#include "Adafruit_CCS811.h"

LiquidCrystal lcd(12, 13, 8, 9, 10, 11); /// REGISTER SELECT PIN,ENABLE PIN,D4 PIN,D5 PIN, D6 PIN, D7 PIN
Adafruit_CCS811 ccs;

void setup() {
  lcd.begin(16, 2);  
  ccs.begin();
  //calibrate temperature sensor
  while(!ccs.available());
  float temp = ccs.calculateTemperature();
  ccs.setTempOffset(temp - 25.0);
}

void loop() {
  if(ccs.available()){
    float temp = ccs.calculateTemperature();
    if(!ccs.readData()){
      int co2 = ccs.geteCO2();
      int tvoc = ccs.getTVOC();
      lcd.setCursor(0, 0);
      lcd.print(String ("CO2:")+ String (co2)+String(" PPM"));
      
      lcd.setCursor(0, 1);
      lcd.print(String ("TVOC:")+ String (tvoc)+String(" PPB "));
      lcd.print(String("T:"+String (int(temp)))+String("C"));

      delay(3000);
      lcd.clear();      
    }
    else{
      lcd.print("ERROR");
      while(1);
    }
  }

}
