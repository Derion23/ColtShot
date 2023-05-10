import math
import random

import ColtShot
import Cocktail

supportedIngredients = ["Sprudelwasser", "Whiskey", "Limettensaft", "Tequila", "Orangensaft", "Gin", "Vodka", "Rum", "Cola", "TonicWater", "OrangenLikör", "Sirup", "Martini", "Cachaca", "GrenadineSirup"]

supportedCocktails = []
supportedCocktails.append(Cocktail.Cocktail("Gin Tonic", [("Gin", 4), ("TonicWater", 20)]))
supportedCocktails.append(Cocktail.Cocktail("Margarita", [("OrangenLikör", 1), ("Tequila", 5), ("Limettensaft", 2)]))
supportedCocktails.append(Cocktail.Cocktail("Cuba Libre", [("Rum", 5), ("Cola", 10), ("Limettensaft", 1)]))
supportedCocktails.append(Cocktail.Cocktail("Daiquiri", [("Rum", 5), ("Limettensaft", 3), ("Sirup", 2)]))
supportedCocktails.append(Cocktail.Cocktail("Vodka Gimlet", [("Vodka", 4),("Limettensaft", 2), ("Sirup", 2)]))
supportedCocktails.append(Cocktail.Cocktail("Martini Cola", [("Martini", 5), ("Cola", 10)]))
supportedCocktails.append(Cocktail.Cocktail("Mojito", [("Rum", 5), ("Sprudelwasser", 6), ("Limettensaft", 1), ("Sirup", 2)]))
supportedCocktails.append(Cocktail.Cocktail("Caipirinha", [("Cachaca", 7),("Limettensaft", 1)]))
supportedCocktails.append(Cocktail.Cocktail("Tequila Sunrise", [("Tequila", 6),("OrangenLikör", 2), ("Orangensaft", 10), ("GrenadineSirup", 2), ("Limettensaft", 2)]))

#VARIABLES
cs = ColtShot.ColtShot(8)
chosenIngredientsGame = []
state = "start"

# CONSTANTS
tankMarkInCl = 10         # one Tank mark is equal to 10cl
maxGlassCapacity = 30     # a Cocktail has to be smaller than 30cl

def addCocktail(cocktail):
  tankContents = cs.tankContents()
  for ingredient in cocktail.ingredients:
    if not ingredient[0] in tankContents:
      cs.addContent(ingredient[0])

def makeCocktail(cocktail):
  for ingredient in cocktail.ingredients:
      for tank in cs.tanks:
        if tank.content != ingredient[0]: 
          continue
        tank.withdraw(ingredient[1])

# HELPER METHODS
def getPossibleCocktails():
  possibleCocktails = []
  tankContents = cs.tankContents()
  availableCocktails = getAvailableCocktails()

  for cocktail in supportedCocktails:
    if cocktail in availableCocktails: # Cocktail already available
      continue
    newIngredients = 0              
    for ingredient in cocktail.ingredients:
      if not ingredient[0] in tankContents:
        newIngredients += 1
    # Cocktail has more new Ingredients than available
    if(newIngredients > tankContents.count("")): 
      continue

    possibleCocktails.append(cocktail)
  return possibleCocktails

def getAvailableCocktails():
  availableCocktails = []
  tankContents = cs.tankContents()

  for cocktail in supportedCocktails:
    cocktailAvailable = True
    for ingredient in cocktail.ingredients:
      if not ingredient[0] in tankContents:
        cocktailAvailable = False
        break
    if cocktailAvailable:
      availableCocktails.append(cocktail)
  return availableCocktails

def returnIntegerInput(maxNum, minNum = 1):
  while True:
    choice = input().strip()

    if not choice.isdigit():
      print("Bitte eine Zahl eingeben")
      continue
    intChoice = int(choice)
    if intChoice < minNum or intChoice > maxNum:
      print("Bitte eine Zahl zwischen %d und %d angeben" %(minNum, maxNum))
      continue
    break
  return int(choice)

def fillTank(tank):
  global state

  if tank.content == "":
    print("Du kannst keinen Tank ohne festgelegten Inhalt befüllen")
    return tank

  # Turn Machine
  print("Fülle den Tank direkt über dem Glas, genau bis zu einer Markierung mit %s" %tank.content)

  print("%s-Tank gefüllt bis Markierung: " %tank.content)
  intChoice = returnIntegerInput(5, max(1,  math.ceil(tank.fillingLevel / tankMarkInCl))) # 5 marks

  tank.fillTo(intChoice * tankMarkInCl)

  print("Dieser Tank wurde gespeichert, Inhalt: %s Füllstand: %dcl" %(tank.content, intChoice * tankMarkInCl))

  return tank

def getRecipeStrFromCocktail(cocktail):
  recipeStr = ""
  for ingredient in cocktail.ingredients:
    newIngredient = f'{ingredient[0]}({ingredient[1]}cl)'
    recipeStr = f'{recipeStr}, {newIngredient}'
  recipeStr = recipeStr[2:]
  return recipeStr



# MENU FUNCTIONS
def start():
  global state

  print("Wähle eine der folgenden Möglichkeiten die Maschine zu befüllen:")
  print("1) Zuerst Cocktails auswählen")
  print("2) Zuerst Zutaten auswählen")

  intChoice = returnIntegerInput(2)
  
  if intChoice == 1:
    state = "addCocktails"
  elif intChoice == 2:
    state = "addIngredients"

def addIngredients():
  global state

  while cs.tankContents().count("") > 0:
    print("Tanks: %s" %cs.printTankContents())
    print("Wähle einen der folgenden Zutaten aus:")
    i = 1
    availableIngredients = []
    for ingredient in supportedIngredients:
      if ingredient in cs.tankContents():
        continue
      availableIngredients.append(ingredient)

    for ingredient in availableIngredients:
      print("%d) %s" %(i, ingredient))
      i += 1
    # other options
    print("")
    addCocktails = False
    fillTanks = False
    if len(getPossibleCocktails()) > 0:
      print("%d) Cocktails hinzufügen" %i)
      addCocktails = True
      i += 1
    if len(cs.tankContents()) - cs.tankContents().count("") > 0:
      print("%d) Tanks befüllen" %i)
      fillTanks = True
      i += 1
    print("%d) Zurück zum Hauptmenü" %i)
    

    intChoice = returnIntegerInput(i)

    #options
    # # Cocktails hinzufügen
    if addCocktails and intChoice == i - (2 if fillTanks else 1): 
      state = "addCocktails"
      return

    if fillTanks and intChoice == i - 1: # Tanks befüllen
      state = "tankContentSet"
      return

    if intChoice == i:    # Zurück zum Hauptmenü
      cs.emptyTanks()
      state = "start"
      return

    #ingredients
    cs.addContent(availableIngredients[intChoice - 1])
  state = "tankContentSet"

def addCocktails():
  global state

  while len(getPossibleCocktails()) > 0:
    print("Tanks: %s" %cs.printTankContents())
    print("Wähle einen der folgenden Cocktails:")
    i = 1
    for cocktail in getPossibleCocktails():
      ingredients = ""
      for ingredient in cocktail.ingredients:
       ingredients = f'{ingredients}, {ingredient[0]}'
      print("%d) %s(%s)" %(i, cocktail.name, ingredients[2:]))
      i += 1
    # other options
    fillTanks = False
    print("")
    print("%d) Zutaten hinzufügen" %i)
    i += 1
    if len(cs.tankContents()) - cs.tankContents().count("") > 0:
      print("%d) Tanks befüllen" %i)
      fillTanks = True
      i += 1
    print("%d) Zurück zum Hauptmenü" %i)

    intChoice = returnIntegerInput(i)
      
    #options
    if intChoice == i - (2 if fillTanks else 1):    # Zutaten hinzufügen
      state = "addIngredients"
      return

    if fillTanks and intChoice == i - 1: # Tanks befüllen
      state = "tankContentSet"
      return

    if intChoice == i:    # Zurück zum Hauptmenü
      cs.emptyTanks()
      state = "start"
      return

    #cocktails
    addCocktail(getPossibleCocktails()[intChoice - 1])
  
  emptyTanks = cs.tankContents().count("")
  if  emptyTanks > 0:
    if emptyTanks == 1:
      print("Es ist noch 1 Tank frei und es sind keine Cocktails mehr möglich. Möchtest du:")
    else:
      print("Es sind noch %d Tanks frei und es sind keine Cocktails mehr möglich. Möchtest du:" %emptyTanks)
    print("1) Zutaten hinzufügen")
    print("2) Tanks befüllen")
    intChoice = returnIntegerInput(2)

    if intChoice == 1:
      state = "addIngredients"
      return
  state = "tankContentSet"
  
def tankContentSet():
  global state

  cocktailsStr = ""
  for cocktail in getAvailableCocktails():
    cocktailsStr = f'{cocktailsStr}, {cocktail.name}'
  cocktailsStr = cocktailsStr[2:]

  print("Gute Wahl. Du kannst mit deiner Auswahl folgende Cocktails zubereiten:")
  print(cocktailsStr)
  print("Die Tanks sollen dann wie folgt befüllt werden:")
  print(cs.printTankContents())

  #options
  print("")
  print("1) Bestätigen und Tanks befüllen")
  print("2) Zurück zum Hauptmenü")

  intChoice = returnIntegerInput(2)

  if intChoice == 1:
    state = "fillTanks"
  else:
    cs.emptyTanks()
    state = "start"

def fillTanks():
  global state

  for tank in cs.tanks:
    if tank.content == "":
      continue
    
    tank = fillTank(tank)

    #options
    print("1) Weiter befüllen")
    print("2) Zurück, alle Tanks müssen geleert werden")
    print("3) Zurück zum Hauptmenü, alle Tanks müssen geleert werden")
    intChoice = returnIntegerInput(3)

    if intChoice == 1:
      continue
    if intChoice == 2:
      print("Wirklich zurückgehen?")
      print("1) Nein, nächsten Tank befüllen")
      print("2) Ja, zurückgehen und alle Tanks leeren")
      intChoice = returnIntegerInput(2)
      if intChoice == 1:
        continue
      else:
        state = "tankContentsSet"
        return
    if intChoice == 3:
      print("Wirklich zurück zum Hauptmenü?")
      print("1) Nein, nächsten Tank befüllen")
      print("2) Ja, zurück zum Hauptmenü und alle Tanks leeren")
      intChoice = returnIntegerInput(2)
      if intChoice == 1:
        continue
      else:
        cs.emptyTanks()
        state = "start"
        return
  
  print("Die Maschine ist nun gefüllt")
  cs.printTanks()
  state = "machineFilled"

def machineFilled():
  global state

  print("Wähle eine der folgenden Möglichkeiten")
  print("1) Cocktailauswahl")
  print("2) Persönlicher Cocktail")
  print("3) Spielmodus")
  print("")
  #options
  print("4) Tanks anzeigen")
  print("5) Tanks nachfüllen")
  print("6) Neustart, zurück zum Hauptmenü")

  intChoice = returnIntegerInput(6)

  if intChoice == 1:
    state = "cocktailSelection"
  if intChoice == 2:
    state = "customCocktail"
  if intChoice == 3:
    state = "gameMenu"
  if intChoice == 4:
    cs.printTanks()
  if intChoice == 5:
    state = "refillTanks"
  if intChoice == 6:
    print("Wirklich zurück zum Hauptmenü?")
    print("1) Nein, erneut Option wählen")
    print("2) Ja, zurück zum Hauptmenü und alle Tanks leeren")
    intChoice = returnIntegerInput(2)
    if intChoice == 1:
      return
    else:
      cs.emptyTanks()
      state = "start"
      return

def cocktailSelection():
  global state

  print("Wähle einen der folgenden Cocktails:")
  i = 1
  for cocktail in getAvailableCocktails():
    print("%d) %s" %(i, cocktail.name))
    i += 1
  print("")
  print("%d) Zurück" %i)
  intChoice = returnIntegerInput(i)

  # options
  if(intChoice == i):
    state = "machineFilled"
    return
  
  #cocktails
  chosenCocktail = getAvailableCocktails()[intChoice - 1]
  print("%s besteht aus %s" %(chosenCocktail.name, getRecipeStrFromCocktail(chosenCocktail)))

  for ingredient in chosenCocktail.ingredients:
    for tank in cs.tanks:
      if tank.content != ingredient[0]:
        continue
      if tank.fillingLevel - ingredient[1] < 0:
        print("Cocktail nicht möglich, da %s-Tank zu niedrig" %ingredient[0])
        print("Möchstest du den %s-Tank auffüllen" %ingredient[0])
        print("1) Ja, auffüllen")
        print("2) Nein, anderen Cocktail wählen")
        intChoice = returnIntegerInput(2)
        if intChoice == 1:
          state = "refillTanks"
        return
      if tank.fillingLevel - ingredient[1] < 5:
        print("Hinweis: %s-Tank nach diesem Cocktail fast leer" %ingredient[0])

  print("")
  print("1) Cocktail zubereiten")
  print("2) Zurück")
  intChoice = returnIntegerInput(2)

  #options
  if intChoice == 1:
    # cs.makeCocktail(chosenCocktail)
    makeCocktail(chosenCocktail)
    print("Du kannst nun deinen %s herausnehmen. Prost!" %chosenCocktail.name)
    state = "machineFilled"
    return
  if intChoice == 2:
    return

def customCocktail():
  global state

  customCocktail = Cocktail.Cocktail("Shots", [])
  usedVolume = 0

  print("Hier kannst du deinen persönlichen Cocktail zubereiten lassen.")

  while usedVolume < maxGlassCapacity:
    print("Wähle eine Zutat für deinen persönlichen Cocktail")
    print("Persönlicher Cocktail: %s" %getRecipeStrFromCocktail(customCocktail))

    availableTanks = []
    for tank in cs.tanks:
      if tank.content == "" or tank.fillingLevel == 0 or tank.content in customCocktail.getListOfIngredients():
        continue
      availableTanks.append(tank)

    i = 1
    for tank in availableTanks:
      print("%d) %s(%scl)" %(i, tank.content, tank.fillingLevel))
      i += 1
    
    print("")
    customCocktailEmpty = True
    if customCocktail.ingredients != []:
      print("%d) Persönlichen Cocktail zubereiten" %i)
      customCocktailEmpty = False
      i += 1
    print("%d) Zurück, persönlicher Cocktail wird gelöscht" %i)
    intChoice = returnIntegerInput(i)

    #options
    if intChoice == i:
      state = "machineFilled"
      return
    if (not customCocktailEmpty) and intChoice == i - 1:
      break

    chosenTank = availableTanks[intChoice - 1]
    maxAdditionalVolume = min(chosenTank.fillingLevel, maxGlassCapacity - usedVolume)
    print("Wie viele cl von %s willst du in deinem Cocktail? Maximal %dcl möglich" %(chosenTank.content, maxAdditionalVolume))
    intChoice = returnIntegerInput(maxAdditionalVolume)

    print("%dcl %s zu deinem Cocktail hinzugefügt" %(intChoice, chosenTank.content))
    customCocktail.ingredients.append([chosenTank.content, intChoice])
    usedVolume += intChoice

  print("Soll dein persönlicher Cocktail bestehend aus %s hergestellt werden?" %getRecipeStrFromCocktail(customCocktail))
  print("1) Ja, Cocktail herstellen")
  print("2) Abbrechen, persönlicher Cocktail wird gelöscht")
  intChoice = returnIntegerInput(2)

  if intChoice == 2:
    state = "machineFilled"
    return
  
  makeCocktail(customCocktail)
  print("Du kannst nun deinen perönlichen Cocktail entnehmen, Prost!")
  state = "machineFilled"

def gameMenu():
  global state
  global chosenIngredientsGame

  print("Das ist der Spielmodus, wir spielen Russian-Roulette. Du wählst eine Zahl zwischen 1 und 3 aus. Eine zufällige Zahl zwischen 1 und 3 enthält einen besonderen Shot für dich.")
  print("Wähle zuerst aus welchen Zutaten der Shot hergestellt werden soll:")

  possibleIngedients = []
  for tank in cs.tanks:
    if tank.content == "":
      continue
    possibleIngedients.append(tank.content) 

  chosenIngredients = []
  while True:
    print("Zutat hinzufügen:")
    i = 1

    for ingredient in possibleIngedients:
      print("%d) %s" %(i, ingredient))
      i += 1

    print("")
    startPossible = False
    if len(chosenIngredients) > 0:
      startPossible = True
      chosenIngredientsStr = ""
      for ingredient in chosenIngredients:
        chosenIngredientsStr = f'{chosenIngredientsStr}, {ingredient}'
      print("%d) Russian-Roulette mit folgenden Zutaten starten: %s" %(i, chosenIngredientsStr[2:]))
      i += 1
    print("%d) Zurück zum Hauptmenü" %i)
    intChoice = returnIntegerInput(i)

    #options
    if intChoice == i:
      state = "machineFilled"
      return

    if startPossible and intChoice == i - 1:
      chosenIngredientsGame = chosenIngredients
      state = "game"
      return
    
    chosenIngredients.append(possibleIngedients[intChoice - 1])
    del possibleIngedients[intChoice - 1]
  
def game():
  global state
  global chosenIngredientsGame

  print("Wähle eine Zahl zwischen 1 und 3.")
  intChoice = returnIntegerInput(3)
  shotNum = random.randrange(1, 4) # random int in range [1,3]

  if intChoice == shotNum:
    
    print("Dieses Magazin war geladen. Dein Shot wird zubereitet.")
    shotIngredient = chosenIngredientsGame[random.randrange(0, len(chosenIngredientsGame))]
    gameCocktail = Cocktail.Cocktail("gameShot", [[shotIngredient, 2]]) # 2 is gameShot Size in cl
    
    for ingredient in gameCocktail.ingredients:
      for tank in cs.tanks:
        if tank.content != ingredient[0]:
          continue
        if tank.fillingLevel - ingredient[1] < 0:
          print("Shot nicht möglich, da %s-Tank zu niedrig" %ingredient[0])
          print("Möchstest du den %s-Tank auffüllen" %ingredient[0])
          print("1) Ja, auffüllen")
          print("2) Nein, zurück ins Hauptmenü")
          intChoice = returnIntegerInput(2)
          if intChoice == 1:
            state = "refillTanks"
          if intChoice == 2:
            state = "machineFilled"
          return
        if tank.fillingLevel - ingredient[1] < 5:
          print("Hinweis: %s-Tank nach diesem Cocktail fast leer" %ingredient[0])

    #cs.createShot(shotIngredient)
    makeCocktail(gameCocktail)
    print("Du kannst nun deinen Shot bestehend aus: %s(%dcl) entnehmen" %(shotIngredient, 2))
  else:
    print("Glück gehabt dieses Magazin war leer.")

  print("1) Nochmal spielen")
  print("2) Zurück zum Hauptmenü")
  intChoice = returnIntegerInput(2)

  if intChoice == 2:
    state = "machineFilled"
    return

def refillTanks():
  global state

  filledTanks = []
  for tank in cs.tanks:
    if tank.content == "":
      continue
    filledTanks.append(tank)

  print("Wähle welchen Tank du nachfüllen willst")
  i = 1
  for tank in filledTanks:
    print("%d) %s(%scl)" %(i, tank.content, tank.fillingLevel))
    i += 1
  print("")
  print("%d) Zurück" %i)
  intChoice = returnIntegerInput(i)

  # options
  if intChoice == i:
    state = "machineFilled"
    return
  
  chosenTank = filledTanks[intChoice - 1]
  print("1) Fortfahren mit dem Auffüllen des %s-Tanks" %chosenTank.content)
  print("2) Zurück")
  intChoice = returnIntegerInput(2)
  if intChoice == 2:
    return
  chosenTank = fillTank(chosenTank)



while True:
  if state == "start":
    start()
  elif state == "addIngredients":
    addIngredients()
  elif state == "addCocktails":
    addCocktails()
  elif state == "tankContentSet":
    tankContentSet()
  elif state == "fillTanks":
    fillTanks()
  elif state == "machineFilled":
    machineFilled()
  elif state == "cocktailSelection":
    cocktailSelection()
  elif state == "customCocktail":
    customCocktail()
  elif state == "gameMenu":
    gameMenu()
  elif state == "game":
    game()
  elif state == "refillTanks":
    refillTanks()
  
  elif state == "end":
    break
