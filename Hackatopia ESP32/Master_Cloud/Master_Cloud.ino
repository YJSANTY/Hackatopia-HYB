#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <SoftwareSerial.h>
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <Arduino.h>
/********* WiFi Access Point ***********/
#define WLAN_SSID       "alpha"
#define WLAN_PASS       "23572357"

/********* Adafruit.io Setup ***********/
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "yj_santy"
#define AIO_KEY         "aio_DGrH902CBh8BvgEXcXTJO6ZnucIg"

/********* Mesh_Network Setup ***********/
#include <iostream>
#include <string>
using namespace std;
#define   MESH_SSID       "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

/**** Global State (you don't need to change this!) ******/
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/********** Feeds *************/
// ... [Your feed definitions remain unchanged]
// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish mqtt_humidity_1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity-1");
Adafruit_MQTT_Publish mqtt_temperature_1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temp-1");
Adafruit_MQTT_Publish mqtt_ldr_1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ldr-1");
Adafruit_MQTT_Publish mqtt_soil_1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/moist-1");

Adafruit_MQTT_Publish mqtt_humidity_2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity-2");
Adafruit_MQTT_Publish mqtt_temperature_2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature-2");
Adafruit_MQTT_Publish mqtt_ldr_2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ldr-2");
Adafruit_MQTT_Publish mqtt_soil_2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/moisture-2");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe onoffbutton_1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/valve-1");
Adafruit_MQTT_Subscribe onoffbutton_2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/valve-2");

/********* Sketch Code ************/
void MQTT_connect();

#define Valve_1 4  // Adjust as per your ESP32 board's GPIO
#define Valve_2 5  // Adjust as per your ESP32 board's GPIO

SoftwareSerial sim800lSerial(13, 15); // RX (SIM800L) to GPIO13, TX (SIM800L) to GPIO15


#define TXD2 17
#define RXD2 16

String kal="zzz";
struct {
  int temp_1 = 0;
  int humi_1 = 0;
  int ldr_1  = 0;
  int soil_1 = 0;
  int temp_2 = 0;
  int humi_2 = 0;
  int ldr_2  = 0;
  int soil_2 = 0;
}alpha ;


String temp_1,humi_1,ldr_1,soil_1,temp_2,humi_2,ldr_2,soil_2;
char c;
String dataIn;
int indexofA,indexofB,indexofC,indexofD,indexofE,indexofF,indexofG,indexofH;


void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) {
    return;
  }
  
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);
    retries--;
    
    if (retries == 0) {
      while (1);
    }
  }
  
  Serial.println("MQTT Connected!");
}

void mqtt_code() {
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoffbutton_1) {
      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton_1.lastread);
      
      if (0 == strcmp((char *)onoffbutton_1.lastread, "0")) {
        digitalWrite(Valve_1, HIGH);
      }
      if (0 == strcmp((char *)onoffbutton_1.lastread, "1")) {
        digitalWrite(Valve_1, LOW);
      }
    }
    else if (subscription == &onoffbutton_2) {
      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton_2.lastread);
      
      if (0 == strcmp((char *)onoffbutton_2.lastread, "0")) {
        digitalWrite(Valve_2, HIGH);
      }
      if (0 == strcmp((char *)onoffbutton_2.lastread, "1")) {
        digitalWrite(Valve_2, LOW);
      }
    }
  }
  
  if (1) {
    for (int i = 0; i < 13; i++) Serial.print("-"); Serial.print("Data");
    for (int i = 0; i < 13; i++) Serial.print("-"); Serial.println();
    Serial.print("Humidity_1:"); Serial.println(alpha.humi_1);
    Serial.print("Temperature_1:"); Serial.println(alpha.temp_1);
    Serial.print("LDR_1:"); Serial.println(alpha.ldr_1);
    Serial.print("Soil_1:"); Serial.println(alpha.soil_1);
    Serial.print("Humidity_2:"); Serial.println(alpha.humi_2);
    Serial.print("Temperature_2:"); Serial.println(alpha.temp_2);
    Serial.print("LDR_2:"); Serial.println(alpha.ldr_2);
    Serial.print("Soil_2:"); Serial.println(alpha.soil_2);
    
    for (int i = 0; i < 30; i++) Serial.print("-"); Serial.println();
  }



 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print(alpha.humi_1);
 lcd.print("%");
 lcd.setCursor(8, 0);
 lcd.print(alpha.temp_1);
 lcd.print("C");
 lcd.setCursor(0, 1);
 lcd.print("L:");
 lcd.print(alpha.ldr_1);
 lcd.setCursor(8, 1);
 lcd.print("S:");
 lcd.print(alpha.soil_1);
 lcd.print("%");

 delay(3000);

 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print(alpha.humi_2);
 lcd.print("%");
 lcd.setCursor(8, 0);
 lcd.print(alpha.temp_2);
 lcd.print("C");
 lcd.setCursor(0, 1);
 lcd.print("L:");
 lcd.print(alpha.ldr_2);
 lcd.setCursor(8, 1);
 lcd.print("S:");
 lcd.print(alpha.soil_2);
 lcd.print("%");

 delay(3000);
 
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Valve_1 : ");
 if (0 == strcmp((char *)onoffbutton_1.lastread, "0")) {
       lcd.print("OFF");
      }
if (0 == strcmp((char *)onoffbutton_1.lastread, "1")) {
       lcd.print("ON");
      }

 lcd.setCursor(0, 1);
 lcd.print("Valve_2 : ");

if (0 == strcmp((char *)onoffbutton_2.lastread, "0")) {
       lcd.print("OFF");
      }
if (0 == strcmp((char *)onoffbutton_2.lastread, "1")) {
       lcd.print("ON");
      }
  //Send Serial
  
  mqtt_humidity_1.publish(alpha.humi_1);    delay(3000);
  mqtt_temperature_1.publish(alpha.temp_1); delay(3000);
  mqtt_ldr_1.publish(alpha.ldr_1);          delay(3000);
  mqtt_soil_1.publish(alpha.soil_1);        delay(3000);
  
  mqtt_humidity_2.publish(alpha.humi_2);    delay(3000);
  mqtt_temperature_2.publish(alpha.temp_2); delay(3000);
  mqtt_ldr_2.publish(alpha.ldr_2);          delay(3000);
  mqtt_soil_2.publish(alpha.soil_2);        delay(3000);

}



void seperate()
{
 if(kal[0]== 't')
  {
    alpha.temp_1 = (kal.substring(1)).toInt();
  }
  else if(kal[0]=='l')
  {
    alpha.ldr_1 = (kal.substring(1)).toInt();
  }
  else if(kal[0]=='s')
  {
    alpha.soil_1 = (kal.substring(1)).toInt();
  }  
  else if(kal[0]=='h')
  {
    alpha.humi_1 = (kal.substring(1)).toInt();
  }
  else if(kal[0]=='e')
  {
    alpha.temp_2 = (kal.substring(1)).toInt();
  }
  else if(kal[0]=='d')
  {
    alpha.ldr_2 = (kal.substring(1)).toInt();
  }
  else if(kal[0]=='o')
  {
    alpha.soil_2 = (kal.substring(1)).toInt();
   }
  else if(kal[0]=='u'){
    alpha.humi_2 = (kal.substring(1)).toInt();
    }
}


// Function to send an SMS
void sendSMS(const char* phoneNumber, const char* message) {
  sim800lSerial.print("AT+CMGF=1"); // Set SMS mode to text
  delay(100);
  
  sim800lSerial.print("AT+CMGS=\""); // Command to send SMS
  sim800lSerial.print(phoneNumber);
  sim800lSerial.println("\"");
  
  delay(100);
  sim800lSerial.print(message); // Message content
  delay(100);
  
  sim800lSerial.write(26); // Ctrl+Z to send message
}

// Function to make a call
void makeCall(const char* phoneNumber) {
  sim800lSerial.print("ATD"); // ATD command to dial a number
  sim800lSerial.println(phoneNumber);
}

void read_values()
{ 
  
  if (Serial2.available()) {
    char dataChar = Serial2.read();

    if (dataChar == 'h') {
      int temp_1 = Serial2.parseInt();
      int humi_1 = Serial2.parseInt();
      int ldr_1 = Serial2.parseInt();
      int soil_1 = Serial2.parseInt();
      int temp_2 = Serial2.parseInt();
      int humi_2 = Serial2.parseInt();
      int ldr_2 = Serial2.parseInt();
      int soil_2 = Serial2.parseInt();

      // Print received data
      Serial.println("Received Data:");
      Serial.print("Temp 1: "); Serial.println(temp_1);
      Serial.print("Humidity 1: "); Serial.println(humi_1);
      Serial.print("LDR 1: "); Serial.println(ldr_1);
      Serial.print("Soil 1: "); Serial.println(soil_1);
      Serial.print("Temp 2: "); Serial.println(temp_2);
      Serial.print("Humidity 2: "); Serial.println(humi_2);
      Serial.print("LDR 2: "); Serial.println(ldr_2);
      Serial.print("Soil 2: "); Serial.println(soil_2);
      Serial.println("--------------------------");
    }
  }
}


void  Parse_the_Data()
{
  indexofA = dataIn.indexOf("A");
  indexofB = dataIn.indexOf("B");
  indexofC = dataIn.indexOf("C");
  indexofD = dataIn.indexOf("D");
  indexofE = dataIn.indexOf("E");
  indexofF = dataIn.indexOf("F");
  indexofG = dataIn.indexOf("G");
  indexofH = dataIn.indexOf("H");
  
 temp_1 = dataIn.substring(0,indexofA);
 humi_1 = dataIn.substring(indexofA + 1,indexofB);
 ldr_1 = dataIn.substring(indexofB + 1,indexofC);
 soil_1 = dataIn.substring(indexofC + 1,indexofD);
 temp_2 = dataIn.substring(indexofD + 1,indexofE);
 humi_2 = dataIn.substring(indexofE + 1,indexofF);
 ldr_2 = dataIn.substring(indexofF + 1,indexofG);
 soil_2 = dataIn.substring(indexofG + 1,indexofH);

 alpha.temp_1 = temp_1.toInt();
 alpha.humi_1 = humi_1.toInt();
 alpha.ldr_1 = ldr_1.toInt();
 alpha.soil_1 = soil_1.toInt();
 alpha.temp_2 = temp_2.toInt();
 alpha.humi_2 = humi_2.toInt();
 alpha.ldr_2 = ldr_2.toInt();
 alpha.soil_2 = soil_2.toInt();
}


void setup() {
  Serial.begin(115200);
  Serial2.begin(4800, SERIAL_8N1, RXD2, TXD2);
  delay(10);
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Program Start");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);

  sim800lSerial.begin(9600);
  delay(2000);
  sim800lSerial.println("ATE0");
  delay(100);
  sim800lSerial.println("AT+CNMI=0,0,0,0,0");
  
  lcd.print("Connecting...");
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  
  mqtt.subscribe(&onoffbutton_1);
  mqtt.subscribe(&onoffbutton_2);
    
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connected");
  
  pinMode(Valve_1, OUTPUT);
  digitalWrite(Valve_1, HIGH);
  pinMode(Valve_2, OUTPUT);
  digitalWrite(Valve_2, HIGH);
}

void loop() {
  read_values(); 
  mqtt_code();
  //delay(20000);
    makeCall("+123456789"); // Replace with the desired phone number
  
  delay(5000); // Delay before next operation
  
  // Example: Send SMS
  sendSMS("+123456789", "Hello, this is a test message!"); // Replace with the desired phone number and message
  
  delay(5000); // Delay before next operation
}
