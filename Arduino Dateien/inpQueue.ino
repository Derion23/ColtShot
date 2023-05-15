int queuesize = 10; //nur in dequeueInput() und INTs verwendet, kann statisch eingetragen werden (ohne variable)
int inputQueue[queuesize];
int inputQueuePoint = 0; //pointer auf das aktuell erste freie inputQueue[] Element (kein Hardware-pointer!! ;P)

attachInterrupt(digitalPinToInterrupt(pinBTN), buttonPressINT, FALLING); //wenn pinBTN -> LOW => Button pressed

void buttonPressINT(){
	if (inputQueuePoint+1 >= queuesize){				//check if queue is full
		Serial.println("inputQueue overflow!");
		return;
	}
	inputQueue[inpuQueuePoint++] = 2;
	Serial.println("Button: pressed");
}

attachInterrupt(digitalPinToInterrupt(pinCLK), knobTurnINT, CHANGE); //pinCLK ändert zustand (evtl fällt, steigt?), wenn knob gedreht?

void knobTurnINT(){
	if (inputQueuePoint+1 >= queuesize){				//check if queue is full
		Serial.println("inputQueue overflow!");
		return;
	}
 	int clkState = digitalRead(pinCLK);
 	int dtState = digitalRead(pinDT);

 	// check if enough time has passed and if rotation occured
 	if((millis() - timeOfLastDebounce) > delayOfDebounce && clkState != previousCklState){
    	timeOfLastDebounce = millis();

    	if(dtState != clkState &&){						//inpQ[inpQP++] = dtState!=clkState; => Hardwaretechnisch schneller (branchless)
      		inputQueue[inputQueuePoint++] = 1;
      		Serial.println("Knob: positive rotation");
    	} else if(dtState == clkState){
      		inputQueue[inputQueuePoint++] = -1;			//inpQ[inpQP++] = -dtState==clkState; => aber print() fällt weg und Geschw. wahrsch. vernachlässigar
      		Serial.println("Knob: negative rotation");
    	}
  	}
}

int dequeueInput(){
	int out = inputQueue[0];
	if (out == 0){
		return 0;						//keine Eingabe erfolgt
	}
	for (int i = 0; i < queuesize && inputQueue[i] != 0; i++){ 	//queue shift, schiebt den nächsten Input nach vorne
		inputQueue[i] = inputQueue[i+1];
	}
	inputQueue[queuesize-1] = 0;
	inputQueuePoint--;
	return out;
}

int getTurnKnobIndex(){
  // Check if turning is allowed
  if(!turningAllowed) return 0;
  if (inputQueue[0] == 2){						// falls erster Input ein buttonpress war
  	return 0;
  }

  /*int clkState = digitalRead(pinCLK);										// |
  int dtState = digitalRead(pinDT);										// |
														// |
  // check if enough time has passed and if rotation occured							// |
  if((millis() - timeOfLastDebounce) > delayOfDebounce && clkState != previousCklState){			// |
    timeOfLastDebounce = millis();										// |
														// |
    if(dtState != clkState && counter < numOfChoices - 1){							// |
      counter++;												// |
    } else if(dtState == clkState && counter > 0){								// |
      counter--;												// |
    }														// |
    Serial.println(counter);											// |
  }*/														// L--> unnecessary, just for reference
  int temp = dequeueInput();
  if (temp > 0 && counter < numOfChoices-1){
	  counter++;
  }
  else if(temp < 0 && counter > 0){
	  counter--;
  }
  Serial.println(counter);
  // update previousCklState
  //previousCklState = clkState; 											//unnecessary
  return counter;
}
