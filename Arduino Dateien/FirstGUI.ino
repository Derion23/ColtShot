#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library
#include <SD.h>   // libraries for SD card
#include <SPI.h>

LCDWIKI_KBV mylcd(ILI9486,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset

// CLASSES VARIABLES AND FUNCTIONS
const int numOfTanks = 6;
void print(String message);

// CLASSES
class Ingredient{
  private:
    String name;
    int amount;
  public:
    Ingredient(){
      this->name = "";
      this->amount = 0;
    }
    Ingredient(String name, int amount){
      this->name = name;
      this->amount = amount;
    }
    void setName(String name){ this->name = name; }
    void setAmount(int amount){ this->amount = amount; }
    String getName(){ return this->name; }
    int getAmount(){ return this->amount; }
};

class Cocktail{
  private:
    String name;
    Ingredient ingredients[numOfTanks];

  public:
    Cocktail(){this->name = "";}

    Cocktail(String name, Ingredient ingredients[numOfTanks]){
      this->name = name;
      for (int i = 0; i < numOfTanks; i++){
        this->ingredients[i] = Ingredient(ingredients[i].getName(), ingredients[i].getAmount());
        if(ingredients[i].getName() == "") break;
      }
    }

    String getName() {return this->name;}
    void setName(String name){this->name = name;}
    Ingredient getIngredient(int index){return this-> ingredients[index];}

    /*
    void printCocktail(){
      print (this->name+":");
      for (int i = 0; i < numOfTanks; i++){
      print(" ->" + String(this->ingredients[i].getAmount())+"cl of " + String(this->ingredients[i].getName()));
      }
    }
    */

    void printToConsole(){
      Serial.print(this->name);
      for (int i = 0; i < numOfTanks; i++){
        if(this->ingredients[i].getName() == "") break;
        Serial.print(" -> " + String(this->ingredients[i].getAmount())+"cl von " + String(this->ingredients[i].getName()));
      }
      Serial.println("");
    }
};

class Tank{
  private:
    int fillingLevel = 0;
    String content;
  public:
    Tank(){
      this->content = ""; 
      this->fillingLevel = 0;
    }
    void setContent(String content){ this->content = content; }
    void fillTo(int amount){this->fillingLevel = amount;}
    void withdraw(int amount){this->fillingLevel -= amount;}
    String getContent(){  return this->content; }
    int getAmount(){ return this->fillingLevel;}
};

class ColtShot{
  private:
    Tank tanks[numOfTanks];
  public:
    ColtShot(){}

    void addContent(String content){
      for(int i = 0; i < numOfTanks; i++){
        if(this->tanks[i].getContent() == "")
        this->tanks[i].setContent(content);
        return;
      }
    }

    void emptyTanks(){
      for (int i = 0; i < numOfTanks; i++){
        this->tanks[i].fillTo(0);
        this->tanks[i].setContent("");
      }
    }
    
    void printTanks(){
      print("Tanks:");
      for (int i = 0; i < numOfTanks; i++){
        print("Content: " + this->tanks[i].getContent() + ", Amount: " + char(this->tanks[i].getAmount()) + "cl");
      }
    }
    /*
    void TestPrintTanks(){
      print("Tanks:");
      for (int i = 0; i < numOfTanks; i++){
        Serial.println("Content: " + this->tanks[i].getContent() + ", Amount: " + String(this->tanks[i].getAmount()) + "cl");
      }
    }
    */

    String getTankContent(int index){ return this->tanks[index].getContent();}
  };

// CONSTANTS
const int textPaddingX = 10;
const int textPaddingY = 10;
const int textSpacingY = 20;
const int choiceRadioButtonRadius = 10;
const int focusedCircleRadius = 8;
const int choiceSpacingY = 25;
const int correctionCirclePosY = 3;
const int choiceMaxPosY = 280;
const int maxNumOfPages = 5;
const int textChoiceSpacingY = 15;
const int maxNumOfCocktails = 20;
const int maxNumOfIngredients = 40;
const int separatorLineX = 280;
const int separatorLineY = 80;

const String supportedIngredients[] = {"Sprudelwasser", "Whiskey", "Limettensaft", "Tequila", "Orangensaft", "Gin", "Vodka", "Rum", "Cola", "TonicWater", "OrangenLikoer", "Sirup", "Martini", "Cachaca", "GrenadineSirup",
"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p"};
const String supportedCocktails[] = {"Gin Tonic", "Margaritha", "Cuba Libre", "Daiquiri", "Vodka Gimlet", "Martini Cola", "Mojito", "Caipirinha", "Tequila Sunrise"};


// GLOBAL VARIABLES
String state = "start";
String lastState;
String secondLastState;

// maybe use instead of integers bytes for saving storage
int textPosY;
int startOfChoicesY;
int lastFocusedChoice;
int focusedChoice;
int numOfChoices;
int currentPage = 0;
int numOfPages = 1;
bool nextPageOptionPrinted = false;
bool previousPageOptionPrinted = false;
int numOfChoicesOnPage[maxNumOfPages - 1];
int skippedChoices = 0; // numOfChoices which were not printed because the were on a previous page
bool previousPageChoicesSkipped = false;

ColtShot cs;
Cocktail cocktails[maxNumOfCocktails];
String ingredients[maxNumOfIngredients];
bool chosenIngredients[maxNumOfIngredients];

// SD CARD, name of opened txt file could no be longer than 8 chars
int pinSD = 53;
int pinSDVcc = 22;
int pinSDGnd = 48;

// TURN KNOB
int pinDT = 20;
int pinCLK = 21;
int pinOkBTN = 19;
int pinBackBTN = 18;

int counter = 0;
int inputState; //0: kein Input, 1: knobTurn forward, -1: knobTurn backwards, 2: OkBTNPress, 3: BackBTNPress (maybe 0-3 -> unsigned word, weniger Speicheplatz)
int previousClkState;

// Variables to prevent counter from bouncing, jumping
long timeOfLastDebounceTurn = 0;
int delayOfDebounceTurn = 200;
long timeOfLastDebouncePress = 0;
int delayOfDebouncePress = 750;


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

  // setup Turn knob
  pinMode(pinDT, INPUT_PULLUP);
  pinMode(pinCLK, INPUT_PULLUP);
  pinMode(pinOkBTN, INPUT_PULLUP);
  previousClkState = digitalRead(pinCLK);

  // setupSD Card
  pinMode(pinSD, OUTPUT);
  pinMode(pinSDVcc, OUTPUT);
  pinMode(pinSDGnd, OUTPUT);
  digitalWrite(pinSDVcc, HIGH);
  digitalWrite(pinSDGnd, LOW);

  // INTERRUPTS, possible Interrupt PINS: 2, 3, 18, 19, 20, 21
  attachInterrupt(digitalPinToInterrupt(pinOkBTN), okButtonPress, FALLING); // if pinOkBTN HIGH -> LOW => Button pressed
  attachInterrupt(digitalPinToInterrupt(pinBackBTN), backButtonPress, FALLING); // if back btn was pressed
  attachInterrupt(digitalPinToInterrupt(pinCLK), knobTurn, CHANGE); // if clk Pin Changes

  // setUp of Screen
  mylcd.Set_Text_Mode(0);
  mylcd.Fill_Screen(BLACK);  
  mylcd.Set_Text_Back_colour(BLACK);
  mylcd.Set_Text_colour(WHITE);
  mylcd.Set_Text_Size(2);

  // setup ColtShot
  cs = ColtShot();
  initChosenIngredientsArr();

  // SD card
  if(!initializeSDCard()) return;

  downloadCocktailsFromSD();
  saveIngredientsFromCocktailsToSD();
  downloadIngredientsFromSD();

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
  else if(state == "error")
    error();
}


// MENU FUNCTIONS 
void start(){
  resetGlobalVariables();
  mylcd.Fill_Screen(BLACK);

  numOfChoices = 2;

  print("Waehle eine der folgenden Moeglich-");
  print("keiten die Maschine zu befuellen:");
  addSpacingY(textChoiceSpacingY);
  printChoice("Cocktails auswaehlen");
  printChoice("Zutaten auswaehlen");
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
  
  printAddCocktailsMenu();

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      // last Choice & next page option exists
      if(focusedChoice == numOfChoices - 1 && currentPage < numOfPages - 1){
        goToNextPage();
        printAddCocktailsMenu();
      }
      else if(focusedChoice == 0 && currentPage > 0){
        goToPreviousPage();
        printAddCocktailsMenu();
      }
      else if(focusedChoice == 0 && currentPage == 0){
        state = "addIngredients";
        return;
      }

    }

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
  
  printAddIngredientsMenu();     // sets numOfChoices

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      // last Choice & next page option exists
      if(focusedChoice == numOfChoices - 1 && currentPage < numOfPages - 1){
        goToNextPage();
        printAddIngredientsMenu();
      }
      else if(focusedChoice == 0 && currentPage > 0){
        goToPreviousPage();
        printAddIngredientsMenu();
      }
      else if(focusedChoice == 0 && currentPage == 0){
        state = "addCocktails";
        return;
      }

    }

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
  addSpacingY(textChoiceSpacingY);
  printChoice("Abbrechen");
  printChoice("Ja, zurueckgehen");

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

void error(){
  mylcd.Fill_Screen(BLACK);
  print("ERROR");
  while(true){

  }
}


// INTERRUPT FUNCTIONS
void okButtonPress(){
  // check if enough time has passed and if rotation occured
  if((millis() - timeOfLastDebouncePress) > delayOfDebouncePress){
      timeOfLastDebouncePress = millis();

      inputState = 2;
      Serial.println("OK-Button: pressed");
  }
  
}

void backButtonPress(){
  inputState = 3;
  Serial.println("Back-Button: pressed");
}

void knobTurn(){
  // check if enough time has passed and if rotation occured
  if((millis() - timeOfLastDebounceTurn) > delayOfDebounceTurn){
      timeOfLastDebounceTurn = millis();

      int clkState = digitalRead(pinCLK);
      int dtState = digitalRead(pinDT);

      if(dtState != clkState){          
          inputState = 1;
          Serial.println("Knob: positive rotation");
      } else if(dtState == clkState){
          inputState = -1;     
          Serial.println("Knob: negative rotation");
      }
    }
}


// MENU PRINT FUNCTIONS
void printAddCocktailsMenu(){
  mylcd.Fill_Screen(BLACK);

  if(currentPage == 0){
    print("Waehle eine der folgenden Cocktails aus:");
    addSpacingY(textChoiceSpacingY);
    printChoice("Zutaten hinzufuegen");
  }

  for(int i = 0; i < maxNumOfCocktails; i++){
    if(cocktails[i].getName() == "") break;
    printChoiceToPages(cocktails[i].getName());
  }

  // print tanks and separator lines
  printTanks();

  if(currentPage == 0)
    numOfChoices++; // additional "Zutaten hinzufügen" option
}

void printAddIngredientsMenu(){
  mylcd.Fill_Screen(BLACK);

  if(currentPage == 0){
    print("Waehle eine der folgenden Zutaten aus:");
    addSpacingY(textChoiceSpacingY);
    printChoice("Cocktails hinzufuegen");
  }

  for(int i = 0; i < 25; i++){
    if(ingredients[i] == "") break;
    printChoiceToPages(ingredients[i]);
  }

  // print tanks and separator lines
  printTanks();

  if(currentPage == 0)
    numOfChoices++; // additional "Cocktails hinzufuegen" option
}

// HELPER FUCTIONS
void initChosenIngredientsArr(){
  for(int i = 0; i < maxNumOfIngredients; i++){
    chosenIngredients[i] = false;
  }
}

void addSpacingY(int spacingY){
  textPosY += spacingY;
  startOfChoicesY = textPosY;
}

void print(String message){
  mylcd.Print_String(message, textPaddingX, textPosY);
  textPosY += textSpacingY;
  startOfChoicesY = textPosY;
}

void printChoice(String message){
  mylcd.Set_Draw_color(RED);
  mylcd.Print_String(message, 2 * textPaddingX + 2 * choiceRadioButtonRadius, textPosY);
  mylcd.Draw_Circle(textPaddingX + choiceRadioButtonRadius, textPosY + choiceRadioButtonRadius - correctionCirclePosY, choiceRadioButtonRadius);

  Serial.print("TextPosY: ");
  Serial.println(textPosY);

  textPosY += choiceSpacingY;
}

void printChoiceToPages(String message){
  // print option to go to previous page
  if(currentPage != 0 && !previousPageOptionPrinted){
    printChoice("Vorherige Seite");
    previousPageOptionPrinted = true;
    return;
  }

  // Skip a choice if it was printed at a previous page
  if(currentPage != 0  && !previousPageChoicesSkipped){
    int skipped = skippedChoices + 1; // first Choice was skipped because of printing prev. Page option
    for(int i = 0; i < currentPage; i++){
      skipped -= numOfChoicesOnPage[i];
      // skip choice because it was printed
      if(skipped < 0){
        skippedChoices++;
        return;
      }
    }
    previousPageChoicesSkipped = true;
  }

  // print option to go to next page
  if(textPosY >= choiceMaxPosY){
    if(nextPageOptionPrinted || numOfPages == maxNumOfPages) return;

    printChoice("Naechste Seite");
    numOfPages++;
    numOfChoices = currentPage == 0 ? numOfChoicesOnPage[currentPage] + 1 : numOfChoicesOnPage[currentPage] + 2;  // one more choice: additional prev. Page Option + next Page option
    nextPageOptionPrinted = true;
    return;
  }

  printChoice(message);
  numOfChoicesOnPage[currentPage]++;
  numOfChoices = currentPage == 0 ? numOfChoicesOnPage[currentPage] : numOfChoicesOnPage[currentPage] + 1;  // one more choice: additional prev. Page Option
}

void printTanks(){
  // print lines
  mylcd.Set_Draw_color(WHITE);
  mylcd.Draw_Fast_HLine(separatorLineX, separatorLineY, 400);
  mylcd.Draw_Fast_VLine(separatorLineX, separatorLineY, 300);

  int textPosXTanks = textPaddingX + separatorLineX;
  int textPosYTanks = textPaddingY + separatorLineY;
  mylcd.Print_String("Tanks:",textPosXTanks , textPosYTanks);
  for(int i = 0; i < numOfTanks; i++){
    textPosYTanks += textSpacingY;
    String tankContent = cs.getTankContent(i) == "" ? "leer" : cs.getTankContent(i);
    mylcd.Print_String(tankContent, textPosXTanks , textPosYTanks);
  }
}

void goToNextPage(){
  currentPage++; // went to next page
  resetGlobalVariablesForPage();
}

void goToPreviousPage(){
  currentPage--; // went to previous page
  resetGlobalVariablesForPage();
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
  resetGlobalVariablesForPage();

  numOfPages = 1;
  currentPage = 0;
  numOfChoicesOnPage[0] = 0;
}

void resetGlobalVariablesForPage(){
  nextPageOptionPrinted = false;
  previousPageOptionPrinted = false;
  skippedChoices = 0;
  previousPageChoicesSkipped = false;
  textPosY = textPaddingY;
  startOfChoicesY = textPaddingY;
  lastFocusedChoice = -1;
  focusedChoice = 0;
  counter = 0;

  numOfChoicesOnPage[currentPage] = 0;
}


// SD Card
bool initializeSDCard(){
  Serial.print("Initializing SD card...");
  if (!SD.begin(53)) {
    Serial.println("initialization failed!");
    state = "error";
    return false;
  }
  Serial.println("initialization done.");
  return true;
}

// Format: name, ingredient1, amount1, ingredient2, amount2, ..., ingredientn, amountn,;
void downloadCocktailsFromSD(){
  File cocktailTxt = SD.open("cocktail.txt");
  if(!cocktailTxt){
    // if the file didn't open, print an error:
    Serial.println("error opening cocktail.txt");
    state = "error";
    return;
  }
  
  Serial.println("downloading Cocktails");

  for(int i = 0; i < maxNumOfCocktails; i++){
    // name of Cocktail
    String name = "";
    while (cocktailTxt.available()) {
      char currentChar = cocktailTxt.read();
      if(currentChar == ',') break;
      if(currentChar != '\n')
        name += currentChar;
    }
    // get Ingredients
    Ingredient ingredients[numOfTanks];
    for(int j = 0; j < numOfTanks; j++){
      if(cocktailTxt.read() == ';') break;
      String nameOfIngredient = "";
      while (cocktailTxt.available()) {
        char currentChar = cocktailTxt.read();
        if(currentChar == ',') break;
        nameOfIngredient += currentChar;
      }

      // get rid of' ' and check for ';'
      if(cocktailTxt.read() == ';') break;
      String amountStr = "";
      while (cocktailTxt.available()) {
        char currentChar = cocktailTxt.read();
        if(currentChar == ',') break;
        amountStr += currentChar;
      }

      // Save Ingredient
      ingredients[j] = Ingredient(nameOfIngredient, amountStr.toInt());
      if(j != numOfTanks - 1)
        ingredients[j + 1] = Ingredient();
    }

    // Save Cocktail
    cocktails[i] = Cocktail(name, ingredients);
    if(i != maxNumOfCocktails - 1)
      cocktails[i + 1] = Cocktail();
  }

  // close File
  cocktailTxt.close();

/*
  ingredients[0] = Ingredient("Tequila", 4);
  ingredients[1] = Ingredient("Orangenlikoer", 4);
  ingredients[2] = Ingredient();
  cocktails[1] = Cocktail("Margheritha", ingredients);
  cocktails[2] = Cocktail();
  */
}

void downloadIngredientsFromSD(){
  File ingredientsTxt = SD.open("ingred.txt");
  if(!ingredientsTxt){
    // if the file didn't open, print an error:
    Serial.println("error opening ingred.txt");
    state = "error";
    return;
  }
  
  Serial.println("downloading Ingredients");

  for(int i = 0; i < maxNumOfIngredients; i++){
    String name = "";
    while (ingredientsTxt.available()) {
      char currentChar = ingredientsTxt.read();
      if(currentChar == ',') break;
      if(currentChar != '\n')
        name += currentChar;
    }

    // Save Ingredients
    ingredients[i] = name;
    if(i != maxNumOfIngredients - 1)
      ingredients[i + 1] = "";
  }

  
}

void saveIngredientsFromCocktailsToSD(){
  File ingredientsTxt = SD.open("ingred.txt");
  if(!ingredientsTxt){
    // if the file didn't open, print an error:
    Serial.println("error opening ingred.txt");
    state = "error";
    return;
  }

  Serial.println("saving Ingredients");

  for(int i = 0; i < maxNumOfCocktails; i++){
    if(cocktails[i].getName() == "") return;

    for(int j = 0; j < numOfTanks; j++){
      // open file for reading
      ingredientsTxt = SD.open("ingred.txt");

      String ingredient = cocktails[i].getIngredient(j).getName();
      if(ingredient == "") break;

      // check if ingredient is in ingred.txt
      bool isInside = false;
      while(ingredientsTxt.available()){
        String name = "";
        while (ingredientsTxt.available()) {
          char currentChar = ingredientsTxt.read();
          if(currentChar == ',') break;
          if(currentChar != '\n')
            name += currentChar;
        }
        if(name == ingredient){
          isInside = true;
          break;
        }
      }

      // if not write it to that file
      if(!isInside){
        ingredientsTxt.close();
        ingredientsTxt = SD.open("ingred.txt", FILE_WRITE);
        ingredientsTxt.print(ingredient);
        ingredientsTxt.print(",");
        ingredientsTxt.close();
      }
    }
  }

  ingredientsTxt.close();
}












