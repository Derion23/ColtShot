const byte sensorPin = 19;
int irCount = 0;				//zählt, wie oft etwas den Sensor ausgelöst hat
bool detected = false;




void detectedINT(){
	if (!digitalRead(sensorPin)){
		detected = true;
		irCount++;
		return();
	}
	else{
		detected = false;
	}
}


int getIRCountAndReset(){
	int temp = irCount;
	irCount = 0;
	return temp;
}


//implementiert das Ausgabemuster aus dem Beispielprogramm:

void setup(){
	attachInterrupt(digitalPinToInterrupt(sensorPin), detectedINT, CHANGING);
	Serial.begin(9600);
	Serial.println("IR Sensor");
}

bool lastDetected = false;
void loop(){
	if (!lastDetected && detected){
		lastDetected = true;
		Serial.print(F("Neues Hindernis wahrgenommen! "));
		while (detected){
			Seriel.print(".");
			delay(20);
		}
		Serial.println(F("entfernt"));
		latDetected = false;
	}
}

//========================================================================
// nur eine überlegung
bool move = false; //zustand, ob motor sich dreht?
//=>
void moveNTanks(int nTanks){
	irCount = 0;
	while(irCount < nTanks){} //wwartet während die Maschine n Tanks weiterrotiert
	move = false;
	irCount = 0;
	return();
}
// ------> bis hier: iwie so in der Art könnte man dann den Motor über eine Funktion steuern
//========================================================================