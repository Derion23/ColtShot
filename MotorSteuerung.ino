#include <AFMotor.h>
//Parameter fuer HauptMotor
#define HauptMotorGeschwindigkeit 128 //0-255 (PWM)
#define delayTime 500 
#define sensorPin A5
#define NummerHauptMotor 1    //die Nummer der Motorschnittstelle auf Controll Board L298n

//Parameter fuer NemaMotor
#define dirPin1 26  //Nema Motor für Rührer hoch und runterfahren
#define stepPin1 27
#define dirPin2 28  //Nema Motor für Rührer drehen
#define stepPin2 29
#define NemaMotor1Geschwindigkeit 2000  //je kleiner, desto schneller
#define NemaMotor2Geschwindigkeit 2000  //je kleiner, desto schneller
#define NemaMotor1Drehwinkel 400 //720° drehen
#define NemaMotor2Drehwinkel 800 //1440° drehen
   //fuer Stepper Motor: Jeder Step wird der Motor 1,8° drehen
#define Verzoegerung 500


AF_DCMotor HauptMotor(NummerHauptMotor);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  HauptMotor.setSpeed(HauptMotorGeschwindigkeit);
  pinMode(stepPin1,OUTPUT);
  pinMode(stepPin2,OUTPUT);
  pinMode(dirPin1,OUTPUT);
  pinMode(dirPin2,OUTPUT);


}

void loop() {
}

void RotateClockwise(){
  bool Zustand = digitalRead(sensorPin);
  // HIGH, wenn nichts im Weg. LOW, wenn eine weisse Markierung auf der Flasche ist.
  HauptMotor.run(FORWARD);
  delay(delayTime); 
  while(Zustand == 1){
    HauptMotor.run(FORWARD);
  }
  HauptMotor.run(RELEASE);
}

void RotateCounterclockwise(){
  bool Zustand = digitalRead(sensorPin);
  HauptMotor.run(BACKWARD);
  delay(delayTime);
  while(Zustand == 1){
    HauptMotor.run(BACKWARD);
  }
  HauptMotor.run(RELEASE);
}

void Ruehrer(){
  digitalWrite(dirPin1,LOW);  //im Uhrzeigersinn einstellen
  for(int x=0; x<NemaMotor1Drehwinkel; x++){
    digitalWrite(stepPin1,HIGH);
    delayMicroseconds(NemaMotor1Geschwindigkeit);
    digitalWrite(stepPin1,LOW);
    delayMicroseconds(NemaMotor1Geschwindigkeit);
  }
  delay(Verzoegerung);

  digitalWrite(dirPin2,LOW);
  for(int x=0;x<NemaMotor2Drehwinkel;x++){
    digitalWrite(stepPin2,HIGH);
    delayMicroseconds(NemaMotor2Geschwindigkeit);
    digitalWrite(stepPin2,LOW);
    delayMicroseconds(NemaMotor2Geschwindigkeit);
  }

  delay(Verzoegerung);
  
  digitalWrite(dirPin1,HIGH); 
  for(int x=0; x<NemaMotor1Drehwinkel; x++){
    digitalWrite(stepPin1,HIGH);
    delayMicroseconds(NemaMotor1Geschwindigkeit);
    digitalWrite(stepPin1,LOW);
    delayMicroseconds(NemaMotor1Geschwindigkeit);
  }

}