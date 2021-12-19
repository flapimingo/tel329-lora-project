
#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <SD.h> // for the SD card
#include <DHT.h> // for the DHT sensor
#include <RTClib.h> // for the RTC

//define DHT pin
#define DHTPIN 3     
#define DHTTYPE DHT22

// initialize DHT sensor for normal 16mhz Arduino
DHT dht (DHTPIN, DHTTYPE);

const int chipSelect = 5;

// Create a file to store the data
File myFile;
String Data;
double Temperature, Humidity;
String fileName = "data.txt";

// RTC
RTC_DS1307 rtc;

const long frequency = 915E6;  // LoRa Frequency

const int csPin = 10;          // LoRa radio chip select
const int resetPin = 9;        // LoRa radio reset
const int irqPin = 2;          // change for your board; must be a hardware interrupt pin

byte localAddress = 0xA1;
byte gatewayAddress = 0xFF;
byte requestData = 0xE0;
byte nodeSending = 0xE1;
byte nodeStopSending = 0xE2;

bool sendData = false;

void setup() {
  Serial.begin(9600);                   // initialize serial
  dht.begin();                          // initialize DHT
  while (!Serial);

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

  SD.remove(fileName);

  // Open file
  myFile = SD.open(fileName, FILE_WRITE);
  // If its opened, write to it
  if (myFile) {
    myFile.close();
  } 
  else {
    Serial.println("error opening data.txt");
  }

  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(frequency)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  Serial.println("LoRa init succeeded.");
  
  LoRa.onReceive(onReceive);
  LoRa.onTxDone(onTxDone);
  LoRa_rxMode();
}

void loop() {
  if (runEvery(5000)) { // repeat every 1000 millis
    getTemp();
    loggingData();
  }

  if(sendData) {
    String msg = "";
    // re-open the file for reading:
    myFile = SD.open(fileName);
    if (myFile) {
      Serial.println("test.txt:");
  
      // read from the file until there's nothing else in it:
      while (myFile.available()) {
        msg += (char)myFile.read();
      }
      Serial.println(msg);
      // close the file:
      myFile.close();
    } else {
      // if the file didn't open, print an error:
      Serial.println("error opening test.txt");
    }
    
    LoRa_sendMessage(msg); // send a message

    Serial.println("Send Message!");
  }
}

void LoRa_rxMode(){
  LoRa.enableInvertIQ();                // active invert I and Q signals
  LoRa.receive();                       // set receive mode
}

void LoRa_txMode(){
  LoRa.idle();                          // set standby mode
  LoRa.disableInvertIQ();               // normal mode
}

void LoRa_sendMessage(String message) {
  LoRa_txMode();                        // set tx mode
  LoRa.beginPacket();                   // start packet
  LoRa.write(localAddress);             // node address
  LoRa.write(nodeSending);              // message code
  LoRa.print(message);                  // add payload
  LoRa.endPacket(true);                 // finish packet and send it
  sendData = false;
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;

  byte sender = LoRa.read();            // sender address
  byte code = LoRa.read();              // message code

  if(sender != gatewayAddress) return;   // message is from another node
  
  String message = "";

  while (LoRa.available()) {
    message += (char)LoRa.read();
  }

  Serial.println("Node Receive: ");
  Serial.println("received from: 0x" + String(sender, HEX));
  Serial.println("message code: 0x" + String(code,HEX));
  Serial.println(message);

  if(code == requestData){
    sendData = true;
  }
}

void onTxDone() {
  Serial.println("TxDone");
  LoRa_rxMode();
}

boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}

void getTemp() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius
  Temperature = dht.readTemperature();
  // Read temperature as Fahrenheit
  //float f = dht.readTemperature(true);
  
  // Check if any reads failed and exit early (to try again).
  if  (isnan(Temperature) /*|| isnan(f)*/) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  //debugging purposes
  Serial.print("Temperature: "); 
  Serial.print(Temperature);
  Serial.println(" *C");
  //Serial.print(f);
  //Serial.println(" *F\t"); 
}

void loggingData() {
  DateTime now = rtc.now();
  myFile = SD.open(fileName, FILE_WRITE);
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
    myFile.print(Temperature);
    myFile.println(",");
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
