#include <SPI.h>
#include <Ethernet.h>
String readString;
const int launchPin = 11;

byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0F, 0x17, 0x20
};
IPAddress ip(192, 168, 1, 100);
EthernetServer server(80);

void setup() {
  Serial.begin(9600);

  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  pinMode(launchPin, OUTPUT);
}


void loop() {
  
  EthernetClient client = server.available();

  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        if (readString.length() < 100) {
          //store characters to string
          readString += c;
        }

        if (c == '\n') {
          
          client.println("HTTP/1.1 200 OK"); //send new page
          client.println("Content-Type: text/html");
          client.println();
          client.println("{\"status\":\"ready\"}");

          delay(1);
          client.stop();

          if (readString.indexOf("?launch") > 0) {
            digitalWrite(launchPin, HIGH);
            delay(500);
            digitalWrite(launchPin, LOW);
          }

          readString = ""; //clearing string for next read
        }
      }
    }
  }
}

