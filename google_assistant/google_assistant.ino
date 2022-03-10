//remote switch 
//adafruit,goodgle assistant and ifttt platfroms
//switch on/off the lights using voice remotely
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <ESP8266WiFi.h>

#define Relay            16// set relay pin to D0

#define WLAN_SSID       "NEVEREST"             // Your SSID
#define WLAN_PASS       "nikoworks"        // Your password

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com" //connects to the adafruit server
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME  "ciku" // Replace it with your username
#define AIO_KEY       "aio_XcQE74AZQJxB8P1Z9KVgNAJzrkK6" // Replace with your Project Auth Key got from the adafruit platform


/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/


// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe Light = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/light"); // FeedName of the project you want to perform

void MQTT_connect();

void setup() {
  Serial.begin(115200);

  pinMode(Relay, OUTPUT);//defines the relay pin as the output

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); //gets the ip adress of the network you are operating in
  Serial.println(WiFi.localIP());
 

  // Setup MQTT subscription for on/off feed.
  mqtt.subscribe(&Light);//subscription for light on/off
}

void loop() {
 //connecting to the IFTTT applets via MQTT
  MQTT_connect();
  
  Adafruit_MQTT_Subscribe *subscription;//checks for the applet in the IFTTT/adafruit platform that correcsponds to the action and project you want to perform
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &Light) {
      Serial.print(F("Got: "));//reads the value received from the adafruit toggle switch where 0 refers to light OFF and 1 refers to light ON
      Serial.println((char *)Light.lastread);//reads the value from adafruit and checks the character from adafruit that comes in as a string
      int Light_State = atoi((char *)Light.lastread);//checks for the last state of the bulb and waits for the next command via google assistant;converts the string into integer
      digitalWrite(Relay,!(Light_State));//toggles the state of the relay according to the data received in the adafruit server
      delay(2000);//waits for 2 seconds
    }
  }
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;//try 3 times incase MQTT is not connected
  
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds before it tries to reconnect tpo mqtt again
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me;checks for any other networks available,connects to them;gets the ip address 
      while (1);
    }
  }
  Serial.println("MQTT Connected!");//once ip adress is received,it reconnects to mqtt and returns connected
  
}
