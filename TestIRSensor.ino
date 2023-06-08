/*
  Allgemeine Infos zum Sensor:
  - kann nur reflektierende Objekte wahrnehmen (schwarz wird nicht erkannt)
  - mit Potentiometer sollte die Sensitivität einstellbar sein
  - Umgebungsbedingungen, z.B. Sonnenlicht, können Sensorgenauigkeit beeinflussen --> evtl. erneute Kalibrierung notwendig

  Je nachdem, wie fein der Sensor reagiert, sollten wir die Position der markierten Tankflaschen durch die Anzahl der Aufrufe der 
  inputHeld-Funktion recht genau bestimmen können.
*/

// CONSTANTS
// Pin, mit output des IR Sensors verbunden
const byte sensorPin = 2;

// GLOBALS
// Flag, zeigt an, ob Hindernis wahrgenommen wird
bool detected = false; 

void setup() {
  Serial.begin(9600);
  Serial.println("IR Sensor");
}

// Funktion wird aufgerufen, wenn neues Hindernis wahrgenommen wird
void inputDetected() {
  Serial.print(F("Neues Hindernis wahrgenommen! "));
  detected = true;
}

// Funktion wird wiederholt aufgerufen, wenn das Hindernis im "Blickfeld" des Sensors verweilt
void inputHeld() {
  Serial.print(".");
}

// Funktion wird aufgerufen, wenn Hindernis entfernt wird
void inputRemoved() {
  Serial.println(F("entfernt"));
  detected = false;
}

void loop() {
  // Sensor gibt LOW, wenn Hindernis, und sonst HIGH
  int reading =! digitalRead(sensorPin);
  // Wenn Hindernis erkannt
  if(reading) {
    // ... und davor (im vorherigen Frame) kein Hindernis wahrgenommen wurde
    if(detected == false) {
      //Hindernis ist neu
      inputDetected();
    }
    // ... oder das Hindernis immer noch wahrgenommen wird
    else {
      inputHeld();
    }
  }
  // Wenn gerade kein Hindernis wahrgenommen wird
  else {
    // Wenn davor (im vorherigen Frame) aber ein Hindernis wahrgenommen wurde
    if(detected) {
      inputRemoved();
    }
  }
  delay(20);
}
