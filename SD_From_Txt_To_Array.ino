#include <SD.h>
#include <SPI.h>

#define SDpin 10

File textFile;

struct Ingredient{
  char ingredient[20];
  int nUnit;
};

struct Recipe{
  char introduction[100];
  struct Ingredient ingredients[6];
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  if(!SD.begin(SDpin)){
    Serial.println("Initialisierung erfolgt nicht.");
    return;
  }
  Serial.println("Initialisierung elfolgt.");
}

void loop() {
  // put your main code here, to run repeatedly:
  struct Recipe bsp;
}

void getString(Recipe &n,char filename[]){
  int index = 0;
  textFile = SD.open(filename);
  while(textFile.available()){
    char c = textFile.read();
    n.introduction[index]=c;
    index++;
  }
}

void getIngredients(Recipe &n,char filename[]){
  int index1 =0;
  int index2 =0;
  textFile = SD.open(filename);
  bool state= 0;
  while(textFile.available()){
    char c = textFile.read();
    
    while(true){
      if(c==":") break;
      n.ingredients[index1].ingredient[index2]=c;
      index2++; 
    }
    
    if("0"<=c && c<="9"){
      n.ingredients[index1].nUnit= c-'0';  // char->int
    }

    if(c=="\n") index1++;
  }
}

