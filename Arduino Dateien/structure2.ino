 #define nTanks 6
// print definiert

void print(String a){} //nur zum testkompilieren

class Ingredient{
private:
  String name;
  int amount;
public:
  /*Ingredient(String name, int amount){
    this->name = name;
    this->amount = amount;
  }*/
  Ingredient(){}
  void setName(String a){
    this->name = a;
  }
  void setAmount(int a){
    this->amount = a;
  }
  String getName(){
    return this->name;
  }
  int getAmount(){
    return this->amount;
  }
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
  Ingredient getIngredient(int a){
    return ingredients[a];
  }
};

class Tank{
private:
  int fillingLevel = 0;
  String content;
public:
  
  Tank(){
    this->content = "";
  }
  void setIngredient(String ing){
    this->content = ing;
  }
  int fillTo(int amount){
    this->fillingLevel = amount;
    return 1;
  }
  int withdraw(int amount){
    this->fillingLevel -= amount;
    return 1;
  }
  String getContent(){
    return this->content;
  }
  int getAmount(){
    return this->fillingLevel;
  }
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

void setup(){}
void loop(){}
