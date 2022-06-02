////////////////////////////////////////////////////////INCLUDE////////////////////////////////////////

#include "thingProperties.h"      // propriété dans l'IOT Arduino
#include <SoftwareSerial.h>
#include <Adafruit_SSD1306.h>     //library pour écran
#include <Wire.h>                 // cette bibliothèque permet de communiquer en I2C / TWI devices.

////////////////////////////////////////////////////////DEFINE////////////////////////////////////////

#define OLED_WIDTH 128            // caracctéristique écran 
#define OLED_HEIGHT 64
#define OLED_RESET -1

////////////////////////////////////////////////////////////////////////////////////////////////////
// declaration de varioble globale
String split;
String Ser;
String W;

////////////////////////////////////////////////INIT_DES_LIB////////////////////////////////////////////

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET); // init de l'écran

/////////////////////////////////////////////////VOID_SETUP////////////////////////////////////////

void setup() {

  Serial.begin(9600);                                 // transmission série begin ( baud 9600 )

  initProperties();                                   // Defined in thingProperties.h
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);  // Connect to Arduino IoT Cloud
  
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);          // paramétrage de l'écran avec son adresse mac car 2 écran sont utilisés
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  while (!Serial) {                                   // attente du port serie / reste dans la boucle while sinon
    ;
  }
}
////////////////////////////////////////////////////////VOID_LOOP///////////////////////////////////////////

void loop() {

  ArduinoCloud.update();                  // renvoie sur le Cloud Arduino les valeurs des variables (mise à jours)

  if (Serial.available() > 0) {           // verification de la connection Serie
    split = Serial.readString();          // lecture de l'arrivage série de la mega, train d'info en String

    String Ser = "connected";                    // renvoi l'etat de la connection serie OK/OFF
    display.setCursor(0, 0);
    display.print("Etat_Serie: ");
    display.print(Ser);

  }
  else
  {
    String Ser = "OFF";
    display.setCursor(0, 0);
    display.print("Etat_Serie: ");
    display.print(Ser);
  }


  if (WiFi.status() != WL_CONNECTED) {    // verification de la connection Wifi
    String W = "OFF";              // renvoi l'etat de la connection wifi /disconnect
    display.setCursor(0, 15);
    display.print("wifi: ");
    display.print(W);
  }
  else
  {
    String W = "connected";                 // renvoi l'etat de la connection wifi connecte/
    display.setCursor(0, 15);
    display.print("wifi: ");
    display.print(W);
  }
  if (ArduinoCloud.connected() == 0) {              // renvoi l'etat de la connection serveur
    String serveur = "OFF";
    display.setCursor(0, 30);
    display.print("Serveur: ");
    display.print(serveur);
  }else{
    String serveur = "connected";
    display.setCursor(0, 30);
    display.print("Serveur: ");
    display.print(serveur);
  }
  
  display.display();                      // attente de 2 sec / clear de l'écran
  delay(2000);
  display.clearDisplay();
  
  


  //// on sépare la String et récupère la variable qui nous interresse//////

  String t = getValue(split, '&', 0);
  String h = getValue(split, '&', 1);
  String fg = getValue(split, '&', 2);
  String ap = getValue(split, '&', 3);
  String co = getValue(split, '&', 4);
  String tv = getValue(split, '&', 5);
  String p1 = getValue(split, '&', 6);
  String p25 = getValue(split, '&', 7);
  String p100 = getValue(split, '&', 8);


  //// on convertie la String récuperer en Float //////

  float T = t.toFloat();          // température
  temperature = T;

  float H = h.toFloat();          // humidité
  humidity = H;

  float FG = fg.toFloat();        // gaz inflammable
  gaz_inflamable = FG;

  float AP = ap.toFloat();        // pollution d'air
  pollution_air = AP;

  float CO = co.toFloat();        // CO2
  CO2 = CO;

  float TV = tv.toFloat();        // TVOC
  TVOC = TV;

  float P1 = p1.toFloat();        // PM1
  pM1 = P1;

  float P25 = p25.toFloat();        // PM25
  pM25 = P25;

  float P100 = p100.toFloat();        // PM1000
  pM100 = P100;

  //// Affichage sur le display des valeurs récuperer //////
  /*
  display.clearDisplay();           // 1er affichage d'écran
  display.setCursor(0, 0);
  display.print(H);                //humidité de l'air
  display.setCursor(0, 13);
  display.println(T);               //temperature
  display.setCursor(0, 26);
  display.print(AP);                // pollution de l'air
  display.setCursor(0, 39);
  display.println(FG);               // gaz inflammable
  display.setCursor(0, 52);
  display.display();                // affichage des print
  delay(3500);                      // delay
  display.clearDisplay();           // clear de l'écran

  display.setCursor(0, 0);          // 2em affichage d'écran
  display.print(CO2);               // taux de CO2
  display.setCursor(0, 13);
  display.println(TVOC);
  display.setCursor(0, 26);
  display.print(pM1);               // particule fine 1/25/100
  display.setCursor(0, 39);
  display.println(pM25);
  display.setCursor(0, 52);
  display.print(pM100);
  display.display();                // affichage des print
  delay(3500);                      // delay
  display.clearDisplay();           // clear de l'écran
  */
}// me peremet de verifier l'etat des données reçu par l'etat série ( non nécessaire au code final )


//////// Fonction de séparation de String avec séparateur ///////////

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
