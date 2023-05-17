int queuesize = 10; //nur in dequeueInput() und INTs verwendet, kann statisch eingetragen werden (ohne variable)
int inputState;
int inputQueuePoint = 0; //pointer auf das aktuell erste freie inputQueue[] Element (kein Hardware-pointer!! ;P)

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

      if(dtState != clkState &&){           //inpQ[inpQP++] = dtState!=clkState; => Hardwaretechnisch schneller (branchless)
          inputState = 1;
          Serial.println("Knob: positive rotation");
      } else if(dtState == clkState){
          inputState = -1;     //inpQ[inpQP++] = -dtState==clkState; => aber print() fällt weg und Geschw. wahrsch. vernachlässigar
          Serial.println("Knob: negative rotation");
      }
    }
}

/*int dequeueInput(){
  int out = inputQueue[0];
  if (out == 0){
    return 0;           //keine Eingabe erfolgt
  }
  for (int i = 0; i < queuesize && inputQueue[i] != 0; i++){  //queue shift, schiebt den nächsten Input nach vorne
    inputQueue[i] = inputQueue[i+1];
  }
  inputQueue[queuesize-1] = 0;
  inputQueuePoint--;
  return out;
}*/

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
  return;
}
