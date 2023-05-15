#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

LCDWIKI_KBV mylcd(ILI9486,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset

// CONSTANTS
const int textPaddingX = 10;
const int textPaddingY = 10;
const int textSpacingY = 20;
const int choiceRadioButtonRadius = 10;
const int focusedCircleRadius = 8;
const int choiceSpacingY = 25;
const int correctionCirclePosY = 3;

// GLOBAL VARIABLES
String state = "start";

int textPosY;
int startOfChoicesY;
bool turningAllowed;      // turning of knob allowed?
int lastFocusedChoice;
int focusedChoice;
int numOfChoices;


// DREHKNOPF
int pinDT = 43;
int pinCLK = 45;
int pinBTN = 41;

int counter = 0;
int previousCklState;

// Variables to prevent counter from bouncing, jumping
long timeOfLastDebounce = 0;
int delayOfDebounce = 250;


// DEFINITIONS
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define VERTICAL 0
#define HORIZONTAL 3


void setup() {
  Serial.begin(9600);
  mylcd.Init_LCD();
  mylcd.Set_Rotation(HORIZONTAL);

  // setup Drehknopf
  pinMode(pinDT, INPUT_PULLUP);
  pinMode(pinCLK, INPUT_PULLUP);
  pinMode(pinBTN, INPUT_PULLUP);
  previousCklState = digitalRead(pinCLK);

  // setUp of Screen
  mylcd.Set_Text_Mode(0);
  mylcd.Fill_Screen(BLACK);  
  mylcd.Set_Text_Back_colour(BLACK);
  mylcd.Set_Text_colour(WHITE);
  mylcd.Set_Text_Size(2);

  Serial.println("Program started...");
}


void loop() {
  if(state == "start")
    start();
  else if(state == "addCocktails")
    addCocktails();
  else if(state == "addIngredients")
    addIngredients();
  else if(state == "end")
    end();
}


// MENU FUNCTIONS 
void start(){
  resetGlobalVariables();
  numOfChoices = 2;

  print("Waehle eine der folgenden Moeglich-");
  print("keiten die Maschine zu befuellen:");
  print("");
  printChoice("Cocktails auswaehlen");
  printChoice("Zutaten auswaehlen");
  turningAllowed = true;
  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isButtonPressed()){
      if(focusedChoice == 0)
        state = "addCocktails";
      else if(focusedChoice == 1)
        state = "addIngredients";
        
      return;
    }
  }
}

void addCocktails(){
  mylcd.Fill_Screen(BLACK);
  print("addCocktails");
  while(true){

  }
}

void addIngredients(){
  mylcd.Fill_Screen(BLACK);
  print("addIngredients");
  while(true){

  }
}

void end(){
  mylcd.Fill_Screen(BLACK);
  print("End");
  while(true){

  }
}


// HELPER FUCTIONS
void print(String message){
  mylcd.Print_String(message, textPaddingX, textPosY);
  textPosY += textSpacingY;
  startOfChoicesY = textPosY;
}

void printChoice(String message){
  mylcd.Set_Draw_color(RED);
  mylcd.Print_String(message, 2 * textPaddingX + 2 * choiceRadioButtonRadius, textPosY);
  mylcd.Draw_Circle(textPaddingX + choiceRadioButtonRadius, textPosY + choiceRadioButtonRadius - correctionCirclePosY, choiceRadioButtonRadius);
  textPosY += choiceSpacingY;
}

void focus(){
  // Check if refocus is needed
  if(lastFocusedChoice == focusedChoice) return;

  // defocus marked choice
  if(lastFocusedChoice != -1){
    mylcd.Set_Draw_color(BLACK);
    mylcd.Fill_Circle(textPaddingX + choiceRadioButtonRadius, startOfChoicesY + choiceSpacingY * lastFocusedChoice + choiceRadioButtonRadius - correctionCirclePosY, focusedCircleRadius);
  }

  // focus new Choice
  mylcd.Set_Draw_color(RED);
  mylcd.Fill_Circle(textPaddingX + choiceRadioButtonRadius, startOfChoicesY + choiceSpacingY * focusedChoice + choiceRadioButtonRadius - correctionCirclePosY, focusedCircleRadius);


  lastFocusedChoice = focusedChoice;
}

int getTurnKnobIndex(){
  // Check if turning is allowed
  if(!turningAllowed) return 0;

  int clkState = digitalRead(pinCLK);
  int dtState = digitalRead(pinDT);

  // check if enough time has passed and if rotation occured
  if((millis() - timeOfLastDebounce) > delayOfDebounce && clkState != previousCklState){
    timeOfLastDebounce = millis();

    if(dtState != clkState && counter < numOfChoices - 1){
      counter++;
    } else if(dtState == clkState && counter > 0){
      counter--;
    }
    Serial.println(counter);
  }

  // update previousCklState
  previousCklState = clkState;
  return counter;
}

bool isButtonPressed(){
  // check if button is pressed, maybe implement a short block time after the button was pressed
  if(digitalRead(pinBTN) == LOW){
    Serial.println("Button pressed");
    return true;
  }
  return false;
}

void resetGlobalVariables(){
  textPosY = textPaddingY;
  startOfChoicesY = 0;
  turningAllowed = false;      // Drehen des Drehknopfes erlaubt?
  lastFocusedChoice = -1;
  focusedChoice = 0;
}

















