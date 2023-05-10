import Tank

class ColtShot:
  tanks = []

  def __init__(self, tankCount):
    self.tanks = []
    for i in range(tankCount):
      self.tanks.append(Tank.Tank())

  def addContent(self, content):
    for tank in self.tanks:
      if tank.content == "":
        tank.setContentTo(content)
        break
  
  def emptyTanks(self):
    for tank in self.tanks:
      tank.setContentTo("")
      tank.fillTo(0)
  
  def printTanks(self):
    print("Tanks:")
    for tank in self.tanks:
      print("Content: %s, Amount: %dcl" %(tank.content, tank.fillingLevel))

  def tankContents(self):
    contents = []
    for tank in self.tanks:
      contents.append(tank.content)
    return contents

  def tankFillingLevels(self):
    fillingLevels = []
    for tank in self.tanks:
      fillingLevels.append(tank.fillingLevel)
    return fillingLevels

  def printTankContents(self):
    contentsStr = ""
    for tank in self.tanks:
      tankContent = tank.content if tank.content != "" else "leer"
      contentsStr = f'{contentsStr}, {tankContent}'
    return contentsStr[2:]