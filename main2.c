#include <stdio.h>

float clPerSecond = 0.5;
#define sizeName 20
#define nContainer 6
#define MaxNRecipes 5
#define ingNameSpace 30
int nEnteredRecipes = 0;
char ingredients[][30] = {"Sprite", "Vodka", "OSaft", "Rum", "Sirup", "Cola", "A-Saft", "Kiba", "Bier", "Wein", "wtfkommtincocktailsrein"};
char containerContents[nContainer][30] = {"Sprite", "Orangensaft", "Vodka", "Rum", "Sirup", "Cola"};

//arduino vars
int potprev;
int potval;
int potdelta;
//ende

int menustate; /*
0 - start, tanks befüllen
1 - 
*/

struct Ingredient{
	char ingredient[ingNameSpace];
	int nUnit;
};

struct Recipe {
	char name[sizeName];
	struct Ingredient ingredients[nContainer];
	int nIce;
	int timeSwirl;
};
struct Recipe recipes[MaxNRecipes];


int assignString(char* arr, char* src, int len){
	for (int i = 0; i < len; i++){
		*(arr+i) = *(src+i);
	}
	return 1;
}

int deleteRecipe(int num){
	num--;
	printf("lösche %s\n\n", recipes[num].name);
	for (int i = 0; i < nEnteredRecipes-num+1; i++){
		assignString(recipes[num+i].name, recipes[num+i+1].name, sizeName);
		for (int k = 0; k < nContainer; k++){
			recipes[num+i].ingredients[k] = recipes[num+i+1].ingredients[k];
		}
		recipes[num+i].nIce = recipes[num+1+i].nIce;
		recipes[num+i].timeSwirl = recipes[num+1+i].timeSwirl;

	}
	nEnteredRecipes--;
	return 1;
}

int printContainers(){
	for (int i = 0; i < nContainer-1; i++){
		printf("%s, ", containerContents[i]);
	}
	printf("%s\n\n", containerContents[nContainer-1]);
}



int newRecipe(char* name, char** ings, int* ing_amounts, int nIce, int timeSwirl){
	if (nEnteredRecipes >= MaxNRecipes) {printf("Maximale Anzahl an Rezpten erreicht! Bitte mind. ein anderes Rezept löschen!\n"); return 0;}
	assignString(recipes[nEnteredRecipes].name, name, sizeName);
	for (int i = 0; i < nContainer; i++){
		assignString(recipes[nEnteredRecipes].ingredients[i].ingredient, ings[i], ingNameSpace);
		recipes[nEnteredRecipes].ingredients[i].nUnit = ing_amounts[i];
	}
	recipes[nEnteredRecipes].nIce = nIce;
	recipes[nEnteredRecipes].timeSwirl = timeSwirl;
	nEnteredRecipes++;
	return 1;
}

int printByNum(int num){
	if (num > nEnteredRecipes){printf("index out of bound: Rezept %d existiert nicht!\n", num); return 0;}
	printf("%s:\n", recipes[num-1].name);
	printf("-> %d Ice cubes\n-> ", recipes[num-1].nIce);
	for (int i = 0; i < nContainer-1; i++){																			//		auslesen von Stringarrays..?
	//	printf("%d\n", i);
		printf("%d Units %s, ", /*recipes[num-1].ingredients[i].nUnit*/10, recipes[num-i].ingredients[i].ingredient);
	}
	printf("%d Units %s\n-> Swirl %d seconds\n\n", /*recipes[num-1].ingredients[nContainer-1].nUnit*/10, recipes[num-1].ingredients[nContainer-1].ingredient, recipes[num-1].timeSwirl);
	return 1;
}

int printRecipeList(){
	for (int i = 0; i<nEnteredRecipes; i++){
		printf("%d. %s\n", i+1, recipes[i].name);
	}
	printf("\n");
}

int equalChar(char* one, char* two, int size){
	for (int i = 0; i < size; i++){
		if (*(one+i) != *(two+i)){
			return 0;
		}
	}
	return 1;
}

int searchForRecipe(char* name){
	for (int i = 0; i < nEnteredRecipes; i++){
		if (equalChar(name, recipes[i].name, sizeName)){
			return i;
		}
	}
	return 0;
}

int printByName(char* name){
	int num = searchForRecipe(name);
	if (num){
		printByNum(num+1);
		return 1;
	} 
	printf("Rezept %s existiert nicht!\n", name);
	return 0;
}

int deleteByName(char* name){
int num = searchForRecipe(name);
	if (num){
		deleteRecipe(num+1);
		return 1;
	} 
	printf("Rezept %s existiert nicht!\n", name);
	return 0;
}


void main(){
	printf("heyy\n");
	char* testings[] = {"Vodkda", "Rum", "Kiba","osaft","wasser","sirup"};
	//recipes[0].timeContainer[2] = 5;
	//char name[] = "hii";
	//assignString(recipes[0].name, "hii", 20);
	//recipes[0].nIce = 2;
	int nums[] = {1,2,3,4,5,6};
	newRecipe("Mische", testings, nums, 2, 20);
	newRecipe("Mische2",testings, nums , 3, 15);
	newRecipe("Mische3",testings, nums, 3, 15);
	//recipes[2].timeSwirl = 10;
	//printf("%d\n", recipes[0].nIce);
	//printf("%d\n", recipes[0].timeContainer[2]);
	//printf("%s\n", recipes[0].name);
	printRecipeList();
	printContainers();
	printByNum(1);
	printByName("Mische2");
	deleteRecipe(2);
	newRecipe("Mische2", testings,nums, 3, 15);
	printRecipeList();
	deleteByName("Mische3");
	printRecipeList();

}
void loop(){

}

void prntMenu(){

}

void menu(){
	
}










/*
funclist

newRecipe()

printRecipeList()

printContainers()

printByNum()

printByName()

deleteByNum()

deleteByName()

=================================
