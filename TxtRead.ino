#include <SD.h>
#include <SPI.h>

File myFile;
const int SDpin;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:

}

void readTet(){
  if( !SD.begin(SDpin)){  //SD.begin(pin) Initialisiren
    Serial.println("Initialisierung erfolgt nicht.");
    return;
  }  
  myFile = SD.open("Dateiname.txt");  // eine Datei auf der SD-Karte Öffnen
  if(myFile){
    while(myFile.available()){  //checken, ob es einlesbare Datei gibt
      Serial.write(myFile.read());  //1 Bit Data einlesen 
    }
  myFile.close();
  }
}