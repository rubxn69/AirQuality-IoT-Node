#include <WiFi.h>
#include <HTTPClient.h>
#include <SDS011.h>
#include <SoftwareSerial.h>
#include <ThingSpeak.h>
#include <DHT.h>  // Include the DHT library

#define RX_PIN 14 // GPIO14 connected to TX of SDS011
#define TX_PIN 12 // GPIO12 connected to RX of SDS011

#define DHTPIN 15      // Pin connected to the DHT11 data pin
#define DHTTYPE DHT11  // Define the sensor type (DHT11 in this case)

DHT dht(DHTPIN, DHTTYPE);  // Initialize DHT sensor   

SoftwareSerial sdsSerial(RX_PIN, TX_PIN); // Define SoftwareSerial for SDS011
SDS011 my_sds;

// WiFi credentials
const char* ssid = "your ssid";
const char* password = "your password";

// ThingSpeak settings
const char * myWriteAPIKey = "your thingspeak write api key";
unsigned long myChannelNumber = your thingspeak channel id ;        // Replace with your ThingSpeak Channel ID

WiFiClient client;
  

  float p10, p25; // Variables to store PM10 and PM2.5 values

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
 
   dht.begin(); 
  
  // Initialize SoftwareSerial for SDS011
  sdsSerial.begin(9600);

  // Initialize SDS011 sensor with SoftwareSerial
  my_sds.begin(RX_PIN, TX_PIN);

  Serial.println("SDS011 Sensor is initializing...");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.println("IP address: " + WiFi.localIP().toString());

  // Initialize AHT10/AHT20 sensor
 
  ThingSpeak.begin(client);
}

void loop() {
  int err;

   // Reading temperature and humidity
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

    // Check if any reads failed and exit early (NaN indicates failure)
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // Read PM2.5 and PM10 values
  err = my_sds.read(&p25, &p10);

  if (!err) {
    // If no error, print the values to the Serial Monitor
    Serial.println("P2.5: " + String(p25) + " µg/m³");
    Serial.println("P10:  " + String(p10) + " µg/m³");
  } else {
    Serial.println("Error reading SDS011 data!");
  }

  delay(1000); 
  
 
  // Print the readings to the Serial Monitor
  Serial.print("Temperature: "); 
  Serial.print(temperature); 
  Serial.println(" degrees C");
  
  Serial.print("Humidity: "); 
  Serial.print(humidity); 
  Serial.println("% rH");

  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, p25);
  ThingSpeak.setField(4, p10);

 int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  // Wait before sending the next data
  delay(10000);  // Delay of 10 seconds, 
