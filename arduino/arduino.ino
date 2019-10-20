#include <LiquidCrystal.h> //Incluimos la libreria LCD
LiquidCrystal lcd(7, 8, 9, 10, 11, 12); 
int rPackages = 0;
int lPackages = 0;
int pkgType = 0;
char left ='>';
char right =  '<';
char dir = ' ';
void setup(){
  
   lcd.begin(16, 2); // Activamos LCD
}
void loop(){


lcd.setCursor(0, 0); //Posicionamos el cursor en el LCD
lcd.print("RP:");//Mostramos el texto en el LCD
lcd.print(rPackages);//Mostramos el valor del Vin en el LCD
lcd.setCursor(0,1);
lcd.print("LP:");
lcd.print(lPackages);
lcd.setCursor(5,0);
lcd.print(right);
lcd.setCursor(5,1);
lcd.print(" Type:");
lcd.print(pkgType);
delay(5); //Se hace una lectura de voltaje cada 500 ms
}
