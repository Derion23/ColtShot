#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library
#include <SD.h>   // libraries for SD card
#include <SPI.h>

LCDWIKI_KBV mylcd(ILI9486,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset

// CLASSES VARIABLES AND FUNCTIONS
const byte numOfTanks = 6;
byte numOfEmptyTanks = numOfTanks;
byte currentTankIndex = 0;
void saveTankContentsToSD(String lasState);

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
    bool mix = false;

  public:
    Cocktail(){this->name = "";}

    Cocktail(String name, Ingredient ingredients[numOfTanks], bool mix){
      this->name = name;
      this->mix = mix;
      for (int i = 0; i < numOfTanks; i++){
        this->ingredients[i] = Ingredient(ingredients[i].getName(), ingredients[i].getAmount());
        if(ingredients[i].getName() == "") break;
      }
    }

    String getName() {return this->name;}
    void setName(String name){this->name = name;}
    Ingredient getIngredient(int index){return this->ingredients[index];}
    void setIngredientName(int index, String ingredientName){this->ingredients[index].setName(ingredientName);}
    void setIngredientAmount(int index, int amount){this->ingredients[index].setAmount(amount);}

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
    int getFillingLevel(){ return this->fillingLevel;}
};

class ColtShot{
  private:
    Tank tanks[numOfTanks];
  public:
    ColtShot(){}

    void addContent(String content){
      for(int i = 0; i < numOfTanks; i++){
        if(this->tanks[i].getContent() == ""){
          this->tanks[i].setContent(content);
          numOfEmptyTanks--;
          return;
        }
      }
    }

    void emptyTanks(){
      for (int i = 0; i < numOfTanks; i++){
        this->tanks[i].fillTo(0);
        this->tanks[i].setContent("");
      }
      numOfEmptyTanks = numOfTanks;
      currentTankIndex = 0;
    }

    void fillTankTo(int index, int fillingLevel){
      this->tanks[index].fillTo(fillingLevel);
      saveTankContentsToSD(F("masterOverview"));
    }

    void withdraw(int index, int amount){
      this->tanks[index].fillTo(this->tanks[index].getFillingLevel() - amount);
    }

    void fillTankToSetup(int index, int fillingLevel){
      this->tanks[index].fillTo(fillingLevel);
    }

    String getTankContent(int index){ return this->tanks[index].getContent();}
    int getTankFillingLevel(int index){ return this->tanks[index].getFillingLevel();}
  };

// CONSTANTS
const byte maxNumOfCocktails = 30;
const byte maxNumOfIngredients = 60;

const byte textPaddingX = 10;
const byte textPaddingY = 10;
const byte textSpacingY = 20;
const byte choiceRadioButtonRadius = 10;
const byte focusedCircleRadius = 8;
const byte choiceSpacingY = 25;
const byte correctionCirclePosY = 3;
const int choiceMaxPosY = 280;
const byte maxNumOfPages = 5;
const byte textChoiceSpacingY = 15;
const int separatorLineX = 280;
const int separatorLineWideX = 220;
const byte separatorLineY = 80;
const byte seperatorLineWideY = 170;
const byte maxNumOfCharsPerLine = 37;
const byte tankMarkToCl = 10;
const byte printTanksWideX = 25;
const byte maxGlassCapacity = 30;     // a Cocktail has to be smaller than that capacity
const byte maxAmountPerIngredient = 30;
const byte shotSize = 2;
const byte maxRandomNum = 3; // exclusive, determines probability in Game

// GLOBAL VARIABLES
String state = "restoreFromSDCard";
String lastState;
String secondLastState;

// maybe use bytes instead of integers to save storage
int textPosY;
int startOfChoicesY;
byte lastFocusedChoice;
byte focusedChoice;
byte numOfChoices;
byte currentPage = 0;
byte numOfPages = 1;
bool nextPageOptionPrinted = false;
bool previousPageOptionPrinted = false;
byte numOfChoicesOnPage[maxNumOfPages - 1];
byte skippedChoices = 0; // numOfChoices which were not printed because the were on a previous page
bool previousPageChoicesSkipped = false;
String goingBackMessage = "";
byte numIngredsCustomCocktail = 0;
byte numOfIngredsGame = 0;
byte currentGlassCapacity = maxGlassCapacity;

ColtShot cs;
Cocktail cocktails[maxNumOfCocktails];
Cocktail customCocktail;
bool possibleCocktails[maxNumOfCocktails];
bool availableCocktails[maxNumOfCocktails];

String ingredients[maxNumOfIngredients];
bool chosenIngredients[maxNumOfIngredients];
Ingredient chosenIngredientsGame[numOfTanks];

// SD CARD, name of opened txt file could not be longer than 8 chars
const byte pinSD = 53;
const byte pinSDVcc = 22;
const byte pinSDGnd = 48;

// TURN KNOB
const byte pinDT = 20;
const byte pinCLK = 21;
const byte pinOkBTN = 19;
const byte pinBackBTN = 18;

int counter = 0;
byte inputState; //0: kein Input, 1: knobTurn backwards, 2: knobTurn forward, 3: OkBTNPress, 4: BackBTNPress
byte previousClkState;

// Variables to prevent counter from bouncing, jumping
long timeOfLastDebounceTurn = 0;
int delayOfDebounceTurn = 200;
long timeOfLastDebouncePress = 0;
int delayOfDebouncePress = 750;


// DEFINITIONS
#define BLACK   0x0000
#define RED     0xF800
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
  initPossibleCocktails();
  initAvailableCocktails();
  initChosenIngredientsArr();

  // SD card
  if(!initializeSDCard()) return;

  downloadCocktailsFromSD();
  saveIngredientsFromCocktailsToSD();
  downloadIngredientsFromSD();

  // RANDOM - randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));

  Serial.println(F("Program started..."));
}


void loop() {
  if(state == F("restoreFromSDCard"))
    restoreFromSDCard();
  else if(state == F("start"))
    start();
  else if(state == F("addCocktails"))
    addCocktails();
  else if(state == F("addIngredients"))
    addIngredients();
  else if(state == F("confirmGoingBack"))
    confirmGoingBack();
  else if(state == F("tankContentSet"))
    tankContentSet();
  else if(state == F("fillTanks"))
    fillTanks();
  else if(state == F("masterOverview"))
    masterOverview();
  else if(state == F("machineFilled"))
    machineFilled();
  else if(state == F("cocktailSelection"))
    cocktailSelection();
  else if(state == F("customCocktailMenu"))
    customCocktailMenu();
  else if(state == F("gameMenu"))
    gameMenu();
  else if(state == F("game"))
    game();
  else if(state == F("refillTanks"))
    refillTanks();
  else if(state == F("questionEmptyTanks"))
    questionEmptyTanks();
  else if(state == F("emptyTanksGlassSize"))
    emptyTanksGlassSize();
  else if(state == F("emptyTanksMenu"))
    emptyTanksMenu();
  else if(state == F("cleaningMenu"))
    cleaningMenu();
  else if(state == F("error"))
    error();
}


// MENU FUNCTIONS 
void start(){
  resetGlobalVariables();
  mylcd.Fill_Screen(BLACK);

  numOfChoices = 2;

  print(F("Waehle eine der folgenden Moeglich-"));
  print(F("keiten, die Maschine zu befuellen:"));
  addSpacingY(textChoiceSpacingY);
  printChoice(F("Cocktails auswaehlen"));
  printChoice(F("Zutaten auswaehlen"));
  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      if(focusedChoice == 0)
        state = F("addCocktails");
      else if(focusedChoice == 1)
        state = F("addIngredients");
        
      return;
    }
  }
}

void restoreFromSDCard(){
  // Check if tankContents were saved
  if(!tankContentsSaved()){
    state = F("start");
    return;
  }

  resetGlobalVariables();
  mylcd.Fill_Screen(BLACK);

  numOfChoices = 2;

  print(F("Es wurden Tankinhalte auf der SD Karte"));
  print(F("gespeichert. Daten wiederherstellen?"));
  addSpacingY(textChoiceSpacingY);
  printChoice(F("Ja, wiederherstellen"));
  printChoice(F("Nein"));
  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      if(focusedChoice == 0)
        downloadTankContentsFromSDCard(); // sets State
      else if(focusedChoice == 1){
        File tankContentsTxt = SD.open(F("tanks.txt"), FILE_WRITE | O_TRUNC);    // O_TRUNC deletes existing file content
        tankContentsTxt.close();
        state = F("start");
      }
        
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
        state = F("addIngredients");
        return;
      }

      // Cocktail clicked
      int index = focusedChoice - 1; // -1 because there is always a non Cocktail option at position 0
      for(int i = 0; i < currentPage; i++){
        index += numOfChoicesOnPage[i];
      }
      int currentIndex = 0;
      for(int i = 0; i < maxNumOfCocktails; i++){
        if(possibleCocktails[i] == false) continue;

        if(index == currentIndex) {
          confirmAddCocktail(cocktails[i]);
          
          return;
        }

        currentIndex++;
      }
    }

    if(isBackButtonPressed()){
      lastState = F("addCocktails");
      secondLastState = F("start");
      goingBackMessage = F("Ja, alle Tanks werden geloescht");

      state = F("confirmGoingBack");
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
        continue;
      }
      else if(focusedChoice == 0 && currentPage > 0){
        goToPreviousPage();
        printAddIngredientsMenu();
        continue;
      }
      else if(focusedChoice == 0 && currentPage == 0){
        state = F("addCocktails");
        return;
      }

      // Ingredient clicked
      int index = focusedChoice - 1; // -1 because there is always a non Ingredient option at position 0
      for(int i = 0; i < currentPage; i++){
        index += numOfChoicesOnPage[i];
      }
      int currentIndex = 0;
      for(int i = 0; i < maxNumOfIngredients; i++){
        if(ingredients[i] == "") break;
        if(chosenIngredients[i] == true) continue;

        if(index == currentIndex) {
          chosenIngredients[i] = true;
          cs.addContent(ingredients[i]);
          if(numOfEmptyTanks == 0)
            state = F("tankContentSet");
          
          return;
        }

        currentIndex++;
      }
    }

    if(isBackButtonPressed()){
      lastState = F("addIngredients");
      secondLastState = F("start");
      goingBackMessage = F("Ja, alle Tanks werden geloescht");
      state = F("confirmGoingBack");
      return;
    }
  }
}

void confirmGoingBack(){
  resetGlobalVariables();
  mylcd.Fill_Screen(BLACK);

  numOfChoices = 2;

  print(F("Wirklich zurueckgehen?"));
  addSpacingY(textChoiceSpacingY);
  printChoice(F("Abbrechen"));
  printChoice(goingBackMessage);

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      if(focusedChoice == 0){
        state = lastState;
      } else if(focusedChoice == 1){
        deleteTankContents();
        state = secondLastState;
      }

      return;
    }
  }
}

void tankContentSet(){
  mylcd.Fill_Screen(BLACK);
  resetGlobalVariables();

  // needed to get the available Cocktails
  updateAvailableCocktails();
  saveTankContentsToSD(state);

  print(F("Die Tankinhalte sind jetzt festgelegt"));
  print(F("und wurden auf der SD-Karte gespeichert"));
  print(F("Es koennen nun folgende Cocktails"));
  print(F("zubereitet werden:"));

  addSpacingY(10);
  printAvailableCocktails();
  addSpacingY(10);

  print(F("Die Maschine muss dafuer mit den"));
  print(F("folgenden Zutaten befuellt werden:"));

  addSpacingY(10);
  printTankContents();
  addSpacingY(textChoiceSpacingY);

  numOfChoices = 1;
  printChoice(F("Weiter zur Befuellung der Maschine"));
  
  
  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();
    
    if(isOkButtonPressed()){
      state = F("fillTanks");
      return;
    } else if(isBackButtonPressed()){
      lastState = F("tankContentSet");
      secondLastState = F("start");
      goingBackMessage = F("Ja, alle Tanks werden geloescht");

      state = F("confirmGoingBack");
      return;
    }
  }
}

void fillTanks(){
  String ingredient = cs.getTankContent(currentTankIndex);
  if(ingredient == ""){
    state = F("machineFilled");
    currentTankIndex = 0;
    return;
  }
  mylcd.Fill_Screen(BLACK);
  resetGlobalVariables();
  print(F("Fuelle den Tank direkt ueber der"));
  print("Ausgabeoeffnung mit " + ingredient + ".");
  print(F("Waehle die Markierung, bis zu welcher"));
  print(F("du den Tank gefuellt hast:"));
  numOfChoices = 5;
  addSpacingY(textChoiceSpacingY);
  printChoice(F("1"));
  printChoice(F("2"));
  printChoice(F("3"));
  printChoice(F("4"));
  printChoice(F("5"));

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      int fillingLevel = (focusedChoice + 1) * tankMarkToCl;
      cs.fillTankTo(currentTankIndex, fillingLevel);

      if(currentTankIndex == numOfTanks - 1){
        state = F("masterOverview");
        currentTankIndex = 0;
        return;
      }
      currentTankIndex++;
      return;
    } else if(isBackButtonPressed()){
      lastState = F("fillTanks");
      secondLastState = F("start");
      state = F("confirmGoingBack");
      return;
    }
  }
}

void masterOverview(){
  mylcd.Fill_Screen(BLACK);
  resetGlobalVariables();

  saveTankContentsToSD(state);

  print(F("Die Maschine ist jetzt bereit fuer"));
  print(F("die Party!"));
  print(F("Der jetzige Status ist auf der"));
  print(F("SD-Karte gespeichert."));
  addSpacingY(textChoiceSpacingY);

  print(F("Die Tanks sind nun wie folgt gefuellt:"));

  addSpacingY(textChoiceSpacingY);
  printTankContentsWithAmounts();

  numOfChoices = 1;
  addSpacingY(textChoiceSpacingY);
  printChoice(F("Zum Party-Modus"));

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      state = F("machineFilled");
      return;
    }
  }
}

void machineFilled(){
  mylcd.Fill_Screen(BLACK);
  resetGlobalVariables();

  initChosenIngredientsArr();
  clearCustomCocktail();

  initChosenIngredientsGame();

  print(F("Waehle eine der folgenden Optionen:"));

  numOfChoices = 4;
  addSpacingY(textChoiceSpacingY);
  printChoice(F("Cocktail auswaehlen"));
  printChoice(F("Persoenlicher Cocktail"));
  printChoice(F("Russian Roulette"));
  printChoice(F("Tanks nachfuellen"));

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      if(focusedChoice == 0)
        state = F("cocktailSelection");
      else if(focusedChoice == 1)
        state = F("customCocktailMenu");
      else if(focusedChoice == 2)
        state = F("gameMenu");
      else if(focusedChoice == 3)
        state = F("refillTanks");

      return;
    }
    else if(isBackButtonPressed()){
      state = F("questionEmptyTanks");
      return;
    }
  }
}

void cocktailSelection(){
  mylcd.Fill_Screen(BLACK);
  resetGlobalVariables();

  updateAvailableCocktails();

  print(F("Waehle einen der folgenden Cocktails:"));

  addSpacingY(textChoiceSpacingY);
  printChoice(F("Zurueck"));
  numOfChoices = 1;


  for(int i = 0; i < maxNumOfCocktails; i++){
    if(cocktails[i].getName() == "") break;
    if(!availableCocktails[i]) continue;

    numOfChoices++;
    printChoice(cocktails[i].getName());
  }

  // print tanks and separator lines
  printTankContentsWithAmountsBoxed();

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      if(focusedChoice == 0){
        state = F("machineFilled");
        return;
      }

      int index = focusedChoice - 1; // -1 because there is always a "Zurueck" option at position 0
      int currentIndex = 0;
      for(int i = 0; i < maxNumOfCocktails; i++){
        if(cocktails[i].getName() == "") break;
        if(!availableCocktails[i]) continue;

        if(index == currentIndex) {
          lastState = F("cocktailSelection");
          confirmMakeCocktail(cocktails[i]);
          
          return;
        }

        currentIndex++;
      }

    }
  }

}

void customCocktailMenu(){
  resetGlobalVariables();

  printCustomCocktailMenu();  // sets numOfChoices

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      if(focusedChoice == 0){
        if(numIngredsCustomCocktail > 0){
          lastState = F("customCocktailMenu");
          confirmMakeCocktail(customCocktail);
        }
        
        state = F("machineFilled");
        return;
      }

      // Ingredient clicked
      int index = focusedChoice - 1; // -1 because there is always a non Ingredient option at position 0

      int currentIndex = 0;
      for(int i = 0; i < numOfTanks; i++){
        if(cs.getTankContent(i) == "") break;
        if(chosenIngredients[i] == true) continue;

        if(index == currentIndex) {
          chosenIngredients[i] = true;
          addIngredientNameToCustomCocktail(cs.getTankContent(i));

          getCustomCocktailAmount();

          if(numIngredsCustomCocktail == numOfTanks || currentGlassCapacity == 0){
            lastState = F("customCocktailMenu");
            confirmMakeCocktail(customCocktail);
            state = F("machineFilled");
            return;
          }
          
          return;
        }

        currentIndex++;
      }
    }
  }
}

void gameMenu(){
  resetGlobalVariables();

  printGameMenu();

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      // "Abbrechen" option
      if(focusedChoice == 0){
        state = F("machineFilled");
        return;
      } // "Spiel starten" option
      else if(focusedChoice == numOfChoices - 1 && numOfIngredsGame > 0){
        state = F("game");
        return;
      }

      // Ingredient clicked
      int index = focusedChoice - 1; // -1 because there is always a non Ingredient option at position 0

      int currentIndex = 0;
      for(int i = 0; i < numOfTanks; i++){
        if(cs.getTankContent(i) == "") break;
        if(chosenIngredients[i] == true) continue;

        if(index == currentIndex) {
          chosenIngredients[i] = true;

          // add Ingredient to chosenIngredientsGame
          chosenIngredientsGame[numOfIngredsGame] = Ingredient(cs.getTankContent(i), shotSize);
          numOfIngredsGame++;

          if(numOfIngredsGame == numOfTanks)
            state = F("game");
    
          return;
        }

        currentIndex++;
      }
    }
  }
}

void game(){
  mylcd.Fill_Screen(BLACK);
  resetGlobalVariables();

  print(F("Russian Roulette"));
  addSpacingY(textChoiceSpacingY);
  print(F("Traust du dich, den Abzug zu druecken?"));

  numOfChoices = 2;
  addSpacingY(textChoiceSpacingY);
  printChoice(F("Abzug druecken"));
  printChoice(F("Spiel abbrechen"));

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      if(focusedChoice == 1){
        state = F("machineFilled");
        return;
      }

      // Trigger pressed
      printGameResult();
      return;
    }
  }
}

void refillTanks(){
  mylcd.Fill_Screen(BLACK);
  resetGlobalVariables();

  print(F("Welcher Tank soll nachgefuellt werden?"));

  addSpacingY(textChoiceSpacingY);
  printChoice(F("Zurueck"));
  numOfChoices = 1;
  for(int i = 0; i < numOfTanks; i++){
    String tankContent = cs.getTankContent(i);
    int tankFillingLevel = cs.getTankFillingLevel(i);
    if(tankContent == "" || tankFillingLevel == 50) continue;

    printChoice(tankContent);
    numOfChoices++;
  }

  printTankContentsWithAmountsBoxed();

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      if(focusedChoice == 0)
        state = F("machineFilled");

      int tankIndex = focusedChoice - 1;    // because of "Zurueck" Option
      int currentIndex = 0;
      for(int i = 0; i < numOfTanks; i++){
        String tankContent = cs.getTankContent(i);
        int tankFillingLevel = cs.getTankFillingLevel(i);
        if(tankContent == "" || tankFillingLevel == 50) continue;

        if(tankIndex != currentIndex){
          currentIndex++;
          continue;
        }
        
        refillTank(i);
        return;
      }

      return;
    }
  }
}

void questionEmptyTanks(){
  mylcd.Fill_Screen(BLACK);
  resetGlobalVariables();

  print(F("Tanks leeren?"));
  addSpacingY(textChoiceSpacingY);
  print(F("Sollen alle Tanks geleert werden?"));
  addSpacingY(textChoiceSpacingY);

  numOfChoices = 2;
  printChoice(F("Ja, alle Tanks leeren"));
  printChoice(F("Abbrechen"));
  
  printTankContentsWithAmountsBoxed();

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      if(focusedChoice == 0)
        state = F("emptyTanksGlassSize");
      else if(focusedChoice == 1)
        state = F("machineFilled");

      return;
    }
  }
}

void emptyTanksGlassSize(){
  mylcd.Fill_Screen(BLACK);
  resetGlobalVariables();

  print(F("Tanks leeren"));
  addSpacingY(textChoiceSpacingY);
  print(F("Wie viel ml passen in das Glas,"));
  print(F("dass zum entleeren benutzt wird?"));

  addSpacingY(textChoiceSpacingY);
  numOfChoices = 6;
  printChoice(F("Abbrechen"));
  printChoice(F("100ml"));   // 1
  printChoice(F("150ml"));   // 2
  printChoice(F("200ml"));   // 3
  printChoice(F("250ml"));   // 4
  printChoice(F("300ml"));   // 5

  printTankContentsWithAmountsBoxed();

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      if(focusedChoice == 0){
        state = F("machineFilled");
        return;
      }

      currentGlassCapacity = 10 + (focusedChoice - 1) * 5;
      state = F("emptyTanksMenu");

      return;
    }
  }
}

void emptyTanksMenu(){
  for(int i = 0; i < numOfTanks; i++){
    emptyTankMenu(i);
  }
  deleteTANKS_txt();
  cs.emptyTanks();

  mylcd.Fill_Screen(BLACK);
  resetGlobalVariables();

  print(F("Alle Tanks wurden geleert"));
  addSpacingY(textChoiceSpacingY);
  print(F("Moechtest du nun:"));

  addSpacingY(textChoiceSpacingY);
  numOfChoices = 2;
  printChoice(F("Zum Reiningungsmodus"));
  printChoice(F("Zum Hauptmenue zurueckgehen"));

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();
    if(isOkButtonPressed()){
      if(focusedChoice == 0)
        state = F("cleaningMenu");
      else if(focusedChoice == 1)
        state = F("start");
      
      return;
    }
  }
}

void cleaningMenu(){
  fillTanksWithCleaningFluid();
  emptyTanksWithCleaningFluid();

  mylcd.Fill_Screen(BLACK);
  resetGlobalVariables();

  print(F("Alle Tanks wurden gereinigt"));
  addSpacingY(textChoiceSpacingY);
  print(F("Du kannst nun die Maschine ausschalten"));
  print(F("oder zum Hauptmenue zurueckgehen"));
  addSpacingY(textChoiceSpacingY);
  numOfChoices = 1;
  printChoice(F("Zum Hauptmenue zurueckgehen"));

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();
    if(isOkButtonPressed()){
        state = F("start");
      
      return;
    }
  }
}

void error(){
  resetGlobalVariables();
  mylcd.Fill_Screen(BLACK);

  print(F("ERROR"));
  while(true){

  }
}


// INTERRUPT FUNCTIONS
void okButtonPress(){
  // check if enough time has passed and if rotation occured
  if((millis() - timeOfLastDebouncePress) > delayOfDebouncePress){
      timeOfLastDebouncePress = millis();

      inputState = 3;
      Serial.println(F("OK-Button: pressed"));
  }
  
}

void backButtonPress(){
  inputState = 4;
  Serial.println(F("Back-Button: pressed"));
}

void knobTurn(){
  // check if enough time has passed and if rotation occured
  if((millis() - timeOfLastDebounceTurn) > delayOfDebounceTurn){
      timeOfLastDebounceTurn = millis();

      int clkState = digitalRead(pinCLK);
      int dtState = digitalRead(pinDT);

      if(dtState != clkState){          
          inputState = 2;
          Serial.println(F("Knob: positive rotation"));
      } else if(dtState == clkState){
          inputState = 1;     
          Serial.println(F("Knob: negative rotation"));
      }
    }
}


// MENU PRINT FUNCTIONS
void fillTanksWithCleaningFluid(){
  // HWCMD: MOVE1
  currentTankIndex = 0;
  for(byte i = 0; i < numOfTanks; i++){
    mylcd.Fill_Screen(BLACK);
    resetGlobalVariables();

    Serial.print(F("Current Tank Index: "));
    Serial.println(currentTankIndex);

    print(F("Fuellen der Tanks"));
    addSpacingY(textChoiceSpacingY);
    print(F("Fuelle nun Reinigungsfluessigkeit"));
    print(F("in den Tank ueber dem Glas"));
    addSpacingY(textChoiceSpacingY);

    numOfChoices = 1;
    printChoice(F("Weiter"));

    while(true){
      focusedChoice = getTurnKnobIndex();
      focus();

      if(isOkButtonPressed()){
        if(currentTankIndex != numOfTanks - 1)
          currentTankIndex++;
        // HWCMD: MOVE
        break;
      }
    }
  }
}

void emptyTanksWithCleaningFluid(){
  for(byte i = 0; i < numOfTanks; i++){
    mylcd.Fill_Screen(BLACK);
    resetGlobalVariables();

    Serial.print(F("Current Tank Index: "));
    Serial.println(currentTankIndex);

    print(F("Leeren der Tanks"));
    addSpacingY(textChoiceSpacingY);
    print(F("Stelle nun ein Glas in die Maschine"));
    addSpacingY(textChoiceSpacingY);

    numOfChoices = 1;
    printChoice(F("Tank entleeren"));

    while(true){
      focusedChoice = getTurnKnobIndex();
      focus();

      if(isOkButtonPressed()){
        // HWCMD: EMPTY 10cl
        break;
      }
    }

    tankEmptyQuestionCleaning();

    // HWCMD MOVE
    if(currentTankIndex > 0)
      currentTankIndex--;
  }
}

void tankEmptyQuestionCleaning(){
  while(true){
    mylcd.Fill_Screen(BLACK);
    resetGlobalVariables();

    print(F("Ist der Tank nun leer?"));
    addSpacingY(textChoiceSpacingY);
    numOfChoices = 2;
    printChoice(F("Ja"));
    printChoice(F("Nein, Tank weiter leeren"));

    while(true){
      focusedChoice = getTurnKnobIndex();
      focus();

      if(isOkButtonPressed()){
        if(focusedChoice == 0) return;

        // HWCMD: EMPTY 10cl
        break;
      }
    }
  }
}

void emptyTankMenu(int tankIndex){
  String tankContent = cs.getTankContent(tankIndex);
  if(tankContent == "") return;
    
  // empty Tank while not empty
  while(cs.getTankFillingLevel(tankIndex) > 0){
    mylcd.Fill_Screen(BLACK);
    resetGlobalVariables();

    print(tankContent + F(" Tank leeren:"));
    addSpacingY(textChoiceSpacingY);

    numOfChoices = 1;
    printChoice(F("Weiter, Glas leer!"));

    //print("Achte darauf, dass ein Glas mit der");
    //print("angegebenen Kapazitaet in der Machine ist!");

    printTankContentsWithAmountsBoxed();

    while(true){
      focusedChoice = getTurnKnobIndex();
      focus();

      if(isOkButtonPressed()){
        // one extra 2cl dispense
        cs.withdraw(tankIndex, min(cs.getTankFillingLevel(tankIndex) + 2, currentGlassCapacity));
        break;
      }
    }
  }
}

void getCustomCocktailAmount(){
  resetGlobalVariables();

  printGetCustomCocktailAmount();

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      // last Choice & next page option exists
      if(focusedChoice == numOfChoices - 1 && currentPage < numOfPages - 1){
        goToNextPage();
        printGetCustomCocktailAmount();
        continue;
      }
      else if(focusedChoice == 0 && currentPage > 0){
        goToPreviousPage();
        printGetCustomCocktailAmount();
        continue;
      }

      // Amount clicked
      int index = focusedChoice + 1; 
      if(currentPage != 0)
        index--;    // prev. Page option

      for(int i = 0; i < currentPage; i++){
        index += numOfChoicesOnPage[i];
      }
      
      // index is amount
      customCocktail.setIngredientAmount(numIngredsCustomCocktail - 1, index * 2);
      currentGlassCapacity -= index * 2;
      return;
    }
  }
}

void printGetCustomCocktailAmount(){
  mylcd.Fill_Screen(BLACK);

  if(currentPage == 0){
    String currentIngredientName = customCocktail.getIngredient(numIngredsCustomCocktail - 1).getName();
    print("Wie viele cl von " + currentIngredientName);
    print(F("moechtest du in deinem Cocktail?"));
    addSpacingY(textChoiceSpacingY);
  }

  for(int i = 2; i <= min(maxAmountPerIngredient, currentGlassCapacity); i += 2){
    printChoiceToPages(String(i));
  }

  // print customCocktail and separator lines
  printCustomCocktailBoxed();
}

void printCustomCocktailMenu(){
  mylcd.Fill_Screen(BLACK);

  print(F("Waehle eine Zutat fuer deinen"));
  print(F("persoenlichen Cocktail:"));
  addSpacingY(textChoiceSpacingY);
  if(numIngredsCustomCocktail > 0)
    printChoice(F("Cocktail zubereiten"));
  else
    printChoice(F("Abbrechen"));
  
  numOfChoices = 1;

  for(int i = 0; i < numOfTanks; i++){
    if(cs.getTankContent(i) == "") break;
    if(chosenIngredients[i] == true) continue;

    printChoice(cs.getTankContent(i));
    numOfChoices++;
  }

  // print customCocktail and separator lines
  printCustomCocktailBoxed();
}

void printGameMenu(){
  mylcd.Fill_Screen(BLACK);

  print(F("Willkommen zu Russian Roulette!"));
  addSpacingY(textChoiceSpacingY);
  print(F("Waehle zuerst Zutaten fuer die Shots:"));
  addSpacingY(textChoiceSpacingY);

  numOfChoices = 1;
  printChoice(F("Abbrechen"));

  printChosenIngredsGameBoxed();

  for(int i = 0; i < numOfTanks; i++){
    if(cs.getTankContent(i) == "") break;
    if(chosenIngredients[i] == true) continue;

    printChoice(cs.getTankContent(i));
    numOfChoices++;
  }

  if(numOfIngredsGame > 0){
    printChoice(F("Spiel starten"));
    numOfChoices++;
  }
}

void printGameResult(){
  resetGlobalVariables();
  mylcd.Fill_Screen(BLACK);

  int randomNum = random(maxRandomNum);
  if(randomNum == 0)
    printShotReceived();
  else
    printNoShotReceived();
}

void printShotReceived(){
  print(F("Russian Roulette"));
  addSpacingY(textChoiceSpacingY);

  int randomNum = random(numOfIngredsGame);
  Ingredient shotIngredient = chosenIngredientsGame[randomNum];
  print(F("PENG!"));
  print("Pech gehabt, du bekommst einen Shot:");
  addSpacingY(textChoiceSpacingY);
  print(shotIngredient.getName());

  addSpacingY(textChoiceSpacingY);
  print(F("Achte darauf, dass sich ein Glas"));
  print(F("in der Maschine befindet"));
  addSpacingY(textChoiceSpacingY);

  addSpacingY(textChoiceSpacingY);
  numOfChoices = 1;
  printChoice(F("Shot zubereiten"));

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      makeShot(shotIngredient);
      state = F("game");
      return;
    }
  }
}

void printNoShotReceived(){
  print(F("Russian Roulette"));
  addSpacingY(textChoiceSpacingY);
  print(F("Click... Glueck gehabt!"));

  addSpacingY(textChoiceSpacingY);
  numOfChoices = 1;
  printChoice(F("Weiter"));

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      state = F("game");
      return;
    }
  }
}

void notEnoughContentMenu(String ingredientName){
  mylcd.Fill_Screen(BLACK);
  resetGlobalVariables();

  print("Nicht genug " + ingredientName);
  print("Bitte " + ingredientName + " nachfuellen");
  
  addSpacingY(textChoiceSpacingY);
  numOfChoices = 2;
  printChoice(ingredientName + F(" Tank auffuellen"));
  printChoice(F("Anderen Cocktail waehlen"));

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      if(focusedChoice == 0){
        int tankIndex;
        for(int i = 0; i < numOfTanks; i++){
          if(cs.getTankContent(i) != ingredientName) continue;

          tankIndex = i;
          break;
        }

        refillTank(tankIndex);
      }
      else if(focusedChoice == 1)
        state = lastState;

      return;
    }
  }
}

void notEnoughContentGame(String ingredientName){
  mylcd.Fill_Screen(BLACK);
  resetGlobalVariables();

  print("Nicht genug " + ingredientName);
  print("Bitte " + ingredientName + " nachfuellen");
  
  addSpacingY(textChoiceSpacingY);
  numOfChoices = 2;
  printChoice(ingredientName + " Tank auffuellen");
  printChoice(F("Zum Hauptmenue"));

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      if(focusedChoice == 0){
        int tankIndex;
        for(int i = 0; i < numOfTanks; i++){
          if(cs.getTankContent(i) != ingredientName) continue;

          tankIndex = i;
          break;
        }

        refillTank(tankIndex);
      }
      else if(focusedChoice == 1)
        state = F("machineFilled");

      return;
    }
  }
}

void printAddCocktailsMenu(){
  mylcd.Fill_Screen(BLACK);
  updatePossibleCocktails();

  if(currentPage == 0){
    print(F("Waehle eine der folgenden Cocktails aus:"));
    addSpacingY(textChoiceSpacingY);
    printChoice(F("Zutaten hinzufuegen"));
  }

  for(int i = 0; i < maxNumOfCocktails; i++){
    if(cocktails[i].getName() == "" ) break;
    if(possibleCocktails[i] == false) continue;
    printChoiceToPages(cocktails[i].getName());
  }

  // print tanks and separator lines
  printTanks();

  if(currentPage == 0)
    numOfChoices++; // additional "Zutaten hinzufügen" option
}

void confirmAddCocktail(Cocktail cocktail){
  mylcd.Fill_Screen(BLACK);
  resetGlobalVariables();

  print(cocktail.getName() + F(" hinzufuegen?"));
  addSpacingY(textChoiceSpacingY);
  // Maybe insert short info about the cocktail here
  print(F("Enthaltene Zutaten:"));

  for(int i = 0; i < numOfTanks; i++){
    Ingredient ingredient = cocktail.getIngredient(i);
    if(ingredient.getName() == "") break;
    
    print(ingredient.getName() + ": " + ingredient.getAmount() + "cl");
  }

  addSpacingY(textChoiceSpacingY);
  numOfChoices = 2;
  printChoice(F("Ja, Cocktail hinzufuegen"));
  printChoice(F("Abbrechen"));

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      if(focusedChoice == 0){
        addCocktail(cocktail);
        updatePossibleCocktails();
        if(numOfEmptyTanks == 0 || !arePossibleCocktails())
          state = F("tankContentSet");
      }
      
      return;
    }
  }
}

void confirmMakeCocktail(Cocktail cocktail){
  resetGlobalVariables();
  mylcd.Fill_Screen(BLACK);

  print(cocktail.getName() + F(" zubereiten?"));
  addSpacingY(textChoiceSpacingY);
  print(F("Achte darauf, dass ein Glas"));
  print(F("in der Maschine steht!"));
  addSpacingY(textChoiceSpacingY);

  print(F("Enthaltene Zutaten:"));

  for(int i = 0; i < numOfTanks; i++){
    Ingredient ingredient = cocktail.getIngredient(i);
    if(ingredient.getName() == "") break;
    
    print(ingredient.getName() + ": " + ingredient.getAmount() + F("cl"));
  }

  addSpacingY(textChoiceSpacingY);
  numOfChoices = 2;
  printChoice(F("Ja, Cocktail zubereiten"));
  printChoice(F("Abbrechen"));

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      if(focusedChoice == 0){
        makeCocktail(cocktail);
        return;
      }
      

      return;
    }
  }
}

void printAddIngredientsMenu(){
  mylcd.Fill_Screen(BLACK);

  if(currentPage == 0){
    print(F("Waehle eine der folgenden Zutaten aus:"));
    addSpacingY(textChoiceSpacingY);
    printChoice(F("Cocktails hinzufuegen"));
  }

  for(int i = 0; i < maxNumOfIngredients; i++){
    if(ingredients[i] == "") break;
    if(chosenIngredients[i] == true) continue;
    printChoiceToPages(ingredients[i]);
  }

  // print tanks and separator lines
  printTanks();

  if(currentPage == 0)
    numOfChoices++; // additional "Cocktails hinzufuegen" option
}

// .length returns correctNumOfChars + 1
void printAvailableCocktails(){
  // at most 10 available Cocktails possible
  int maxNumOfAvailableCocktails = 10;
  String availableCocktailsArr[maxNumOfAvailableCocktails];
  int currentIndex = 0;

  for(int i = 0; i < maxNumOfCocktails; i++){
    String cocktailName = cocktails[i].getName();
    if(cocktailName == "") break;
    if(!availableCocktails[i]) continue;

    availableCocktailsArr[currentIndex] = cocktailName;
    currentIndex++;
  }

  // print availableCocktailsArr
  int currentStringLength = availableCocktailsArr[0].length() - 1;
  String availableCocktailsStr = availableCocktailsArr[0];
  for(int i = 1; i < currentIndex; i++){
    int addedLength = 1 + availableCocktailsArr[i].length();  // 2 for ", " and -1 for length: "123".length() => 4
    if(currentStringLength + addedLength <= maxNumOfCharsPerLine){
      currentStringLength += addedLength;

      availableCocktailsStr += ", " + availableCocktailsArr[i];
    } else{
      // print a new line with these Cocktails
      print(availableCocktailsStr + ",");

      currentStringLength = addedLength - 2;
      availableCocktailsStr = availableCocktailsArr[i];
    }
  }

  // print not yet printed Cocktails
  print(availableCocktailsStr);
}

void cocktailDoneMenu(Cocktail cocktail){
  mylcd.Fill_Screen(BLACK);
  resetGlobalVariables();

  print(F("Der Cocktail ist nun zubereitet"));
  addSpacingY(textChoiceSpacingY);
  print(F("Du kannst ihn jetzt aus der"));
  print(F("Maschine nehmen"));

  addSpacingY(textChoiceSpacingY);
  numOfChoices = 1;
  printChoice(F("Weiter"));

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      state = F("cocktailSelection");
      return;
    }
  }
}

void shotDoneMenu(Ingredient shotIngredient){
  mylcd.Fill_Screen(BLACK);
  resetGlobalVariables();

  print(F("Der Shot ist nun zubereitet"));
  addSpacingY(textChoiceSpacingY);
  print(F("Du kannst ihn jetzt aus der"));
  print(F("Maschine nehmen"));

  addSpacingY(textChoiceSpacingY);
  numOfChoices = 1;
  printChoice(F("Weiter"));

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      state = "game";
      return;
    }
  }
}

void refillTank(int tankIndex){
  mylcd.Fill_Screen(BLACK);
  resetGlobalVariables();

  String tankContent = cs.getTankContent(tankIndex);
  int tankFillingLevel = cs.getTankFillingLevel(tankIndex);

  print(F("Fuelle den Tank direkt ueber dem Glas"));
  print("mit " + tankContent + ".");
  addSpacingY(textChoiceSpacingY);

  print("Aktueller Fuellstand: " + String(tankFillingLevel) + "cl");

  addSpacingY(textChoiceSpacingY);
  print(F("Waehle die Markierung, bis zu welcher"));
  print(F("du den Tank gefuellt hast:"));
  addSpacingY(textChoiceSpacingY);

  numOfChoices = 0;
  for(int i = 1; i <= 5; i++){
    if(tankFillingLevel >= i * tankMarkToCl) continue;

    printChoice(String(i));
    numOfChoices++;
  }

  while(true){
    focusedChoice = getTurnKnobIndex();
    focus();

    if(isOkButtonPressed()){
      int beginningOfFillingLevels = 6 - numOfChoices;
      int fillingLevel = (focusedChoice + beginningOfFillingLevels) * tankMarkToCl;
      cs.fillTankTo(tankIndex, fillingLevel);

      return;
    }
  }
}

// prints separator lines and Tanks vertically
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

// prints tanks horizontally for tankContentSet-Menu; .length returns correct numOfChars
void printTankContents(){
  int currentStringLength = cs.getTankContent(0).length();
  String tankContentsStr = cs.getTankContent(0);
  for(int i = 1; i < numOfTanks; i++){
    String newIngredientStr = cs.getTankContent(i);
    if(newIngredientStr == "") break;

    int addedLength = 2 + newIngredientStr.length();  // 2 for ", "
    if(currentStringLength + addedLength <= maxNumOfCharsPerLine){
      currentStringLength += addedLength;

      Serial.print(newIngredientStr);
      Serial.print(F(" : length: "));
      Serial.print(addedLength);
      Serial.print(F(" => "));
      Serial.println(currentStringLength);

      tankContentsStr += ", " + newIngredientStr;
    } else{
      // print a new line with these Ingredients
      print(tankContentsStr + ",");

      currentStringLength = addedLength - 2;
      tankContentsStr = cs.getTankContent(i);
    }
  }

  // print not yet printed Ingredients
  print(tankContentsStr);
}

void printTankContentsWithAmounts(){
  for(int i = 0; i < numOfTanks; i++){
    String tankContent = cs.getTankContent(i) + ": ";
    String tankAmountStr = String(cs.getTankFillingLevel(i)) + "cl";
    if(cs.getTankContent(i) == "")
      tankContent = F("leer");

    mylcd.Print_String(tankAmountStr, 250, textPosY);
    print(tankContent);
  }
}

void printTankContentsWithAmountsBoxed(){
  // print lines
  mylcd.Set_Draw_color(WHITE);
  mylcd.Draw_Fast_HLine(separatorLineWideX, seperatorLineWideY, 400);
  mylcd.Draw_Fast_VLine(separatorLineWideX, seperatorLineWideY, 300);

  int textPosXTanks = textPaddingX + separatorLineWideX;
  int textPosYTanks = textPaddingY + seperatorLineWideY;
  mylcd.Print_String(F("Tanks:"), textPosXTanks , textPosYTanks);
  for(int i = 0; i < numOfTanks; i++){
    textPosYTanks += textSpacingY;
    String tankContent = cs.getTankContent(i) == "" ? "leer" : cs.getTankContent(i);
    tankContent += ":";
    String tankFillingLevelStr = String(cs.getTankFillingLevel(i)) + "cl";

    mylcd.Print_String(tankContent, textPosXTanks , textPosYTanks);
    mylcd.Print_String(tankFillingLevelStr, 430 , textPosYTanks);
  }
}

void printCustomCocktailBoxed(){
  // print lines
  mylcd.Set_Draw_color(WHITE);
  mylcd.Draw_Fast_HLine(separatorLineWideX, seperatorLineWideY, 400);
  mylcd.Draw_Fast_VLine(separatorLineWideX, seperatorLineWideY, 300);

  int textPosXTanks = textPaddingX + separatorLineWideX;
  int textPosYTanks = textPaddingY + seperatorLineWideY;
  mylcd.Print_String(F("Zutaten:"), textPosXTanks , textPosYTanks);
  for(int i = 0; i < numIngredsCustomCocktail; i++){
    textPosYTanks += textSpacingY;
    Ingredient ingredient = customCocktail.getIngredient(i);

    String amountStr = String(ingredient.getAmount()) + "cl";

    mylcd.Print_String(ingredient.getName(), textPosXTanks , textPosYTanks);
    mylcd.Print_String(amountStr, 430 , textPosYTanks);
  }
}

void printChosenIngredsGameBoxed(){
   // print lines
  mylcd.Set_Draw_color(WHITE);
  mylcd.Draw_Fast_HLine(separatorLineWideX, seperatorLineWideY, 400);
  mylcd.Draw_Fast_VLine(separatorLineWideX, seperatorLineWideY, 300);

  int textPosXTanks = textPaddingX + separatorLineWideX;
  int textPosYTanks = textPaddingY + seperatorLineWideY;
  mylcd.Print_String(F("Shots:"), textPosXTanks , textPosYTanks);
  for(int i = 0; i < numOfIngredsGame; i++){
    textPosYTanks += textSpacingY;
    Ingredient ingredient = chosenIngredientsGame[i];

    String amountStr = String(ingredient.getAmount()) + "cl";

    mylcd.Print_String(ingredient.getName(), textPosXTanks , textPosYTanks);
    mylcd.Print_String(amountStr, 430 , textPosYTanks);
  }
}


// HELPER FUCTIONS
void addIngredientNameToCustomCocktail(String ingredientName){
  Serial.println(F("addIngredient to customCocktail: "));
  Serial.println(ingredientName);

  customCocktail.setIngredientName(numIngredsCustomCocktail, ingredientName);

  customCocktail.printToConsole();

  numIngredsCustomCocktail++;
}

void clearCustomCocktail(){
  Ingredient ingredients[numOfTanks];
  for(int i = 0; i < numOfTanks; i++){
    ingredients[i] = Ingredient();
  }

  customCocktail = Cocktail(F("Persoenlicher Cocktail"), ingredients, false);
  numIngredsCustomCocktail = 0;
  currentGlassCapacity = maxGlassCapacity;
}

void makeCocktail(Cocktail cocktail){
  // COCKTAIL POSSIBLE ?
  // loop through ingredients
  for(int i = 0; i < numOfTanks; i++){
    Ingredient ingredient = cocktail.getIngredient(i);
    if(ingredient.getName() == "") break;
    // loop through tanks
    for(int j = 0;  j < numOfTanks; j++){
      if(cs.getTankContent(j) != ingredient.getName()) continue;

      int tankFilllingLevel = cs.getTankFillingLevel(j);
      // not enough content to make Cocktail
      if(tankFilllingLevel < ingredient.getAmount()){
        notEnoughContentMenu(ingredient.getName());
        return;
      }
    }
  }

  // MAKE COCKTAIL
  // loop through ingredients
  for(int i = 0; i < numOfTanks; i++){
    Ingredient ingredient = cocktail.getIngredient(i);
    if(ingredient.getName() == "") break;
    // loop through tanks
    for(int j = 0;  j < numOfTanks; j++){
      if(cs.getTankContent(j) != ingredient.getName()) continue;

      cs.withdraw(j, ingredient.getAmount());
      // MOVE MACHINE
    }
  }

  // Cocktail done
  cocktailDoneMenu(cocktail);
}

void makeShot(Ingredient shotIngredient){
  // SHOT POSSIBLE ?
  // loop through tanks
  for(int i = 0;  i < numOfTanks; i++){
    if(cs.getTankContent(i) != shotIngredient.getName()) continue;

    int tankFilllingLevel = cs.getTankFillingLevel(i);
    // not enough content to make Cocktail
    if(tankFilllingLevel < shotIngredient.getAmount()){
      notEnoughContentGame(shotIngredient.getName());
      return;
    }
  }
  

  // MAKE SHOT
  // loop through tanks
  for(int j = 0;  j < numOfTanks; j++){
    if(cs.getTankContent(j) != shotIngredient.getName()) continue;
    cs.withdraw(j, shotIngredient.getAmount());
    // MOVE MACHINE
  }
  
  // Shot done
  shotDoneMenu(shotIngredient);
}

void deleteTankContents(){
  cs.emptyTanks();
  initChosenIngredientsArr();
  initPossibleCocktails();
  initAvailableCocktails();
}

void initChosenIngredientsArr(){
  for(int i = 0; i < maxNumOfIngredients; i++){
    chosenIngredients[i] = false;
  }
}

void initChosenIngredientsGame(){
  numOfIngredsGame = 0;
  for(int i = 0; i < numOfTanks; i++){
    chosenIngredientsGame[i] = Ingredient();
  }
}

void initPossibleCocktails(){
  for(int i = 0; i < maxNumOfCocktails; i++){
    possibleCocktails[i] = true;
  }
}

void initAvailableCocktails(){
  for(int i = 0; i < maxNumOfCocktails; i++){
    availableCocktails[i] = false;
  }
}

void updatePossibleCocktails(){
  for(int i = 0; i < maxNumOfCocktails; i++){
    if(cocktails[i].getName() == "") break;

    int newIngredients = 0;
    for(int j = 0; j < numOfTanks; j++){
      String ingredient = cocktails[i].getIngredient(j).getName();
      if(ingredient == "") break;
      bool isInTanks = false;
      for(int k = 0; k < numOfTanks; k++){
        if(ingredient != cs.getTankContent(k)) continue;
        isInTanks = true;
        break;
      }
      newIngredients += !isInTanks;
    }

    // already available or not possible
    if(newIngredients == 0 || newIngredients > numOfEmptyTanks){
      possibleCocktails[i] = false;
      continue;
    }
    
    possibleCocktails[i] = true;
  }
}

void updateAvailableCocktails(){
  for(int i = 0; i < maxNumOfCocktails; i++){
    if(cocktails[i].getName() == "") break;

    bool cocktailAvailable = true;
    for(int j = 0; j < numOfTanks; j++){
      String ingredient = cocktails[i].getIngredient(j).getName();
      if(ingredient == "") break;
      bool isInTanks = false;
      for(int k = 0; k < numOfTanks; k++){
        if(ingredient != cs.getTankContent(k)) continue;
        isInTanks = true;
        break;
      }
      if(!isInTanks){
        cocktailAvailable = false;
        break;
      }
    }

    availableCocktails[i] = cocktailAvailable;
  }
}

bool arePossibleCocktails(){
  for(int i = 0; i < maxNumOfCocktails; i++){
    if(cocktails[i].getName() == "") return false;
    if(possibleCocktails[i] == true) return true;
  }
  return false;
}

void addCocktail(Cocktail cocktail){
  // mark every ingredient in the chosen Coktail as chosen
  for(int i = 0; i < numOfTanks; i++){
    String ingredient = cocktail.getIngredient(i).getName();
    if(ingredient == "") return;
    for(int j = 0; j < maxNumOfIngredients; j++){
      if(ingredients[j] == "") break;
      if(ingredients[j] != ingredient) continue;
      // ingredient was chosen before
      if(chosenIngredients[j] == true) break;

      chosenIngredients[j] = true;
      cs.addContent(ingredient);
    }
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

  textPosY += choiceSpacingY;
}

void printChoiceToPages(String message){
  // print option to go to previous page
  if(currentPage != 0 && !previousPageOptionPrinted){
    printChoice(F("Vorherige Seite"));
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

    printChoice(F("Naechste Seite"));
    numOfPages++;
    numOfChoices = currentPage == 0 ? numOfChoicesOnPage[currentPage] + 1 : numOfChoicesOnPage[currentPage] + 2;  // one more choice: additional prev. Page Option + next Page option
    nextPageOptionPrinted = true;
    return;
  }

  printChoice(message);
  numOfChoicesOnPage[currentPage]++;
  numOfChoices = currentPage == 0 ? numOfChoicesOnPage[currentPage] : numOfChoicesOnPage[currentPage] + 1;  // one more choice: additional prev. Page Option
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
  if (counter < numOfChoices-1 && inputState == 2){
    counter++;
    Serial.println(counter);
    inputState = 0;
  }
  else if (counter > 0 && inputState == 1){
    counter --;
    Serial.println(counter);
    inputState = 0;
  }

  return counter;
}

bool isOkButtonPressed(){
  // check if Ok-Button is pressed
  if(inputState == 3){
    inputState = 0;
    return true;
  }
  return false;
}

bool isBackButtonPressed(){
  // check if Back-Button is pressed, maybe implement a short block time after the button was pressed
  if(inputState == 4){
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
bool tankContentsSaved(){
  File tankContentsTxt = SD.open(F("tanks.txt"));
  if(!tankContentsTxt){
    // if the file didn't open, print an error:
    Serial.println(F("error opening tanks.txt"));
    state = "error";
    return;
  }

  Serial.println(F("Check if tank contents were saved"));

  bool tankContentsSaved = tankContentsTxt.available();
  tankContentsTxt.close();

  return tankContentsSaved;
}

bool initializeSDCard(){
  Serial.print(F("Initializing SD card..."));
  if (!SD.begin(53)) {
    Serial.println(F("initialization failed!"));
    state = F("error");
    return false;
  }
  Serial.println(F("initialization done."));
  return true;
}

// Format: name, ingredient1, amount1, ingredient2, amount2, ..., ingredientn, amountn,;
void downloadCocktailsFromSD(){
  File cocktailTxt = SD.open(F("cocktail.txt"));
  if(!cocktailTxt){
    // if the file didn't open, print an error:
    Serial.println(F("error opening cocktail.txt"));
    state = F("error");
    return;
  }
  
  Serial.println(F("downloading Cocktails"));

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

      byte amount = amountStr.toInt() % 2 == 1 ? amountStr.toInt() + 1 : amountStr.toInt();

      // Save Ingredient
      ingredients[j] = Ingredient(nameOfIngredient, amount);
      if(j != numOfTanks - 1)
        ingredients[j + 1] = Ingredient();
    }

    // check if cocktail should be mixed
    String mixStr = "";
    while (cocktailTxt.available()){
      char currentChar = cocktailTxt.read();
      if(currentChar == ';') break;
      mixStr += currentChar;
    }
    bool mix = (mixStr == F("Ja"));

    // Save Cocktail
    cocktails[i] = Cocktail(name, ingredients, mix);
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
  File ingredientsTxt = SD.open(F("ingred.txt"));
  if(!ingredientsTxt){
    // if the file didn't open, print an error:
    Serial.println(F("error opening ingred.txt"));
    state = F("error");
    return;
  }
  
  Serial.println(F("downloading Ingredients"));

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

void downloadTankContentsFromSDCard(){
  File tankContentsTxt = SD.open(F("tanks.txt"));
  if(!tankContentsTxt){
    // if the file didn't open, print an error:
    Serial.println(F("error opening tanks.txt"));
    state = F("error");
    return;
  }

  cs.emptyTanks();

  for(int i = 0; i < numOfTanks; i++){
    String ingredientName = "";
    while (tankContentsTxt.available()) {
      char currentChar = tankContentsTxt.read();
      if(currentChar == ',') break;
      if(currentChar != '\n')
        ingredientName += currentChar;
    }

    String fillingLevelStr = "";
    while (tankContentsTxt.available()) {
      char currentChar = tankContentsTxt.read();
      if(currentChar == ',') break;
      if(currentChar != '\n')
        fillingLevelStr += currentChar;
    }

    // Save TanksContents
    if(ingredientName == F("leer"))
      ingredientName = "";
    
    cs.addContent(ingredientName);
    cs.fillTankToSetup(i, fillingLevelStr.toInt());
  }

  String newState = "";
  while (tankContentsTxt.available()) {
    char currentChar = tankContentsTxt.read();
    if(currentChar == ',') break;
    if(currentChar != '\n')
      newState += currentChar;
  }

  tankContentsTxt.close();
  state = newState;
}


void saveIngredientsFromCocktailsToSD(){
  File ingredientsTxt = SD.open(F("ingred.txt"));
  if(!ingredientsTxt){
    // if the file didn't open, print an error:
    Serial.println(F("error opening ingred.txt"));
    state = F("error");
    return;
  }

  Serial.println(F("saving Ingredients"));

  for(int i = 0; i < maxNumOfCocktails; i++){
    if(cocktails[i].getName() == "") return;

    for(int j = 0; j < numOfTanks; j++){
      // open file for reading
      ingredientsTxt = SD.open(F("ingred.txt"));

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
        ingredientsTxt = SD.open(F("ingred.txt"), FILE_WRITE);
        ingredientsTxt.print(ingredient + ",");
        ingredientsTxt.close();
      }
    }
  }

  ingredientsTxt.close();
}

void saveTankContentsToSD(String lastState){
  File tankContentsTxt = SD.open(F("tanks.txt"), FILE_WRITE | O_TRUNC);    // O_TRUNC deletes existing file content
  if(!tankContentsTxt){
    // if the file didn't open, print an error:
    Serial.println(F("error opening tanks.txt"));
    state = F("error");
    return;
  }

  Serial.println(F("saving TankContents"));

  for(int i = 0; i < numOfTanks; i++){
    String ingredientName = cs.getTankContent(i);   //
    if(ingredientName == "") 
      ingredientName == F("leer");
    
    tankContentsTxt.print(ingredientName + "," + cs.getTankFillingLevel(i) + ",");
    
  }
  tankContentsTxt.print(lastState + ",");
  tankContentsTxt.close();
}

void deleteTANKS_txt(){
  File tankContentsTxt = SD.open(F("tanks.txt"), FILE_WRITE | O_TRUNC);    // O_TRUNC deletes existing file content
  tankContentsTxt.close();
}










