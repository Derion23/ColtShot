#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

LCDWIKI_KBV mylcd(ILI9486,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset

//define some colour values
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

void setup() {
  Serial.begin(9600);
  mylcd.Init_LCD();
  Serial.println(mylcd.Read_ID(), HEX);
  testText();
}

void loop() {

}

// -----------------------------------------------------------------------------------------


//display some strings
void testText(){
  // no idea what this does :)
  mylcd.Set_Text_Mode(0);
  // Set up Display
  mylcd.Fill_Screen(BLACK);  
  mylcd.Set_Text_Back_colour(BLACK);
  mylcd.Set_Text_colour(WHITE);
  mylcd.Set_Text_Size(2);


  int num = mylcd.Get_Display_Width();
  String displayString = String("DisplaySize: ") + num + String("x");

  mylcd.Print_String(displayString, 5, 5);
}










