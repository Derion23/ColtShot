/*
getTurnKnobIndex() -> increases/decreases menucounter according to turn
int inputState -> interrupt-based state of last input, can be read out from anywhere in the code
*/

int inputState; //0: kein Input, 1: knobTurn forward, -1: knobTurn backwards, 2: BTNPress (maybe 0-3 -> unsigned word, weniger Speicheplatz)


attachInterrupt(digitalPinToInterrupt(pinBTN), buttonPressINT, FALLING); //wenn pinBTN -> LOW => Button pressed

void buttonPressINT(){
  inputState = 2;
  Serial.println("Button: pressed");
}

attachInterrupt(digitalPinToInterrupt(pinCLK), knobTurnINT, CHANGE);

void knobTurnINT(){
  int clkState = digitalRead(pinCLK);
  int dtState = digitalRead(pinDT);

  // check if enough time has passed and if rotation occured
  if((millis() - timeOfLastDebounce) > delayOfDebounce){
      timeOfLastDebounce = millis();

      if(dtState != clkState){          
          inputState = 1;
          Serial.println("Knob: positive rotation");
      } else if(dtState == clkState){
          inputState = -1;     
          Serial.println("Knob: negative rotation");
      }
    }
}


int getTurnKnobIndex(){
  // Check if turning is allowed
  if(!turningAllowed) return 0;
  if (inputState == 2){            // falls erster Input ein buttonpress war
    return 0;
  }
  if (counter < numOfChoices-1 && inputState){
    counter++;
  }
  else if (counter > 0 && inputState == -1){
    counter --;
  }
  return counter;
}
