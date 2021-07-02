#include <SPI.h>
#include <ESP8266WiFi.h>
#include<Adafruit_GFX.h>
#include<Adafruit_SSD1306.h>

char ssid[] = "wifi id";      
char pass[] = "password";  
const String myDevice = "deviceDefault@favoriot"; 
char server[] = "apiv2.favoriot.com";

int status = WL_IDLE_STATUS;
WiFiClient client;

#define TRIGGER 13
#define ECHO    15
long duration;
int distance;
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT,&Wire,OLED_RESET);

void setup() {
  Serial.begin (9600);
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  WiFi.disconnect();
  Serial.println("Connecting...");
  WiFi.begin(ssid,pass);
  
  while((!(WiFi.status() == WL_CONNECTED))){
    delay(300);
    Serial.print(".");
  }
  
  Serial.println(WiFi.status());
  Serial.println("Connected!");
  Serial.println("");
   if(!display.begin(SSD1306_SWITCHCAPVCC,0x3c))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);//Dont proceed 
  }
display.display();
delay(2);
display.clearDisplay();
display.clearDisplay();
display.setTextColor(WHITE);
display.setTextSize(2);
display.setCursor(0,5);
display.print("Ultrasonic");
display.display();
delay(3000);
}

void loop() {
  
  //=============================sensor function here==============================
  // Clears the trigPin
  long duration, distance;
  digitalWrite(TRIGGER, LOW);  
  delayMicroseconds(2); 
  
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(TRIGGER, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = (duration/2) / 29.1;
 
  Serial.print("Distance:");
  Serial.print(distance);
  Serial.println(" cm");
  delay(1000);
  
  //=============================oled display======================================
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10,0);
  display.print("Distance:");
  display.setTextSize(3);
  display.setCursor(30,30);
  display.print(distance);
  display.print("cm");
  display.display();

  //=========================json favoriot data send ==================================================
  String json = "{\"device_developer_id\":\""+myDevice+"\",\"data\":{\"Alert Distance:\":\""+distance+"\"}}";
  Serial.println(json);
  if (client.connect(server, 80)) {
    client.println("POST /v2/streams HTTP/1.1");
    client.println("Host: apiv2.favoriot.com");

    client.println(F("apikey:rw-apikey/accesstoken"));  // change it!
    
    client.println("Content-Type: application/json");
    client.println("cache-control: no-cache");
    client.print("Content-Length: ");
    int thisLength = json.length();
    client.println(thisLength);
    client.println("Connection: close");
    client.println();
    client.println(json);
  }
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
  if (!client.connected()) {
    client.stop();
  }
  delay(5000);
}
