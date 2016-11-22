/**
 * Send OSC messages from NodeMCU to another OSC speaking device.
 * Case: Press a physical button (connected to NodeMCU) and get informed about it on your smartphone screen
 * Created by Fabian Fiess in November 2016
 * Inspired by Oscuino Library Examples, Make Magazine 12/2015
 */
 
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>                // for sending OSC messages
#include <OSCBundle.h>                 // for receiving OSC messages

char ssid[] = "difix";                 // your network SSID (name)
char pass[] = "88288828";              // your network password

// Button Input + LED Output
const int btnPin1 = 12;                // D6 pin at NodeMCU
const int ledPin = 14;                 // D5 pin at NodeMCU
const int boardLed = LED_BUILTIN;      // Builtin LED

boolean btnChanged1 = false;
int btnVal1 = 1;  

WiFiUDP Udp;                           // A UDP instance to let us send and receive packets over UDP
const IPAddress destIp(192,168,0,3);    // remote IP of the target device
const unsigned int destPort = 9000;    // remote port of the target device where the NodeMCU sends OSC to
const unsigned int localPort = 8000;   // local port to listen for UDP packets at the NodeMCU (another device must send OSC messages to this port)

void setup() {
    Serial.begin(115200);

     // Specify a static IP address for NodeMCU - only needeed for receiving messages)
     // If you erase this line, your ESP8266 will get a dynamic IP address
     WiFi.config(IPAddress(192,168,0,123),IPAddress(192,168,0,1), IPAddress(255,255,255,0)); 
  
    // Connect to WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    Serial.println("Starting UDP");
    Udp.begin(localPort);
    Serial.print("Local port: ");
    Serial.println(Udp.localPort());

    // btnInput + LED Output
    pinMode(btnPin1, INPUT);
    pinMode(ledPin, OUTPUT);
    pinMode(boardLed, OUTPUT); 
}

void loop() {
    // read btnInput and send OSC
    OSCMessage msgOut("/1/buttonListener");

    if(digitalRead(btnPin1) != btnVal1) {
      btnChanged1 = true;
      btnVal1 = digitalRead(btnPin1);
    }

    if(btnChanged1 == true){
      btnChanged1 = false;
      digitalWrite(ledPin, btnVal1);
      digitalWrite(boardLed, (btnVal1 + 1) % 2);    // strange, but for the builtin LED 0 means on, 1 means off
      Serial.print("Button: ");
      Serial.println(btnVal1);
      msgOut.add(btnVal1);
    }

    Udp.beginPacket(destIp, destPort);
    msgOut.send(Udp);
    Udp.endPacket();
    msgOut.empty();
    delay(100);
}