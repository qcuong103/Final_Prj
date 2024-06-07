


//Libraries
#include <DHT.h>;
#include "MQ7.h"
#include <SPI.h>
#include <LoRa.h>

//Constants
#define BAND 433E6

#define ss 10
#define rst 9
#define dio0 2

// #define MQ7_PIN A2
// #define MQ7_PINA 2
#define VOLTAGE 5
#define DHTPIN 4            //what pin we're connected to
#define DHTTYPE DHT21       //DHT 21  (AM2301)
DHT dht(DHTPIN, DHTTYPE);   //Initialize DHT sensor for normal 16mhz Arduino
// MQ7 mq7(MQ7_PINA, VOLTAGE);

//Variables
float hum;  //Stores humidity value

float temp; //Stores temperature value

String LoRaMessage = "";

char device_id[8] = "Node001";

int readingID = 0;
 
int counter = 0;
 
float temperature = 0;

float humidity = 0;



void startLoRA()
{
  
  LoRa.setPins(ss, rst, dio0); //setup LoRa transceiver module

  while (!LoRa.begin(433E6) && counter < 10) {

    Serial.print(".");

    counter++;

    delay(500);

  }

  if (counter == 50) 
  
  {
    // Increment readingID on every new reading
    
    readingID++;

    Serial.println("Starting LoRa failed!");
    while (1);    
  }

  Serial.println("LoRa Initialization OK!");

  delay(2000);

}

void startDHT()
{
  
  if (isnan(humidity) || isnan(temperature)) 
  
  {

  Serial.println("Failed to read from DHT sensor!");

  return;
  
  }
}

void getReadings(){
  humidity = dht.readHumidity();
  
  temperature = dht.readTemperature();

  Serial.print(F("Humidity: "));

  Serial.print(humidity);

  Serial.print(F("%  Temperature: "));

  Serial.print(temperature);

  Serial.println(F("°C "));

}

void sendReadings() {
  
  LoRaMessage = String(readingID) + "/" + String(temperature) + "&" + String(humidity) ;
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  
  Serial.println(F("LoRa.beginPacket"));
  
  LoRa.print(LoRaMessage);
  
  Serial.println(F("LoRa.print"));
  
  LoRa.endPacket();  
  
  Serial.println(F("LoRa.endPacket"));
  
  Serial.print("Sending packet: ");

  Serial.println(readingID);

  readingID++;

  Serial.println(LoRaMessage);
  
}

void setup()
{
  Serial.begin(9600);
  
  while (!Serial) {
		;	// wait for serial connection
	}
  Serial.println(F("LoRa Sender"));
  SPI.begin();  
  dht.begin();
  startDHT();
  startLoRA();
}


void loop()
{
  getReadings();
  
  sendReadings();
  
  delay(5000); //Delay 2 sec.
}
