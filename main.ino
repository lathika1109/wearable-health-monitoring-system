#include <LiquidCrystal.h>

const int LCD_RS = 7;
const int LCD_EN = 8;
const int LCD_D4 = 9;
const int LCD_D5 = 10;
const int LCD_D6 = 11;
const int LCD_D7 = 12;
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

const int LM35_PIN  = A0;
const int HW827_PIN = A1;
const int AD8232_PIN = A2;

const float ADC_RESOLUTION = 4095.0;
const float ADC_REF_VOLTAGE = 3.3;

const int PULSE_THRESHOLD = 550;
unsigned long lastBeatTime = 0;
unsigned long peakInterval = 0;
bool pulseDetected = false;
int bpm = 0;

unsigned long lastDisplayUpdate = 0;
const unsigned long DISPLAY_INTERVAL = 2000;

void setup() {
  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Health Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();
}

void loop() {
  detectHeartbeat();

  if (millis() - lastDisplayUpdate >= DISPLAY_INTERVAL) {
    lastDisplayUpdate = millis();

    float temperature = readTemperature();
    int ecgValue = readECG();

    displayReadings(bpm, temperature, ecgValue);
    printToSerial(bpm, temperature, ecgValue);
  }
}

float readTemperature() {
  int adcValue = analogRead(LM35_PIN);
  float voltage = (adcValue / ADC_RESOLUTION) * ADC_REF_VOLTAGE;
  float temperatureC = voltage / 0.01;
  return temperatureC;
}

int readECG() {
  int adcValue = analogRead(AD8232_PIN);
  return adcValue;
}

void detectHeartbeat() {
  int sensorValue = analogRead(HW827_PIN);

  if (sensorValue > PULSE_THRESHOLD && !pulseDetected) {
    pulseDetected = true;
    unsigned long currentTime = millis();

    if (lastBeatTime != 0) {
      peakInterval = currentTime - lastBeatTime;
      if (peakInterval > 0) {
        bpm = 60000 / peakInterval;
      }
    }
    lastBeatTime = currentTime;
  }

  if (sensorValue < PULSE_THRESHOLD) {
    pulseDetected = false;
  }
}

void displayReadings(int heartRate, float temperature, int ecg) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("HR:");
  lcd.print(heartRate);
  lcd.print("BPM");

  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C E:");
  lcd.print(ecg);
}

void printToSerial(int heartRate, float temperature, int ecg) {
  Serial.print("Heart Rate: ");
  Serial.print(heartRate);
  Serial.print(" BPM\t");

  Serial.print("Temperature: ");
  Serial.print(temperature, 2);
  Serial.print(" C\t");

  Serial.print("ECG Value: ");
  Serial.println(ecg);
}
