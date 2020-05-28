#include <WiFi.h>
#include "ThingSpeak.h"

const char* WIFI_NAME = "yourssid";
const char* WIFI_PASSWORD = "password";
WiFiServer server(8888);

String header;

const int GPIO_PIN_NUMBER_22 = 22; // LEFT
const int GPIO_PIN_NUMBER_23 = 23; // RIGHT

// For ThingSpeak
const char* api_uploadIP = "api.thingspeak.com";
unsigned long myChannelNumber = 106xxxxx;
const char * myWriteAPIKey = "G4XXXXXXXXXXP";

void setup() {
  Serial.begin(115200);
  pinMode(GPIO_PIN_NUMBER_22, OUTPUT);
  pinMode(GPIO_PIN_NUMBER_23, OUTPUT);
  //pinMode(GPIO_PIN_NUMBER_15, OUTPUT);

  digitalWrite(GPIO_PIN_NUMBER_22, LOW);
  digitalWrite(GPIO_PIN_NUMBER_23, LOW);
  //digitalWrite(GPIO_PIN_NUMBER_15, LOW);


  Serial.print("Connecting to ");
  Serial.println(WIFI_NAME);
  WiFi.begin(WIFI_NAME, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Trying to connect to Wifi Network");
  }
  Serial.println("");
  Serial.println("Successfully connected to WiFi network");
  Serial.println("IP address: ");
  String localIPAddress = WiFi.localIP().toString();
  Serial.println(localIPAddress);
  server.begin();
  Serial.println("Server Started");

  // Initialize and write the local IP to things speak. This is required to get the local IP when PC is not connected.

   WiFiClient client2;
  ThingSpeak.begin(client2);  

  // set the fields with the values
  ThingSpeak.setField(2, localIPAddress);

  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }  
  Serial.println("Setup Completed");
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client is requesting web page");
    String current_data_line = "";
    while (client.connected()) {
      if (client.available()) {
        char new_byte = client.read();
        Serial.write(new_byte);
        header += new_byte;
        if (new_byte == '\n') {

          if (current_data_line.length() == 0)
          {

            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("MOVE=LEFT") != -1)
            {
              Serial.println("MOVE=LEFT");
              digitalWrite(GPIO_PIN_NUMBER_22, LOW);
              digitalWrite(GPIO_PIN_NUMBER_23, HIGH);
            }
            if (header.indexOf("MOVE=RIGHT") != -1)
            {
              Serial.println("MOVE=RIGHT");
              digitalWrite(GPIO_PIN_NUMBER_22, HIGH);
              digitalWrite(GPIO_PIN_NUMBER_23, LOW);
            }
            if (header.indexOf("MOVE=FORWARD") != -1)
            {
              Serial.println("MOVE=FORWARD");
              digitalWrite(GPIO_PIN_NUMBER_22, HIGH);
              digitalWrite(GPIO_PIN_NUMBER_23, HIGH);
            }
            if (header.indexOf("MOVE=STOP") != -1)
            {
              Serial.println("MOVE=STOP");
              digitalWrite(GPIO_PIN_NUMBER_22, LOW);
              digitalWrite(GPIO_PIN_NUMBER_23, LOW);
            }
            
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: 2px solid #4CAF50;; color: white; padding: 15px 32px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer; }");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            // Web Page Heading
            client.println("</style></head>");
            client.println("<body><center><h1>Driverless Car (Remote Driving!)</h1></center>");
            client.println("<center><h2>-Vaseef</h2></center>" );

            client.println("<form><center>");

            client.println("<center> <button class=\"button\" name=\"MOVE\" value=\"LEFT\" type=\"submit\">LEFT</button>") ;
            client.println("<button class=\"button\" name=\"MOVE\" value=\"RIGHT\" type=\"submit\">RIGHT</button><br><br>");
            
            client.println("<button class=\"button\" name=\"MOVE\" value=\"FORWARD\" type=\"submit\">FORWARD</button>");
            client.println("<button class=\"button\" name=\"MOVE\" value=\"STOP\" type=\"submit\">STOP</button> <br><br>");
            
            client.println("</center></form></body></html>");
            client.println();
            break;
          }
          else
          {
            current_data_line = "";
          }
        }
        else if (new_byte != '\r')
        {
          current_data_line += new_byte;
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
