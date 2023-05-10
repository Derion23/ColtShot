

int pinDT = 43;
int pinCLK = 45;

int counter = 0;
int clkState, previousCklState;

// Variables to prevent counter from bouncing, jumping
long timeOfLastDebounce = 0;
int delayOfDebounce = 100;

void setup() {  
  Serial.begin(9600);

  pinMode(pinDT, INPUT_PULLUP);
  pinMode(pinCLK, INPUT_PULLUP);

  previousCklState = digitalRead(pinCLK);
  Serial.println("Program started...");
}

void loop() {

  clkState = digitalRead(pinCLK);

  // check if enough time has passed and if rotation occured
  if((millis() - timeOfLastDebounce) > delayOfDebounce && clkState != previousCklState){
    timeOfLastDebounce = millis();

    if(digitalRead(pinDT) != clkState){
      counter++;
    } else{
      counter--;
    }
    Serial.println(counter);
  }

  // update previousCklState
  previousCklState = clkState;
}
