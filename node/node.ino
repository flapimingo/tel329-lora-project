#include "SD.h"
#include <SPI.h>
#include "DHT.h" 

 #define DHTPIN 2     
 #define DHTTYPE DHT22
 DHT dht (DHTPIN, DHTTYPE) ;

File myFile;
String Temperature, Humidity, Data;

void setup() {
  Serial.begin(115200);
  dht.begin (  ) ;  

  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  myFile = SD.open("data.txt", FILE_WRITE);
  if (myFile) {
    myFile.println( "Temperature(°C), Humidity(%) \r\n");
    myFile.close();
  } 
  else {
    Serial.println("error opening data.txt");
  }
}

void loop() {
   if ( isnan (dht.readTemperature ( ) ) || isnan (dht.readHumidity ( ) ) )
   {
     Serial.println ("DHT22 Sensor not working!") ;
   }
 else
{   
  data_logging();                                                 
 }
delay(20000); 
}

void data_logging() 
{
  String Temperature = String(dht.readTemperature ( ), 2);
  String Humidity = String(dht.readHumidity ( ), 2);
  Data = Temperature + "," + Humidity;
  
  Serial.print("Save data: ");
  Serial.println(Data);
  
  myFile = SD.open("data.txt", FILE_WRITE);
  if (myFile) {
    Serial.print("Writing to data.txt...");
    myFile.println(Data);
    myFile.close();
    Serial.println("done.");
  } else {
    Serial.println("error opening data.txt");
  }  
  Serial.println();
}
