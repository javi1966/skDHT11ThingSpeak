#include <ESP8266SSDP.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <ESP8266WiFi.h> // ESP8266WiFi.h library


#define DHTPIN       2
#define DHTTYPE      DHT11
#define LED          0
#define LEDERROR     14

const int timerSleep= 30* 60* 1000000; //30 minutos
const int timerUpdate= 2 * 60 ; //30 minutos
const char ssid[]     = "xxxxxxxxx";
const char password[] = "xxxxxxxxx";
const char* host = "api.thingspeak.com";
bool toggle = false;
bool bActualiza = true;
float humidity;
float temperature;
int   presion;

String writeAPIKey = "TQVK604D9AF8ZIG2";

DHT dht(DHTPIN, DHTTYPE, 15);
Adafruit_BMP085 bmp;

WiFiClient client;

void setup() {
  // Initialize sensor

  Serial.begin(115200);
  delay(100);
  Serial.println(" ");
  Serial.println("Iniciando ...");

  pinMode(LED, OUTPUT);
  pinMode(LEDERROR,OUTPUT);
  pinMode(16, WAKEUP_PULLUP);


  //  Connect to WiFi network
  WiFi.begin(ssid, password);

  int timeout=0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("*");

    if(++timeout > 100)
    {
      
       Serial.println("Sin Conexion WIFI");
       int a=25;
       while (a--) { digitalWrite(LED, HIGH);delay(100);digitalWrite(LED,LOW);delay(100);}
       Serial.println("Reset SW");
       ESP.reset();
       
    }
    
  }


  dht.begin();
  if (!bmp.begin()) {
        Serial.println("No conecto BMP085 sensor");
      while (1) { digitalWrite(LED, LOW);delay(400); digitalWrite(LED, HIGH);delay(400);}
  }
  delay(1000);

  digitalWrite(LED, HIGH);

/*
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(TimingISR);
  timer0_write(ESP.getCycleCount() + 80000000L); // 80MHz == 1sec
  interrupts();
*/ 
  humidity    = dht.readHumidity();
  temperature = dht.readTemperature();
  presion     = bmp.readPressure() ;  //
  
   if (client.connect(host, 80)) {

      // Construct API request body
      String body = "field1=";
      body +=  String(temperature);
      body += "&field2=";
      body += String(humidity);
      body += "&field3=";
      body += String(presion+881);



      Serial.println(body);

      client.print("POST /update HTTP/1.1\n");
      client.print("Host: api.thingspeak.com\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: " + writeAPIKey + "\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
      client.print(body.length());
      client.print("\n\n");
      client.print(body);
      client.print("\n\n");

    }

    client.stop();

    digitalWrite(LED, LOW); //flashing led
    delay(300);
    digitalWrite(LED, HIGH);
    Serial.println("ESP8266 in sleep mode");
    
    ESP.deepSleep(timerSleep);

}

void loop() {
/*

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  //Serial.println(String(temperature));  //DEBUG
  //Serial.println(String(humidity));

 
    //if (isnan(humidity) || isnan(temperature)) {
   //  return;
   

  // make TCP connections

  if (bActualiza) {

    bActualiza = false;

    if (client.connect(host, 80)) {

     

      // Construct API request body
      String body = "field1=";
      body +=  String(temperature);
      body += "&field2=";
      body += String(humidity);



      Serial.println(body);

      client.print("POST /update HTTP/1.1\n");
      client.print("Host: api.thingspeak.com\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: " + writeAPIKey + "\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
      client.print(body.length());
      client.print("\n\n");
      client.print(body);
      client.print("\n\n");

    }

    client.stop();

    digitalWrite(LED, LOW); //flashing led
    delay(300);
    digitalWrite(LED, HIGH);
    
    ESP.deepSleep(timerSleep);
  }

 */
    
}

void TimingISR() {
  
  static int cntTemp = 0;
  
  if (++cntTemp > timerUpdate)
  {
    bActualiza = true;
   
    cntTemp = 0;
  }
  timer0_write(ESP.getCycleCount() + 80000000L); // 80MHz == 1sec
  
}
