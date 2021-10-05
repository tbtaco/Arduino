#include <WiFi101.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

int device_number = 0;

const int pin_led_1 = 7;
const int pin_led_2 = 6;
const int pin_led_3 = 5;
const int device_num_input = 9;
// MOSI on 11, MISO on 12, SCK on 13
const int wifi_cs = 10;

const int loopDelay = 400; //us

bool led1On = false;
bool led2On = false;
bool led3On = false;

const char* ssid = "SSID Here";
const char* pass = "Password Here";

const char* server = "Server IP Here";
const int port = 0000;

char* clientName = "Metro Mini Device #";

//ex: mosquitto_sub -h localhost -p 0000 -t /# -u Username -P Password
//ex: mosquitto_pub -h localhost -p 0000 -t /Test -u Username -P Password -m 'Testing Testing 1 2 3'

const char* username = "Username";
const char* password = "Password";

char* inTopic = "/#";

void callback(char* topic, byte* payload, unsigned int length)
{
  char msg[length];
  for(int i = 0; i < length; i++)
    msg[i] = (char)payload[i];

  //publishMessage("/TestResponse", "Testing");

  Serial.println("Message with topic \""+(String)topic+"\" received.  No actions taken.");
}

WiFiClient wifiClient;
PubSubClient client(server, port, callback, wifiClient);

void setup()
{
  Serial.begin(9600);
  delay(100);
  Serial.setTimeout(100);
 
  pinMode(pin_led_1, OUTPUT);
  pinMode(pin_led_2, OUTPUT);
  pinMode(pin_led_3, OUTPUT);
  pinMode(device_num_input, INPUT);

  if(digitalRead(device_num_input))
    device_number = 1;

  Serial.println("Device Number Is: "+(String)device_number);

  //wifiClient.setCACert(ca);

  Serial.print("Starting WiFi...");

  WiFi.setPins(10, 8, 4, -1); //cs, irq, rst, en (-1)

  WiFi.begin(ssid, pass);

  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.println(" Complete");

  Serial.print("Connecting To Client...");
  while(!client.connect(clientName, username, password))
  {
    Serial.print(".");
    delay(100);
  }
  Serial.println(" Complete");

  client.subscribe(inTopic);
}
void loop()
{
  client.loop();
  delayMicroseconds(loopDelay);
}
void publishMessage(char* topic, char* msg)
{
  client.publish(topic, msg);
}
