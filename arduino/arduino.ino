#include <LiquidCrystal.h> //Incluimos la libreria LCD
//#include <LiquidCrystal_I2C.h>
#include <FastLED.h>
#include <time.h>

FASTLED_USING_NAMESPACE
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
//#define BACKLIGHT_PIN 13 
//LiquidCrystal_I2C lcd(0x38);


#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    6
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    1
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120


String msg = " ";
int rPackages = 0;
int lPackages = 0;
int pkgType = 0;
char left ='>';
String right =  "<";
char dir = ' ';
void setup(){
  
   Serial.begin(9600);
   FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
   lcd.begin(16, 2); // Activamos LCD
   FastLED.setBrightness(BRIGHTNESS);
   
}
void loop(){
if(Serial.available()){
  msg = Serial.readString();
  splitMsg(msg);
  
}

lcd.setCursor(0, 0); //Posicionamos el cursor en el LCD
lcd.print("RP:");//Mostramos el texto en el LCD
lcd.print(rPackages);//Mostramos el valor de los paqutes derechos en el LCD
lcd.setCursor(0,1);// Movemos el cursor
lcd.print("LP:");//Escribimos el nombre de la variable
lcd.print(lPackages);//Mostramos el valor de los paqutes izquierdos en el LCD
lcd.setCursor(5,0);//movemos el cursor de nuevo
lcd.print(right); // Mostramos la direccion en que se estan moviendo los paquetes
lcd.setCursor(5,1); //Cambiamos el cursor
lcd.print(" Type:"); //Nombramos la variable
lcd.print(pkgType); //Mostramos el tipo de paquete.
delay(5); //Se hace una lectura de voltaje cada 500 ms
}

// Funcion que se encarga de sacar los valores que entran mediante el puerto serial.
void splitMsg(String msg){
  String firstScreen = getValue(msg, ";",0);
  String secondScreen = getValue(msg, ";",1);
  String thirdScreen = getValue(msg, ";",2);
  String rPkgs1 = getValue(firstScreen,",",0); 
  rPackages = (int)getValue(firstScreen,",",0).toInt();
  lPackages = getValue(firstScreen,",",1).toInt();
  pkgType = getValue(firstScreen,",",2).toInt();
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
