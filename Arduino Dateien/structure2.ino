#define nTanks 6
// print definiert

class Ingredient{
private:
	String name;
	int amount;
public:
	Ingredient(String name, int amount){
		this->name = name;
		this->amount = amount;
	}
	String getName(){
		return this->name;
	}
	int getAmount(){
		return this->amount;
	}
}

class Cocktail{
private:
	String name;
	Ingredient ingredients[nTanks];
	int nIngredients[nTanks];
public:
	Cocktail(String name){
		this->name = name;
	}
	int addIngredient(String name, int amount){
		if (nIngredients >= nTanks){
			return 0;
		}
		Ingredient this->ingredients[nIngredients] = new Ingredient(name, amount);
		nIngredients++
		return 1;
	}
	Ingredient[] getListOfIngredients(){
		return ingredients;
	}
	void printCocktail(){
		print (this->name+":");
		for (int i = 0; i < nTanks; i++){
			print(" ->" + char(this->amounts[i].getAmount)+"cl of " + this->ingredients[i].getName);
		}
	}
	Ingredient getIngredient(int a){
		return ingredients[a];
	}
}

class Tank{
private:
	fillingLevel = 0;
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
		this->fillinglevel -= amount;
		return 1;
	}
}

class ColtShot{
private:
	int filledTanks;
public:
	Tanks tanks[nTanks];
	Coltshot(){
		for (int i = 0; i < nTanks; i++){
			Tank this->tanks[i] = new Tank("");
		}
	}
	int addContent(String ing){
		for (int i 0;;i++){
			if this->tanks[i].getName == "";
			this->tanks[i].setName(ing);
			return 1;
		}
		return 0;
	}
	int emptyTanks(){
		for (int i = 0; i < nTanks; i++){
			tanks[i].fillTo(0);
			tanks[i].setName("");
		}
		return 1;
	}

	int printTanks(){
		print("Tanks:");
		for (int i = 0; o < nTanks; i++){
			print("Content: " + tanks[i].getContent() + ", Amount: " + char(tanks[i].getAmount()) + "cl");
		}
	}
}

