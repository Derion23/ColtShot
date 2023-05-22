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

const String supportedIngredients[] = {"Sprudelwasser", "Whiskey", "Limettensaft", "Tequila", "Orangensaft", "Gin", "Vodka", "Rum", "Cola", "TonicWater", "OrangenLikör", "Sirup", "Martini", "Cachaca", "GrenadineSirup"};
const String supportedCocktails[] = {"Gin Tonic", "Margaritha", "Cuba Libre", "Daiquiri", "Vodka Gimlet", "Martini Cola", "Mojito", "Caipirinha", "Tequila Sunrise"};


// GLOBAL VARIABLES
String state = "start";
String lastState;
String secondLastState;

// maybe use instead of integers bytes for saving storage
int textPosY;
int startOfChoicesY;
bool turningAllowed;      // turning of knob allowed?
int lastFocusedChoice;
int focusedChoice;
int numOfChoices;


// DREHKNOPF
int pinDT = 20;
int pinCLK = 21;
int pinOkBTN = 19;
int pinBackBTN = 18;

int counter = 0;
int inputState; //0: kein Input, 1: knobTurn forward, -1: knobTurn backwards, 2: OkBTNPress, 3: BackBTNPress (maybe 0-3 -> unsigned word, weniger Speicheplatz)
int previousClkState;

// Variables to prevent counter from bouncing, jumping
long timeOfLastDebounce = 0;
int delayOfDebounce = 200;


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
  pinMode(pinOkBTN, INPUT_PULLUP);
  previousClkState = digitalRead(pinCLK);
  // INTERRUPTS, possiblw Interrupt PINS: 2, 3, 18, 19, 20, 21
  attachInterrupt(digitalPinToInterrupt(pinOkBTN), okButtonPress, FALLING); // if pinOkBTN HIGH -> LOW => Button pressed
  attachInterrupt(digitalPinToInterrupt(pinBackBTN), backButtonPress, FALLING); // if back btn was pressed
  attachInterrupt(digitalPinToInterrupt(pinCLK), knobTurn, CHANGE); // if clk Pin Changes

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
  else if(state == "confirmGoingBack")
    confirmGoingBack();
  else if(state == "end")
    end();
}


// MENU FUNCTIONS 
void start(){
  resetGlobalVariables();
  mylcd.Fill_Screen(BLACK);

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

    if(isOkButtonPressed()){
      if(focusedChoice == 0)
        state = "addCocktails";
      else if(focusedChoice == 1)
        state = "addIngredients";
        
      return;
    }
  }
}

void addCocktails(){
  resetGlobalVariables();
  mylcd.Fill_Screen(BLACK);

  numOfChoices = 8;

  print("Waehle eine der folgenden Cocktails aus:");
  print("");
  
  for(int i = 0; i < 8; i++){
    printChoice(supportedCocktails[i]);
  }

  turningAllowed = true;
  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isBackButtonPressed()){
      lastState = "addCocktails";
      secondLastState = "start";
      state = "confirmGoingBack";
      return;
    }
  }
}

void addIngredients(){
  resetGlobalVariables();
  mylcd.Fill_Screen(BLACK);

  numOfChoices = 10;

  print("Waehle eine der folgenden Zutaten aus:");
  print("");
  
  for(int i = 0; i < 10; i++){
    printChoice(supportedIngredients[i]);
  }

  turningAllowed = true;
  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isBackButtonPressed()){
      lastState = "addIngredients";
      secondLastState = "start";
      state = "confirmGoingBack";
      return;
    }
  }
}

void confirmGoingBack(){
  resetGlobalVariables();
  mylcd.Fill_Screen(BLACK);

  numOfChoices = 2;

  print("Wirklich zurueckgehen?");
  print("");
  printChoice("Abbrechen");
  printChoice("Ja, zurueckgehen");

  turningAllowed = true;
  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      if(focusedChoice == 0){
        state = lastState;
      } else if(focusedChoice == 1){
        state = secondLastState;
      }

      return;
    }
  }
}


void end(){
  mylcd.Fill_Screen(BLACK);
  print("End");
  while(true){

  }
}


// INTERRUPT FUNCTIONS
void okButtonPress(){
  inputState = 2;
  Serial.println("OK-Button: pressed");
}

void backButtonPress(){
  inputState = 3;
  Serial.println("Back-Button: pressed");
}

void knobTurn(){
  int clkState = digitalRead(pinCLK);
  int dtState = digitalRead(pinDT);

  // check if enough time has passed and if rotation occured
  if((millis() - timeOfLastDebounce) > delayOfDebounce){
      timeOfLastDebounce = millis();

      if(dtState != clkState){          
          inputState = 1;
          Serial.println("Knob: positive rotation");
      } else if(dtState == clkState){
          inputState = -1;     
          Serial.println("Knob: negative rotation");
      }
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

  if (counter < numOfChoices-1 && inputState == 1){
    counter++;
    Serial.println(counter);
    inputState = 0;
  }
  else if (counter > 0 && inputState == -1){
    counter --;
    Serial.println(counter);
    inputState = 0;
  }

  return counter;
}

bool isOkButtonPressed(){
  // check if Ok-Button is pressed, maybe implement a short block time after the button was pressed
  if(inputState == 2){
    inputState = 0;
    return true;
  }
  return false;
}

bool isBackButtonPressed(){
  // check if Back-Button is pressed, maybe implement a short block time after the button was pressed
  if(inputState == 3){
    inputState = 0;
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
  counter = 0;
}

















