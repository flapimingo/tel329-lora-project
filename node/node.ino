#include <SPI.h> //for the SD card module
#include <SD.h> // for the SD card
#include <DHT.h> // for the DHT sensor
#include <RTClib.h> // for the RTC

//define DHT pin
#define DHTPIN 3     
#define DHTTYPE DHT22

// initialize DHT sensor for normal 16mhz Arduino
DHT dht (DHTPIN, DHTTYPE);

const int chipSelect = 10; 

// Create a file to store the data
File myFile;
String Temperature, Humidity, Data;

// RTC
RTC_DS1307 rtc;

void setup() {
  Serial.begin(9600);
  // Initializing the DHT sensor
  dht.begin() ;  

  while(!Serial); // for Leonardo/Micro/Zero
  if(! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  else {
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  if(! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
  }

  // Setup for the SD card
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  // Open file
  myFile = SD.open("data.txt", FILE_WRITE);
  // If its opened, write to it
  if (myFile) {
    myFile.println( "Temperature(°C), Humidity(%) \r\n");
    myFile.close();
  } 
  else {
    Serial.println("error opening data.txt");
  }
}

void loggingTime() {
  DateTime now = rtc.now();
  myFile = SD.open("DATA.txt", FILE_WRITE);
  if (myFile) {
    myFile.print(now.year(), DEC);
    myFile.print('/');
    myFile.print(now.month(), DEC);
    myFile.print('/');
    myFile.print(now.day(), DEC);
    myFile.print(',');
    myFile.print(now.hour(), DEC);
    myFile.print(':');
    myFile.print(now.minute(), DEC);
    myFile.print(':');
    myFile.print(now.second(), DEC);
    myFile.print(",");
  }
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.println(now.day(), DEC);
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);
  myFile.close();
  delay(1000);  
}

void loggingTemperature() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  //float f = dht.readTemperature(true);
  
  // Check if any reads failed and exit early (to try again).
  if  (isnan(t) /*|| isnan(f)*/) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  //debugging purposes
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.println(" *C");
  //Serial.print(f);
  //Serial.println(" *F\t"); 
  
  myFile = SD.open("DATA.txt", FILE_WRITE);
  if (myFile) {
    Serial.println("open with success");
    myFile.print(t);
    myFile.println(",");
  }
  myFile.close();
}

void loop() {
  loggingTime();
  loggingTemperature();
  delay(5000);
}
