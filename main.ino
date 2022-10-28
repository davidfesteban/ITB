#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//JUST CHANGE THE NAME
const String name = "altaer";

// WiFi
const char *ssid = "CP5-WPA4530-V1"; // Enter your WiFi name
const char *password = "TalineMirco9902";  // Enter WiFi password

// Sensor
const int air_value = 577;
const int water_value = 158;
const int sensor_pin = A0;

// MQTT Broker
const String mqtt_broker = "mqtt.naivetardis.de";

// MQTT publish
const String topic_reading = "office/plants/"+name+"/reading";
const String topic_heartbeat = "office/plants/heartbeat";

// MQTT subscribed
const String topic_water = "office/plants/"+name+"/water";
const String topic_setting = "office/plants/"+name+"/setting";

const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  connectWifi();
  connectMqtt();
}

void loop() {
  client.publish(topic_heartbeat, name);
  client.publish(topic_reading, makePlantDrySensorReading());
  client.loop();
  delay(2000);
}

String makePlantDrySensorReading() {
  double resultPercent = 0;
  unsigned int sensor_value = analogRead(sensor_pin) - water_value;
  int max_value = air_value - water_value;

  resultPercent = (sensor_value * 100)/(double)max_value;
  if(resultPercent > 100) {
    resultPercent = 100;
  }

  return String(resultPercent);
}

void connectWifi() {
  // Set software serial baud to 9600bps. (115200);
  Serial.begin(115200);
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");  
}

void connectMqtt() {
  //connecting to a mqtt broker
  client.set_server(mqtt_broker, mqtt_port);
  //client.set_callback(callback);
  while (!client.connected()) {
      String client_id = "IoT-"+name+"-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str())) {
          Serial.println("Public naivetardis mqtt connected");
      } else {
          Serial.print("failed with state ");
          //Serial.print(client.state());
          delay(2000);
      }
  }  
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
      Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}
