/****************************************************
 *                  INCLUDES
 ****************************************************/
#include <HTTPClient.h>

#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

#include <Firebase_ESP_Client.h>

// Firebase helpers
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

/****************************************************
 *               WIFI CREDENTIALS
 ****************************************************/
#define WIFI_SSID "XXXXXXXXXX"
#define WIFI_PASSWORD "XXXXXXXXXX"

/****************************************************
 *               FIREBASE CONFIG
 ****************************************************/
// Firebase API Key
#define API_KEY "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

// Firebase RTDB URL
#define DATABASE_URL "https://xxx-xxxx-xxxx-default-rtdb.asia-southeast1.firebasedatabase.app/"

/****************************************************
 *               GLOBAL OBJECTS
 ****************************************************/
HTTPClient HttpClient;

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

/****************************************************

 *               GLOBAL VARIABLES
 ****************************************************/
int sampling   = 250;   // Number of samples for averaging
int testnotif  = 0;     // Flag to send notification once
int mVperAmp   = 1850;  // Sensor sensitivity (ACS712)
int ACSoffset  = 2500;  // Sensor voltage offset (mV)
int signupok   = 0;     // Firebase signup status

/****************************************************
 *         SEND FIREBASE CLOUD NOTIFICATION
 ****************************************************/
void sendNotification(String title, String body) {

  String DEVICE_FCM_KEY = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
  String FCM_API_KEY    = "key=XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
  String FCM_URL        = "https://fcm.googleapis.com/fcm/send";

  // Check WiFi connection
  if (WiFi.status() == WL_CONNECTED) {

    HttpClient.begin(FCM_URL);

    // HTTP headers
    HttpClient.addHeader("Authorization", FCM_API_KEY);
    HttpClient.addHeader("Content-Type", "application/json");

    // Notification payload
    String data =
      "{\"registration_ids\": [\"" + DEVICE_FCM_KEY +
      "\"], \"notification\": {\"body\":\"" + body +
      "\", \"title\":\"" + title + "\"}}";

    // Send POST request
    int httpCode = HttpClient.POST(data);

    // Check response
    if (httpCode == HTTP_CODE_OK) {
      Serial.println("Notification Sent To The Phone");
    } else {
      Serial.println("Error on sending notification");
      Serial.println(httpCode);
      Serial.println(HttpClient.getString());
    }

    HttpClient.end(); // Free resources
  }
}

/****************************************************
 *                WIFI SETUP
 ****************************************************/
void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Wait for WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

/****************************************************
 *              FIREBASE SETUP
 ****************************************************/
void setup_firebase() {

  // Assign Firebase credentials
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Anonymous sign-up
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase signup OK");
    signupok = 1;
  } else {
    Serial.printf("Signup error: %s\n",
      config.signer.signupError.message.c_str());
  }

  // Token status callback
  config.token_status_callback = tokenStatusCallback;

  // Start Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

/****************************************************
 *                    SETUP
 ****************************************************/
void setup() {
  Serial.begin(115200);
  setup_wifi();
  setup_firebase();
}

/****************************************************
 *                     LOOP
 ****************************************************/
void loop() {

  // Check Firebase readiness
  if (!Firebase.ready() && signupok == 0) {
    Serial.println("Firebase not connected...");
    delay(1000);
    return;
  }

  // Send notification only once
  if (testnotif == 0) {
    sendNotification("Measuring", "from sensors");
    testnotif = 1;
  }

  double rawValue = 0;  // Raw ADC value
  double V = 0;         // Voltage in mV
  double Amps = 0;      // Current in Amps
  double tens = 0;      // Voltage in Volts

  // Read sensor multiple times
  for (int i = 0; i < sampling; i++) {
    rawValue += analogRead(34);
    delay(1);
  }

  rawValue /= sampling;

  // Convert ADC value to voltage
  V = (rawValue / 4095.0) * 5000 * 3; // mV
  Amps = (V - ACSoffset) / mVperAmp;
  tens = V / 1000;

  // Debug output
  Serial.print("Voltage: ");
  Serial.println(tens);
  Serial.print("Current: ");
  Serial.println(Amps);

  // Send data to Firebase
  if (Firebase.RTDB.setFloat(&firebaseData, "projectdata/current", Amps) &&
      Firebase.RTDB.setFloat(&firebaseData, "projectdata/voltage", tens)) {
    Serial.println("Data sent to Firebase successfully");
  } else {
    Serial.println("Error sending data to Firebase");
  }

  delay(1000);
}




