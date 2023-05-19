#include <SD.h>
#include <SPI.h>
#define SDpin 10
#define nTanks 6

File Rezept;

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
    Cocktail(String name){
      this->name = name;
       /* for (int i = 0; i < nTanks; i++){
         Ingredient this->ingredients[i]) = new Ingredient ("", 0);
         nIngredients[i] = 0;
        }*/
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
    
    Cocktail * next;
};

struct List{
  class Cocktail* front;
  class Cocktail* back;

  List(){
    front = nullptr;
    back = nullptr;
  }

  ~List(){
    Cocktail* &current = front;
    while (current != nullptr) {
      Cocktail* &next = current->next;
      delete current;
      current = next;
    }
    front = nullptr;
    back = nullptr;
  }
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
    Tank tanks[nTanks];
    Coltshot(){
    }
    int addContent(String ing){
      tanks[filledTanks].setIngredient(ing);
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
  };



void setup(){
  Serial.begin(9600);
  if(!SD.begin(SDpin)){
    Serial.println("Initialisierung erfolgt nicht.");
    return;
  }
  Serial.println("Initialisierung erfolgt.");
}
void loop(){
  List *list;
  RezeptHerunterladen(list);
}

//Format: "Name"([(Zutat,Menge)],Eis);"Name"([(Zutat,Menge)],Eis);...
void RezeptHerunterladen(struct List * &list){
  Rezept = SD.open("Rezepte.txt");
  char c;
  String name;
  String name2;
  int amount;

  while(Rezept.available()){
    c=Rezept.read();
    if(c=="\""){
      name = Serial.readStringUntil("\"");
      helpHerunterladen(list,name);
    }
    if(c=="("){ name2 = Serial.readStringUntil(","); }
    if('0'<=c && '9'>=c){ amount = c-'0';}
    list->back->addIngredient(name2, amount);
  }
}

void helpHerunterladen(struct List * &list, String name){
  Cocktail neu = Cocktail(name);
  if(list->front==nullptr){
    list->front = &neu;
    list->back = &neu;
    neu.next = nullptr;
  }else{
    list->back->next = &neu;
    list->back=&neu;
    neu.next = nullptr;
  }
}

void searchByIngredient(class Cocktail *&cocktail, struct List* &newlist, String IngredientName){
  bool state = false;
  if(cocktail == nullptr){ return; }
  for(int i=0;i<nTanks;i++){
    String ingredient= cocktail->getIngredient(i).getName();
    state = IngredientName.equals(ingredient);
    //Wenn gleich, wird state true sein.
  }
  if(state == true){
    searchHelp(cocktail,newlist);
  }
  searchByIngredient(cocktail->next, newlist, IngredientName);
}

void searchHelp(class Cocktail * &cocktail, struct List* &list){
  if(list->front==nullptr){
    list->front = cocktail;
    list->back = cocktail;
    cocktail->next=nullptr;
  }else{
    list->back->next= cocktail;
    list->back=cocktail;
    cocktail->next=nullptr;
  }
}
