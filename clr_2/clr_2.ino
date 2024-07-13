#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

/* AP/Service */
const char* ssid = "Vodafone-0FF1";
const char* password = "Schnabeltier1";
const int port = 80;

ESP8266WebServer server(80);

/* RGB */
#define B_PIN 13
#define R_PIN 14
#define G_PIN 12
#define LED_Disc 5

int r = 0;
int g = 0;
int b = 0;
bool state;

void setup()
{
  Serial.begin(115200);
  
  pinMode(LED_Disc, OUTPUT);

  Off();
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    NotConnected();
    delay(500);
  }

  Connected();
  
  server.on("/rgb", HTTP_POST, handlePost);
  server.begin();
  state = true;
}

void loop(){
    server.handleClient();
}

void handlePost()
{
  /* await serial information */

  // output the current state of the esp8266 connection
  if(WiFi.status() == WL_CONNECTED)
  {
    Connected();
  }
  else
  {
    NotConnected();
  }

  // Check if a client has connected
  if (server.hasArg("plain") == false) 
  { 
    // Check if the body is present
    server.send(400, "application/json", "{\"error\":\"Bad Request\"}");
    return;
  }

  // Adjust the size based on your JSON structure
  String body = server.arg("plain");
  DynamicJsonDocument doc(1024); 

  DeserializationError error = deserializeJson(doc, body);
  if (error) 
  {
    server.send(400, "application/json", "{\"error\":\"Bad Request - Invalid JSON\"}");
    return;
  }

  // Check for the presence of the key
  if (doc.containsKey("color")) 
  {
    String values = doc["color"];

    // |R| |G| |B|
    // xxx xxx xxx

    String rString = (String)values[0] + (String)values[1] + (String)values[2];
    String gString = (String)values[3] + (String)values[4] + (String)values[5];
    String bString = (String)values[6] + (String)values[7] + (String)values[8];

    int r_loc = rString.toInt();
    int g_loc = gString.toInt();
    int b_loc = bString.toInt();

    if(r_loc == 0 && g_loc == 0 && b_loc == 0)
    {
        Off();
    }
    else
    {
        r = r_loc;
        b = b_loc;
        g = g_loc;

        analogWrite(R_PIN, r);
        analogWrite(G_PIN, g);
        analogWrite(B_PIN, b);
    }
  } 
  else if (doc.containsKey("state"))
  {
    String value = doc["state"];
    
    if(value[0] == 'O') // turn off the strip
    {
      Off();
    }
    if(value[0] == 'I') // turn on the strip
    {
      On();
    }
  }
  else
  {
    server.send(400, "application/json", "{\"error\":\"Key not found\"}");
  }

  server.send(200, "application/json", "{\"success\":\"Gucci\"}");
}

void On()
{
  analogWrite(R_PIN, r);
  analogWrite(G_PIN, g);
  analogWrite(B_PIN, b);
}

void Off()
{
  analogWrite(R_PIN, 0);
  analogWrite(G_PIN, 0);
  analogWrite(B_PIN, 0);
}

// blue when connected
void Connected()
{
  digitalWrite(LED_Disc, LOW);
}

// purple when not connected to wifi
void NotConnected()
{
  digitalWrite(LED_Disc, HIGH);
}