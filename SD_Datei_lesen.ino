#include <SD.h>
#include <SPI.h>
#include <LCDWIKI_GUI.h>
#include <LCDKIWI_SPI.h>

#define LCD_CS
#define LCD_RST
#define LCD_CD
#define SD_CS

File myFile;
File imageFile;

LCDWIKI_SPI lcd(LCD_RST,LCD_CS,LCD_DC);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  if( !SD.begin(SDpin)){  //SD.begin(pin) Initialisiren
    Serial.println("Initialisierung erfolgt nicht.");
    return;
  }  
}

void loop() {
  // put your main code here, to run repeatedly:

}

void readTet(){
  myFile = SD.open("Dateiname.txt");  // eine Datei auf der SD-Karte Öffnen
  if(myFile){
    while(myFile.available()){  //checken, ob es einlesbare Datei gibt
      Serial.write(myFile.read());  //1 Bit Data einlesen 
    }
  myFile.close();
  }
}

void showBild(){
  imageFile = SD.open("imageName.jpg");

  if(imageFile){
    lcd.LCD_Show_Image(0,0,320,240,imageFile);
    imageFile.close();
  }else{
    Serial.println("Error.")
  }
}
