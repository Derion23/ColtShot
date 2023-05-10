#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

LCDWIKI_KBV mylcd(ILI9486,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset

int pinDT = 43;
int pinCLK = 45;
int pinBTN = 41;

int counter = 0;
int previousCklState;

// Variables to prevent counter from bouncing, jumping
long timeOfLastDebounce = 0;
int delayOfDebounce = 250;

//define some colour values
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

void setup() {
  Serial.begin(9600);
  mylcd.Init_LCD();

  pinMode(pinDT, INPUT_PULLUP);
  pinMode(pinCLK, INPUT_PULLUP);
  pinMode(pinBTN, INPUT_PULLUP);

  previousCklState = digitalRead(pinCLK);

  // setUp of Screen
  // no idea what this does :)
  mylcd.Set_Text_Mode(0);
  mylcd.Fill_Screen(BLACK);  
  mylcd.Set_Text_Back_colour(BLACK);
  mylcd.Set_Text_colour(WHITE);
  mylcd.Set_Text_Size(10);
  mylcd.Print_String("0", 105, 180);

  Serial.println("Program started...");
}

void loop() {
  int clkState = digitalRead(pinCLK);
  int dtState = digitalRead(pinDT);

  // check if enough time has passed and if rotation occured
  if((millis() - timeOfLastDebounce) > delayOfDebounce && clkState != previousCklState){
    timeOfLastDebounce = millis();

    if(dtState != clkState){
      counter++;
    } else{
      counter--;
    }
    Serial.println(counter);
    mylcd.Print_String(String(counter), 100, 170);
  }

  // check if button is pressed, maybe implement a short block time after the button was pressed
  if(digitalRead(pinBTN) == LOW){
    Serial.println("Button pressed");
    mylcd.Print_String("pressed", 100, 250);
  }

  // update previousCklState
  previousCklState = clkState;
}









