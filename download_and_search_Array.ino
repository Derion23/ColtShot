#include <SD.h>
#include <SPI.h>
#define SDpin 10
#define nTanks 6
#define nCocktails 60

File Rezept;
File Tanks;

void print(String a){}

class Ingredient{
  private:
    String name;
    int amount;
  public:
    Ingredient(){}
    void setName(String a){ this->name = a; }
    void setAmount(int a){ this->amount = a; }
    String getName(){ return this->name; }
    int getAmount(){ return this->amount; }
};

class Cocktail{
  private:
    String name;
    Ingredient ingredients[nTanks];
    int nIngredients = 0;

  public:
    Cocktail(){}
    Cocktail(String name){
      this->name = name;
       /* for (int i = 0; i < nTanks; i++){
         Ingredient this->ingredients[i]) = new Ingredient ("", 0);
         nIngredients[i] = 0;
        }*/
    }
    void setName(String name){
      this->name = name;
    }
    int addIngredient(String name, int amount){
      if (nIngredients >= nTanks){
        return 0;
      }
      this->ingredients[nIngredients].setName(name);
      this->ingredients[nIngredients].setAmount(amount);
      nIngredients++;
      return 1;
    }
    /* Ingredient[] getListOfIngredients(){
      return ingredients;
    }*/
    void printCocktail(){
      print (this->name+":");
      for (int i = 0; i < nTanks; i++){
      print(" ->" + String(this->ingredients[i].getAmount())+"cl of " + String(this->ingredients[i].getName()));
      }
    }
    Ingredient getIngredient(int a){ return ingredients[a]; }

};


class Tank{
  private:
    int fillingLevel = 0;
    String content;
  public:
  
    Tank(){ this->content = ""; }
    void setIngredient(String ing){ this->content = ing; }
    int fillTo(int amount){
      this->fillingLevel = amount;
      return 1;
    }
    int withdraw(int amount){
      this->fillingLevel -= amount;
      return 1;
    }
    String getContent(){  return this->content; }
    int getAmount(){ return this->fillingLevel;}
};

class ColtShot{
  private:
    int filledTanks = 0;
  public:
    ColtShot(){}
    Tank tanks[nTanks];
    int addContent(String ing){
      if(filledTanks>=nTanks) return 1;
      tanks[filledTanks].setIngredient(ing);
      filledTanks++;
      return 0;
    }

    int addContent(String ing, int level){
      if(filledTanks>=nTanks) return 1;
      tanks[filledTanks].setIngredient(ing);
      tanks[filledTanks].fillTo(level);
      filledTanks++;
      return 0;
    }

    int emptyTanks(){
      for (int i = 0; i < nTanks; i++){
        tanks[i].fillTo(0);
        tanks[i].setIngredient("");
      }
      return 1;
    }

    int printTanks(){
      print("Tanks:");
      for (int i = 0; i < nTanks; i++){
        print("Content: " + tanks[i].getContent() + ", Amount: " + char(tanks[i].getAmount()) + "cl");
      }
    }

    String getTankContent(int index){ return tanks[index].getContent();}

    char getTankFillingLevel(int index){ return char(tanks[index].getAmount()); }
  };

Cocktail cocktails[nCocktails];

void setup(){
  Serial.begin(9600);
  if(!SD.begin(SDpin)){
    Serial.println("Initialisierung erfolgt nicht.");
    return;
  }
  Serial.println("Initialisierung erfolgt.");
}
void loop(){
  RezeptHerunterladen(cocktails);
  
}

//Format: "Name"([(Zutat,Menge)],Eis);"Name"([(Zutat,Menge)],Eis);...
void RezeptHerunterladen(class Cocktail cocktails[]){
  Rezept = SD.open("Rezepte.txt");
  char c;
  String name;
  String name2;
  int amount;
  int index = -1;

  while(Rezept.available()){
    c=Rezept.read();
    if(c=='\"'){
      index++;
      name = Serial.readStringUntil('\"');
      cocktails[index].setName(name);
    }
    if(c=='('){ name2 = Serial.readStringUntil(','); }
    if('0'<=c && '9'>=c){ amount = c-'0';}
    cocktails[index].addIngredient(name2,amount);

    if(index>=nCocktails){ return; }
  }
  Rezept.close();
}

void SearchByName(Cocktail cocktails[], String SearchingName, int ResultArray[]){
  //The ResultArray is the array of the Searched Cocktails' indexes.
  int index1 = 0, index2 = 0;
  bool state = false;
  for(index1 = 0; index1< nCocktails; index1++){
    for(int i=0; i<nTanks; i++){
      String ingredient = cocktails[index1].getIngredient(i).getName();
      state= ingredient.equals(SearchingName);
    }
    if(state == true){
      ResultArray[index2] = index1;
    }
  }
}


//Format: *ContentName:fillLevel\n*ContentName:fillLevel\n...
void TanksHerunterladen(class ColtShot &coltshot){
  Tanks = SD.open("tanks.txt");
  char c;
  String name;
  int fillinglevel;
  while(Tanks.available()){
    c=Tanks.read();
    if(c=='*'){  name = Serial.readStringUntil(':');  }
    if('0'<=c && '9'>=c){ fillinglevel = c-'0';}
    coltshot.addContent(name,fillinglevel);
  }
  Tanks.close();
}

void TanksHochladen(class ColtShot &coltshot){

    SD.remove("tanks.txt");
  Tanks = SD.open("tanks.txt");
  for(int i=0;i<nTanks;i++){
    Tanks.print('*');
    Tanks.print(coltshot.getTankContent(i));
    Tanks.print(':');
    Tanks.println(coltshot.getTankFillingLevel(i));
  }
  Tanks.close();

}
