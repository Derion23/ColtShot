class Tank:
  content = ""
  fillingLevel = 0

  def setContentTo(self, content):
    self.content = content

  def fillTo(self, fillingLevel):
    self.fillingLevel = fillingLevel

  def withdraw(self, amount):
    self.fillingLevel -= amount
