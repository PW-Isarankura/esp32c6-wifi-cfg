#include <WiFi.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

// ตัวแปรเก็บค่าที่อ่านจากไฟล์
String ssid = "";
String password = "";
int serverPort = 80;

// ฟังก์ชันโหลด Config จาก LittleFS
bool loadConfiguration() {
  File configFile = LittleFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config.json");
    return false;
  }

  // สร้าง Buffer สำหรับเก็บข้อมูล JSON
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, configFile);
  configFile.close();

  if (error) {
    Serial.println("Failed to parse config file");
    return false;
  }

  // ดึงค่ามาลงตัวแปร
  ssid = doc["wifi_ssid"].as<String>();
  password = doc["wifi_password"].as<String>();
  serverPort = doc["port"] | 80;

  Serial.println("--- Configuration Loaded ---");
  Serial.print("SSID: "); Serial.println(ssid);
  Serial.print("Port: "); Serial.println(serverPort);
  Serial.println("----------------------------");

  return true;
}

void setup() {
  // สำหรับ ESP32-C6: ต้องรอ Serial พร้อม (เปิด USB CDC On Boot ด้วย)
  Serial.begin(115200);
  while (!Serial) { delay(10); } 
  
  Serial.println("\nStarting ESP32-C6...");

  // 1. เริ่มต้น LittleFS (ใช้ partition ชื่อ "storage" ตาม CSV ของคุณ)
  // พารามิเตอร์แรกเป็น false เพื่อไม่ให้มันเผลอ format ทับไฟล์ที่เราอัปโหลดมา
  if (!LittleFS.begin(false, "/littlefs", 10, "storage")) {
    Serial.println("LittleFS Mount Failed! Please check partitions.csv or Upload Data.");
    return;
  }
  Serial.println("LittleFS Mounted.");

  // 2. อ่านไฟล์ Config
  if (!loadConfiguration()) {
    Serial.println("WiFi connection cannot proceed without config.");
    return;
  }

  // 3. เริ่มเชื่อมต่อ WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());

  Serial.print("Connecting to WiFi");
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi Connection Failed. Please check SSID/Password.");
  }
}

void loop() {
  // ใส่โค้ดของคุณที่นี่
}