
//Parameter fuer HauptMotor
#define HauptMotorGeschwindigkeit 128 //0-255 (PWM)
#define delayTime1 500 
#define sensorPin A5
#define ENA 9
#define IN1 8
#define IN2 7

//Parameter fuer NemaMotor(Ruehrer)
#define dirPin1 26 
#define stepPin1 27
#define NemaMotor1Geschwindigkeit 2000  //je kleiner, desto schneller
  //Diese Zahl bedeutet, die Zeit(microseconds) zwischen jedem Schritt
#define NemaMotor1Drehwinkel 400 //720° drehen
   //fuer Stepper Motor: Jeder Step wird der Motor 1,8° drehen
#define Verzoegerung1 500

//Parameter fuer NemaMotor(Ausloeser)
#define dirPin2 28
#define stepPin2 29
#define NemaMotor2Geschwindigkeit 2000
#define NemaMotor2Drehwinkel 400
#define Verzoegerung2 5000

//Parameter fuer DCMotor (Ruehrer)
#define dcGeschwindigkeit 128
#define ENB 6
#define IN3 5
#define IN4 4
#define runTime 2000

class DCMotor{
  int EN,in1,in2;

  public:
  DCMotor(int a, int b, int c){
    EN = a; in1 = b; in2 = c;
    pinMode(EN,OUTPUT);
    pinMode(in1,OUTPUT);
    pinMode(in2,OUTPUT);
  }
  void setSpeed(int v){
    analogWrite(EN,v);
  }
  void runClockwise(){
    digitalWrite(in1, HIGH);
    digitalWrite(in2,LOW);
  }
  void runCounterClockwise(){
    digitalWrite(in1,LOW);
    digitalWrite(in2,HIGH);
  }
  void stop(){
    digitalWrite(in1,LOW);
    digitalWrite(in2,LOW);
  }
};

class StepperMotor{
  int dirPin, stepPin;

  public:
  StepperMotor(int a, int b){
    dirPin = a; stepPin = b;
    pinMode(dirPin, OUTPUT);
    pinMode(stepPin,OUTPUT);
  }
  void setDirection(int i){
    if(i==0){ digitalWrite(dirPin, LOW); }   //clockwise
    else{digitalWrite(dirPin,HIGH);}
  }
  void run(int winkel, int geschwindigkeit){
    for(int x=0;x<winkel;x++){
      digitalWrite(stepPin,HIGH);
      delayMicroseconds(geschwindigkeit);
      digitalWrite(stepPin,LOW);
      delayMicroseconds(geschwindigkeit);
    }
  }
};

DCMotor HauptMotor(ENA, IN1, IN2);
DCMotor ruehrerDCMotor(ENB,IN3,IN4);
StepperMotor RuehrerMotor(dirPin1, stepPin1);
StepperMotor DosiererMotor(dirPin2, stepPin2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
}

//Funktionen fuer Hauptmotor:

void RotateClockwise(){
  HauptMotor.setSpeed(HauptMotorGeschwindigkeit);
  bool Zustand = digitalRead(sensorPin);
  // HIGH, wenn nichts im Weg. LOW, wenn eine weisse Markierung auf der Flasche ist.
  HauptMotor.runClockwise();
  delay(delayTime1); 
  while(Zustand == 1){
    HauptMotor.runClockwise();
  }
  HauptMotor.stop();
}

void RotateCounterclockwise(){
  HauptMotor.setSpeed(HauptMotorGeschwindigkeit);
  bool Zustand = digitalRead(sensorPin);
  HauptMotor.runCounterClockwise();
  delay(delayTime1);
  while(Zustand == 1){
    HauptMotor.runCounterClockwise();
  }
  HauptMotor.stop();
}

//Funktion fuer Ruehrer

void Ruehrer(){
  ruehrerDCMotor.setSpeed(dcGeschwindigkeit);

  RuehrerMotor.setDirection(0);
  RuehrerMotor.run(NemaMotor1Drehwinkel, NemaMotor1Geschwindigkeit);
  delay(Verzoegerung1);

  ruehrerDCMotor.runClockwise();
  delay(runTime);
  ruehrerDCMotor.stop();
  delay(Verzoegerung1);
  
  RuehrerMotor.setDirection(1);
  RuehrerMotor.run(NemaMotor1Drehwinkel,NemaMotor1Geschwindigkeit);
}

//Funktion fuer Dosierer
void Dosierer(){
  DosiererMotor.setDirection(0);
  DosiererMotor.run(NemaMotor2Drehwinkel,NemaMotor2Geschwindigkeit);

  delay(Verzoegerung2);

  DosiererMotor.setDirection(1);
  DosiererMotor.run(NemaMotor2Drehwinkel,NemaMotor2Geschwindigkeit);
}
