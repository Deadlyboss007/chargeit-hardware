#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Malay"
#define WIFI_PASSWORD "Malay123"

// Insert Firebase project API Key
#define API_KEY "AIzaSyBZO16nXmK4rXMYWX7JQT3fvsDgJGdbSOU"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://charge-it-8b148-default-rtdb.asia-southeast1.firebasedatabase.app/" 
#define IR_PIN D1
#define RED_LED D2
#define GREEN_LED D3

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int status=0;
bool signupOK = false;

void setup(){
  Serial.begin(115200);
  pinMode(IR_PIN,INPUT);
  pinMode(RED_LED,OUTPUT);
  pinMode(GREEN_LED,OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    if(status==1)
    {
      digitalWrite(RED_LED,HIGH);
      digitalWrite(GREEN_LED,LOW);
    }
        if(status==0)
    {
      digitalWrite(RED_LED,LOW);
      digitalWrite(GREEN_LED,HIGH);
    }
   status=digitalRead(IR_PIN);
    if (Firebase.RTDB.setInt(&fbdo, "VALUE/INT", status)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    Serial.printf("status");
    Serial.print(status);

  }
}