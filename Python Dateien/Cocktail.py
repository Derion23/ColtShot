class Cocktail:
  name = ""
  ingredients = [] # ingredient[0] -> name; ingredient[1] -> amount in cl

  def __init__(self, name, ingredients):
    self.name = name
    self.ingredients = ingredients

  def getListOfIngredients(self):
    listOfIngredients = []
    for ingredient in self.ingredients:
      listOfIngredients.append(ingredient[0])
    return listOfIngredients


  def printCocktail(self):
    print("%s:" %(self.name))
    for ingredient in self.ingredients:
      print("   %s, %dcl" %(ingredient[0], ingredient[1]))
