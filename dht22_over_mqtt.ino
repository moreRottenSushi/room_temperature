#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "DHT.h"
#define DHTPIN D2    // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors


const char* ssid = "ssdi";
const char* password =  "pass";
IPAddress mqttServer(192, 168, 2, 22);
const int mqttPort = 3453;
char server[] = "www.google.com";
float temperaturImRaum = 0.0;
char tir[50];
String tir_str;

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);


void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();
  Serial.println("-----------------------");

}


void setup() {

  Serial.begin(115200);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(2, OUTPUT);

  // Wait for serial to initialize.
  while (!Serial) { }

  Serial.println("Device Started");

  //setup wifi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  // if you get a connection, report back via serial:
  if (espClient.connect(server, 80)) {
    Serial.println("connected to Wifi");
    // Make a HTTP request:
    //espClient.println("GET /search?q=arduino HTTP/1.1");
    //espClient.println("Host: www.google.com");
    //espClient.println("Connection: close");
    //espClient.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }


  //setup mqtt
  Serial.println("Connecting to MQTT...");

  client.setServer("192.168.2.22", 3453);
  client.setCallback(callback);
  delay(1000);

}

void reconnect() {
  Serial.println("reconnect");
  client.disconnect();
  delay(5000);
  while (!client.connected() and client.state() != 0) {
    Serial.println("connected_0");

    if (client.connect("ESP8266Client")) {

      Serial.println("connected_1");

    } else {

      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(2000);

    }
  }
  client.subscribe("temp");
}

void loop() {
  while ( client.state() != 0) {
    reconnect();
  }
  client.loop();


  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  Serial.println("new values:");
  Serial.println(t);
  Serial.println(h);


  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second


  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    timeSinceLastRead = 0;
    return;
  }


  tir_str = "Temperatur (°C): " + String(t) + " Luftfeuchte (rel.): " + String(h);
  tir_str.toCharArray(tir, 50);
  if (client.publish("temp", tir, true)) {
    Serial.println("worked");
  } else {
    Serial.println("failed");
  }









}
