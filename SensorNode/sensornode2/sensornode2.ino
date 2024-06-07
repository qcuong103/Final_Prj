  /* 
modified on Dec 29, 2020
Modified by MehranMaleki from Arduino Examples
Home
*/


//Libraries
#include <DHT.h>;
#include "MQ7.h"

//Constants
#define MQ7_PIN 2
#define VOLTAGE 5
#define DHTPIN 4            //what pin we're connected to
#define DHTTYPE DHT21       //DHT 21  (AM2301)
DHT dht(DHTPIN, DHTTYPE);   //Initialize DHT sensor for normal 16mhz Arduino
MQ7 mq7(MQ7_PIN, VOLTAGE);

//Variables
float hum;  //Stores humidity value
float temp; //Stores temperature value

void setup()
{
  Serial.begin(9600);
  while (!Serial) {
		;	// wait for serial connection
	}
  Serial.println(""); 	// blank new line
  dht.begin();
  Serial.println("Calibrating MQ7");
	mq7.calibrate();		// calculates R0
	Serial.println("Calibration done!");
}

void loop()
{
  //Read data and store it to variables hum and temp
  hum = dht.readHumidity();
  temp= dht.readTemperature();
  
  //Print temp and humidity values to serial monitor
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print("%,  Temperature: ");
  Serial.print(temp);
  Serial.println(" Celsius");
  Serial.print("PPM = "); Serial.println(mq7.readPpm());
  Serial.println(analogRead(MQ7_PIN));
  Serial.println(""); 	// blank new line
  
  delay(2000); //Delay 2 sec.
}
