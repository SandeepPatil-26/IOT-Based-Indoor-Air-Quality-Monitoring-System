#define BLYNK_TEMPLATE_ID "TMPLwToQUqRw"
#define BLYNK_TEMPLATE_NAME "Air Quality Monitoring"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#include <DHT.h>
#include <Wire.h>

#include <LiquidCrystal_I2C.h> 

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address of your LCD

byte degree_symbol[8] = {
  0b00111,
  0b00101,
  0b00111,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "Your_WiFi_SSID";
char pass[] = "Your_WiFi_Password";

BlynkTimer timer;

const int gasPin = A0; // Analog pin connected to the gas sensor
int sensorThreshold = 1200; // Adjust threshold as needed

#define DHTPIN 2 // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void sendSensorData() {
  float h = dht.readHumidity();
  float t = dht.readTemperature(); 

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  int gasValue = analogRead(gasPin);
  Blynk.virtualWrite(V2, gasValue); 
  Serial.print("Gas Value: ");
  Serial.println(gasValue);

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" °C  Humidity: ");
  Serial.println(h);
}

void setup() {
  Serial.begin(115200);

  pinMode(gasPin, INPUT); 

  Blynk.begin(auth, ssid, pass);
  dht.begin();
  timer.setInterval(30000L, sendSensorData); // Corrected timer.setInterval() 

  lcd.begin(); 
  lcd.backlight(); 
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Air Quality");
  lcd.setCursor(0, 1);
  lcd.print("Monitoring");
  delay(2000);
  lcd.clear();
}

void loop() {
  Blynk.run();
  timer.run();

  float h = dht.readHumidity();
  float t = dht.readTemperature(); 
  int gasValue = analogRead(gasPin);

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(t);
  lcd.setCursor(5, 0);
  lcd.write(1); 
  lcd.createChar(1, degree_symbol); 
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Humid: ");
  lcd.print(h);
  lcd.print("%");

  delay(4000);
  lcd.clear();

  Serial.println("Gas Value:");
  Serial.println(gasValue);

  if (gasValue < sensorThreshold) {
    lcd.setCursor(0, 0);
    lcd.print("Gas Value: ");
    lcd.print(gasValue);
    lcd.setCursor(0, 1);
    lcd.print("Fresh Air");
    Serial.println("Fresh Air");
  } else if (gasValue >= sensorThreshold) {
    lcd.setCursor(0, 0);
    lcd.print("Gas Value: ");
    lcd.print(gasValue);
    lcd.setCursor(0, 1);
    lcd.print("Bad Air");
    Serial.println("Bad Air");

    // Send Blynk notification (optional)
    // Blynk.logEvent("pollution_alert", "Bad Air"); 
  }

  delay(4000); // Display on LCD for 4 seconds
  lcd.clear(); 
}