////////////////////////////////////////////////////////INCLUDE////////////////////////////////////////

#include <SoftwareSerial.h>
#include <dht.h>                  // bibliothèque pour la température et humidité
#include <Wire.h>                 // cette bibliothèque permet de communiquer en I2C / TWI devices.
#include <Adafruit_BMP280.h>      // bibliothèque pour le module de pression
#include <Adafruit_SSD1306.h>     // bibliothèque pour pour écran
#include <Adafruit_CCS811.h>      // bibliothèque pour le capteur particule fines
#include <Adafruit_PM25AQI.h>     // bibliothèque particules fines 

////////////////////////////////////////////////////////DEFINE////////////////////////////////////////

#define OLED_WIDTH 128              // caracctéristique écran 
#define OLED_HEIGHT 64
#define OLED_RESET -1
#define BMP280_I2C_ADDRESS  0x76    // define pour le module de pression
#define pinMQ A3                    // Analog Pin sensor is connected to A3 ( capteur de gaz )
#define dht_apin A0                 // Analog Pin sensor is connected to A0 ( capteuer de température et humidité)

// declaration de varioble globale 
const int buzzer = 9; // pin du buzzer

////////////////////////////////////////////////INIT_DES_LIB////////////////////////////////////////////

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET); // init de l'écran 
Adafruit_BMP280  bmp280;                                              // init de library pour le module de pression 
dht DHT;                                                              // init pour DHT ( température ) 
Adafruit_CCS811 ccs;                                                  // init sensor particule fine

                
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

////////////////////////////////////////////////////////VOID_SETUP//////////////////////////////////////

void setup() {
  
  Serial.begin(9600);                                   // transmission série begin ( baud 9600 )
  Serial1.begin(9600);
  ccs.begin();

  pinMode(buzzer, OUTPUT);                              // pin mode pour le buzzer    
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);            // paramétrage de l'écran avec son adresse mac car 2 écran sont utilisés
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  
  if(!ccs.begin()){                               
    while(1);
  }
  while(!ccs.available());                              //attend que le sensor CS soit prêt 

  if (! aqi.begin_UART(&Serial1)) { // connect to the sensor over hardware serial
    while (1) delay(10);
  }         
}
////////////////////////////////////////////////////////VOID_LOOP///////////////////////////////////////////

void loop(){
  
////////////////////////////////////////////////////////DECLARATION_VAR////////////////////////////////////////
  
  float pressure = bmp280.readPressure();      
  float t = DHT.temperature;
  float h = DHT.humidity;
  int AP;
  int FG;
  int CO2;
  int TVOC;
  float PM1;
  float PM2_5;
  float PM10;
  
  
  ////////////////////////////////////////////////////////AFFICHAGE_DISPLAY////////////////////////////////////////
  
  DHT.read11(dht_apin);                       // on dit que le module est un capteur read11
  
  // humidité 
  display.setCursor(0, 0);
  display.print("Humidity : ");
  display.print(h);                           // valeur relative en pourcentage 
  display.print("%");
  
  // température_1
  display.setCursor(0, 15);
  display.print("Temperature : ");
  display.print(t);
  display.print("C");
  
  // gaz flamable
  display.setCursor(0, 30);
  display.print("Gaz inflammable: ");
  FG = analogRead(A3)/100;                    // mise en pourcentage de la valeur 
  display.print(FG);
  display.print("%");
  
  // polution d'air 
  display.setCursor(0, 45);
  display.print("Pollution d'air: ");
  AP = analogRead(A2)/10;                     // mise en pourcentage de la valeur 
  display.print(AP);
  display.print("%");
  display.display();
  delay(2500);
  display.clearDisplay();  
  
  if(ccs.available()){                        //  conditionel pour récuperer les valeurs CO2/TVOC 
    if(!ccs.readData()){
      CO2 = (ccs.geteCO2());
      TVOC = (ccs.getTVOC());
    }
  }


  //particule gaz 
  display.setCursor(0, 0);
  display.print("CO2: ");
  display.print(CO2);                   // mise en pourcentage de la value ( range max sensor 29206)
  display.print(" ppm");

  //particule fine
  display.setCursor(0, 15);
  display.print("TVOC: ");
  display.print(TVOC);                  // mise en pourcentage de la value ( range max sensor 32768)
  display.print(" ppb");
  
  // Pression
  display.setCursor(0, 30);
  display.print("pression :");
  display.print(pressure/100);
  display.println("  hPa");
  display.display();
  delay(2500);
  display.clearDisplay();

  PM25_AQI_Data data;                               // lance la prise de donnée pour le particule fine sensor
  
  if (! aqi.read(&data)) {
    delay(500);  // try again in a bit!
    return;
  }
  
  display.setCursor(0, 0);
  display.print("PM1: ");
  PM1 = data.particles_03um;
  display.print(PM1);                  
  display.print(" unit");
 
  display.setCursor(0, 15);
  display.print("PM2.5: ");
  PM2_5 = data.particles_05um;
  display.print(PM2_5);                  
  display.print(" unit");
  
  display.setCursor(0, 30);
  display.print("PM100: ");
  PM10 = data.particles_10um;
  display.print(PM10);                  
  display.print(" unit");

  display.display();
  delay(2500);
  display.clearDisplay();
  
////////////////////////////////////////////////ENVOI_SERIAL////////////////////////////////////////

  String Train = "";      // créarion de la String qui contient les valeurs des différents capteurs
  Train.concat(t);
  Train.concat("&");
  Train.concat(h);
  Train.concat("&");
  Train.concat(FG);
  Train.concat("&");
  Train.concat(AP);
  Train.concat("&");
  Train.concat(CO2);
  Train.concat("&");
  Train.concat(TVOC);
  Train.concat("&");
  Train.concat(PM1);
  Train.concat("&");
  Train.concat(PM2_5);
  Train.concat("&");
  Train.concat(PM10);
  Serial.println(Train);    // ordre train info = Temp/ hum/ gaz1/ gaz2/ particule1/ particule2 
  delay(50);            // delay qui va nous eviter la saturation du Serial sur l'ESP
  }