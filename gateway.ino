/*
  This is a simple example show the Heltec.LoRa recived data in OLED.

  The onboard OLED display is SSD1306 driver and I2C interface. In order to make the
  OLED correctly operation, you should output a high-low-high(1-0-1) signal by soft-
  ware to OLED's reset pin, the low-level signal at least 5ms.

  OLED pins to ESP32 GPIOs via this connecthin:
  OLED_SDA -- GPIO4
  OLED_SCL -- GPIO15
  OLED_RST -- GPIO16
  
  by Aaron.Lee from HelTec AutoMation, ChengDu, China
  成都惠利特自动化科技有限公司
  www.heltec.cn
  
  this project also realess in GitHub:
  https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series
*/
#include "heltec.h" 
#include "images.h"
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Seraph";
const char* password = "gtwwlsluiz";
const char* url = "http://192.168.0.105:8080";

#define BAND    915E6  //you can set band here directly,e.g. 868E6,915E6,433E6
String rssi = "RSSI --";
String packSize = "--";
String packet;

int senderId;
float distance;

void logo(){
  Heltec.display->clear();
  Heltec.display->drawXbm(0,5,logo_width,logo_height,logo_bits);
  Heltec.display->display();
}

void WIFISetup(){
  WiFi.disconnect(true);
  delay(100);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(true);
  WiFi.begin("Seraph", "gtwwlsluiz");
  delay(100);

  byte count = 0;
  while(WiFi.status() != WL_CONNECTED && count < 10){
    count ++;
    delay(500);
    Heltec.display -> drawString(0, 0, "Connecting...");
    Heltec.display -> display();
  }

  Heltec.display -> clear();
  if(WiFi.status() == WL_CONNECTED){
    Heltec.display -> drawString(0, 0, "Connecting... OK");
    Heltec.display -> display();
  }
  else{
    Heltec.display -> clear();
    Heltec.display -> drawString(0, 0, "Connecting... Failed");
    Heltec.display -> display();
  }
  Heltec.display -> drawString(0, 10, "WIFI Setup done");
  Heltec.display -> display();
  delay(5000);
}

void LoRaData(){
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0 , 15 , "Received "+ packSize + " bytes");
  Heltec.display->drawStringMaxWidth(0 , 26 , 128, packet);
  Heltec.display->drawString(0, 0, rssi);  
  Heltec.display->display();
}

void cbk(int packetSize) {
  packet ="";
  packSize = String(packetSize,DEC);
  for (int i = 0; i < packetSize; i++) { packet += (char) LoRa.read(); }
  rssi = "RSSI " + String(LoRa.packetRssi(), DEC) ;
  LoRaData();
}

void setup() { 
//  Serial.begin(115200);
  
   //WIFI Kit series V1 not support Vext control
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_10);
  logo();
  delay(1500);
  Heltec.display->clear();

  WIFISetup();
 
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Heltec.LoRa Initial success!");
  Heltec.display->drawString(0, 10, "Wait for incoming data...");
  Heltec.display->display();
  delay(1000);
//  LoRa.onReceive(cbk);
  LoRa.receive();
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    int receivedRssi = LoRa.packetRssi();
    float receivedSnr = LoRa.packetSnr();    
    while(LoRa.available()){
      String aux;
      aux = LoRa.readStringUntil('|');
      senderId = aux.toInt();
      aux = LoRa.readStringUntil('|');
      distance = aux.toFloat();
    }

    Heltec.display->clear();
    Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(0 , 0 , "RECEIVED PACKET");
    Heltec.display->drawString(0 , 10 , "RSSI: "+ String(receivedRssi) +"    SNR: "+ String(receivedSnr) + " dB");
    Heltec.display->drawString(0 , 20 , "Sender ID: "+ String(senderId));
    Heltec.display->drawString(0 , 30, "Distance: " + String(distance));
    Heltec.display->display();
  
    Heltec.display->clear();
    if(WiFi.status() == WL_CONNECTED){
      WiFiClient client;

      if(!client.connect("192.168.0.105", 8080)){
        Heltec.display->drawString(0, 30, "Connection Failed!");
        Heltec.display->display();  
        return;
      }

      String content = "{\"id\":" + String(senderId) + ",\"distance\":"+ String(distance) +"}";
      client.println("POST / HTTP/1.1");
      client.println("Host: 192.168.0.105");
      client.println("User-Agent: Arduino/1.0");
      client.println("Content-Length: "+String(content.length()));
      client.println();
      client.println(content);
      client.println();
    }
    else{
      Heltec.display->drawString(0, 10, "WiFi disconnected!");
      Heltec.display->display();
    }
  }
  delay(1000);
}
