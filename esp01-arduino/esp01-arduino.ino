#include <Arduino_JSON.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h> // https://pubsubclient.knolleary.net/api
#include <time.h>
#include <TZ.h>

// Config
#include "secrets.h"
#define MYTZ TZ_Europe_Madrid
const int MQTT_PORT = 8883;
const bool DEBUG = true;
const int relayPin = 0;
const bool initialRelayOn = true;

WiFiClientSecure espClient;
PubSubClient client(espClient);

BearSSL::X509List awsCax509;
BearSSL::X509List rsaCertx509(AWS_CERT_CRT);
BearSSL::PrivateKey rsaCertpk(AWS_CERT_PRIVATE);

#define BUF_SIZE (512)
char buf[BUF_SIZE];

const short topicsLength = 8;
char topics[topicsLength][128];

char topicShadowGetAccepted[128];
char topicShadowUpdateAccepted[128];
char topicShadowGet[128];
char topicShadowUpdate[128];

void switchRelay(bool relayOn)
{
  if (relayOn)
  {
    if (DEBUG)
      Serial.println("Relay HIGH");
    digitalWrite(relayPin, HIGH);
  }
  else
  {
    if (DEBUG)
      Serial.println("Relay LOW");
    digitalWrite(relayPin, LOW);
  }
}

void switchRelayState (bool relayOn) {
  JSONVar reported;
  reported["on"] = relayOn;

  JSONVar state;
  state["reported"] = reported;

  JSONVar newShadow;
  newShadow["state"] = state;

  JSON.stringify(newShadow).toCharArray(buf, BUF_SIZE);
  Serial.println(buf);

  client.publish(topicShadowUpdate, buf);
}

void callback(char *topic, byte *payload, unsigned int length)
{
  // TODO: I'm not sure this is ok
  char payloadString[length];
  for (int i = 0; i < length; i++)
  {
    payloadString[i] = (char)payload[i];
  }

  if (DEBUG)
  {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    Serial.print(payloadString);
    Serial.println();
  }

  if (
      strcmp(topic, topicShadowGetAccepted) == 0 ||
      strcmp(topic, topicShadowUpdateAccepted) == 0)
  {
    JSONVar payloadJson = JSON.parse(payloadString);

    if (
        JSON.typeof(payloadJson) == "undefined" ||
        !payloadJson.hasOwnProperty("state") ||
        !payloadJson["state"].hasOwnProperty("reported") ||
        !payloadJson["state"]["reported"].hasOwnProperty("on"))
    {
      Serial.println("Parsing input failed: ");
      Serial.print("[");
      Serial.print(topic);
      Serial.print("] ");
      Serial.print(payloadString);
      Serial.println();
      return;
    }

    bool relayOn = (bool)payloadJson["state"]["reported"]["on"];
    switchRelay(relayOn);
  }
}

// Tries to connect back if either WiFi or MQTT are disconnected
void connect()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("\nConnecting to ");
    Serial.print(WIFI_SSID);
    Serial.print(" ...");

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }

    configTime(MYTZ, "pool.ntp.org");

    Serial.print("\nWiFi connected. IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("");
  }

  if (!client.connected())
  {
    Serial.print("\nConnecting to MQTT ...");
    while (!client.connected())
    {
      client.connect(THINGNAME);
      delay(500);
      Serial.print('(');
      Serial.print(client.state());
      Serial.print(')');
    }
    Serial.println("\nMQTT connected!");
    Serial.println("");

    for (int i = 0; i < topicsLength; i++)
    {
      client.subscribe(topics[i]);
    }

    // Get the current state
    client.publish(topicShadowGet, "");
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);

  // Add topics (https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-comms-device.htmls)
  sprintf(topics[0], "$aws/things/%s/shadow/delete/accepted", THINGNAME);
  sprintf(topics[1], "$aws/things/%s/shadow/delete/rejected", THINGNAME);
  sprintf(topics[2], "$aws/things/%s/shadow/get/accepted", THINGNAME);
  sprintf(topics[3], "$aws/things/%s/shadow/get/rejected", THINGNAME);
  sprintf(topics[4], "$aws/things/%s/shadow/update/accepted", THINGNAME);
  sprintf(topics[5], "$aws/things/%s/shadow/update/rejected", THINGNAME);
  sprintf(topics[6], "$aws/things/%s/shadow/update/delta", THINGNAME);
  sprintf(topics[7], "$aws/things/%s/shadow/update/documents", THINGNAME);

  sprintf(topicShadowGetAccepted, "$aws/things/%s/shadow/get/accepted", THINGNAME);
  sprintf(topicShadowUpdateAccepted, "$aws/things/%s/shadow/update/accepted", THINGNAME);
  sprintf(topicShadowGet, "$aws/things/%s/shadow/get", THINGNAME);
  sprintf(topicShadowUpdate, "$aws/things/%s/shadow/update", THINGNAME);

  // AWS IoT security/credentials
  awsCax509.append(AWS_CERT_CA1);
  awsCax509.append(AWS_CERT_CA3);
  espClient.setTrustAnchors(&awsCax509);
  espClient.setClientRSACert(&rsaCertx509, &rsaCertpk);

  client.setServer(AWS_IOT_ENDPOINT, MQTT_PORT);
  client.setCallback(callback);

  connect();
}

void loop()
{
  connect(); // Will try to connect back if either WiFi or MQTT is disconnected

  client.loop();
}
