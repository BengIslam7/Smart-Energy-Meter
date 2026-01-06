# Smart-Energy-Meter
# 📡 ESP32 Firebase IoT Power Monitoring System

This project is an **ESP32-based IoT power monitoring system** that measures **voltage and current using an ACS712 sensor**, sends the data to **Firebase Realtime Database**, and triggers **Firebase Cloud Messaging (FCM) push notifications** to a mobile device.

---

## 🚀 Features

* ✅ WiFi connection (ESP32 / ESP8266 compatible)
* 🔌 Current & voltage measurement using **ACS712**
* 📊 Real-time data upload to **Firebase RTDB**
* 🔔 Push notifications using **Firebase Cloud Messaging (FCM)**
* 📉 Signal averaging for accurate sensor readings
* 🛠 Modular and easy-to-extend firmware

---

## 🧰 Hardware Requirements

| Component    | Description                     |
| ------------ | ------------------------------- |
| ESP32        | Main microcontroller            |
| ACS712       | Current sensor (5A / 20A / 30A) |
| Power Source | According to your setup         |
| Jumper Wires | Connections                     |
| Breadboard   | Optional                        |

**ADC Pin Used:**

```
GPIO 34 (ESP32)
```

---

## 🧪 Software & Libraries

### Required Arduino Libraries

Install these from **Arduino Library Manager**:

* `Firebase ESP Client`
* `HTTPClient`
* `WiFi` (ESP32) / `ESP8266WiFi`

---

## 🔐 Firebase Configuration

1. Create a Firebase project
2. Enable **Realtime Database**
3. Set database rules (for testing):

```json
{
  "rules": {
    ".read": true,
    ".write": true
  }
}
```

4. Get:

   * **API Key**
   * **Database URL**
   * **FCM Server Key**
   * **Device FCM Token**

---

## ⚙️ Configuration (Code)

Update these values in the code:

```cpp
#define WIFI_SSID "YOUR_WIFI"
#define WIFI_PASSWORD "YOUR_PASSWORD"

#define API_KEY "YOUR_FIREBASE_API_KEY"
#define DATABASE_URL "YOUR_FIREBASE_DATABASE_URL"
```

FCM credentials inside:

```cpp
sendNotification(String title, String body)
```

---

## 📐 Sensor Calibration

Adjust according to your ACS712 module:

```cpp
int mVperAmp = 1850;   // 5A module
int ACSoffset = 2500; // Sensor offset in mV
```

| ACS712 Model | mVperAmp |
| ------------ | -------- |
| 5A           | 1850     |
| 20A          | 100      |
| 30A          | 66       |

---

## 🔁 Data Flow

```
ACS712 Sensor
      ↓
ESP32 ADC (GPIO34)
      ↓
Signal Averaging
      ↓
Voltage & Current Calculation
      ↓
Firebase RTDB
      ↓
Mobile App (FCM Notification)
```

---

## 📊 Firebase Database Structure

```json
projectdata {
  "current": 1.23,
  "voltage": 5.01
}
```

---

## 🖥 Serial Monitor Output

```
Voltage: 5.01
Current: 1.23
Data sent to Firebase successfully
Notification Sent To The Phone
```

---

## 🛡 Security Notes (Important)

⚠️ **For production use:**

* Do NOT hardcode API keys
* Use Firebase Authentication
* Enable HTTPS/TLS only
* Restrict RTDB rules
* Store secrets securely

---
