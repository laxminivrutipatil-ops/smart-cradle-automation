// ============================================================
// SMART CRADLE AUTOMATION SYSTEM (UPDATED VERSION)
// ESP32 DOIT DEVKIT V1 - Arduino Framework
// ============================================================

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <HardwareSerial.h>

// ---------------- PINS ----------------
#define MOISTURE_PIN    34    // Analog input pin for moisture sensor
#define MOTOR_PIN       18    // Controls the cradle swinging motor
#define SWITCH_PIN      14    // Physical switch to control motor (Internal Pull-Up)
#define LED_ALERT_PIN   12    // Visual indicator for wetness alerts

// Hardware Serial 2 pins for DFPlayer Mini MP3 Module
#define MP3_RX_PIN      16    // Connects to TX of DFPlayer Mini
#define MP3_TX_PIN      17    // Connects to RX of DFPlayer Mini (use 1k resistor)

// ---------------- CONSTANTS ----------------
const int WET_THRESHOLD = 2500;            // Moisture threshold value (0-4095)
const unsigned long CHECK_INTERVAL = 2000; // Sensor sampling interval (2 seconds)

// ---------------- OBJECTS ----------------
LiquidCrystal_I2C lcd(0x27, 16, 2);
HardwareSerial mp3Serial(2);               // Using ESP32 UART2 channel

// ---------------- STATE VARIABLES ----------------
unsigned long lastCheckTime = 0;
bool isWetDetected = false;
volatile bool motorState = false;          // Maintained via physical toggle switch
unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 250;  // Debounce time in milliseconds

// ---------------- DFPLAYER COMMANDS ----------------
// Standard hexadecimal command sequence array for DFPlayer Mini initialization
byte playTrack1Cmd[] = {0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x01, 0xFE, 0xF7, 0xEF}; 
byte setVolumeCmd[]  = {0x7E, 0xFF, 0x06, 0x06, 0x00, 0x00, 0x1E, 0xFE, 0xD7, 0xEF}; // Volume level 30

void sendMp3Command(byte cmd[], int len) {
  for (int i = 0; i < len; i++) {
    mp3Serial.write(cmd[i]);
  }
}

// ---------------- INTERRUPT SERVICE ROUTINE (ISR) ----------------
// Handles physical switch toggle to turn cradle motor on/off instantly
void IRAM_ATTR handleMotorSwitch() {
  unsigned long currentTime = millis();
  if (currentTime - lastDebounceTime > DEBOUNCE_DELAY) {
    motorState = !motorState;
    lastDebounceTime = currentTime;
  }
}

void setup() {
  Serial.begin(115200);
  mp3Serial.begin(9600, SERIAL_8N1, MP3_RX_PIN, MP3_TX_PIN);
  
  // Pin Configurations
  pinMode(MOISTURE_PIN, INPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(LED_ALERT_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  
  // Attach interrupt to physical switch pin (detects when button is pressed to GND)
  attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), handleMotorSwitch, FALLING);
  
  // Initialize default components states
  digitalWrite(MOTOR_PIN, LOW);
  digitalWrite(LED_ALERT_PIN, LOW);
  
  // Setup display hardware
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("CRADLE INITIAL");
  delay(1500);
  
  // Initialize default volume level on startup
  sendMp3Command(setVolumeCmd, 10);
  lcd.clear();
}

void loop() {
  unsigned long currentTime = millis();
  
  // 1. Manage swinging motor output based on physical switch override or automation
  if (motorState || isWetDetected) {
    digitalWrite(MOTOR_PIN, HIGH);
  } else {
    digitalWrite(MOTOR_PIN, LOW);
  }
  
  // 2. Non-blocking sensor evaluation routine
  if (currentTime - lastCheckTime >= CHECK_INTERVAL) {
    lastCheckTime = currentTime;
    
    int moistureValue = analogRead(MOISTURE_PIN);
    Serial.print("Moisture Value: ");
    Serial.println(moistureValue);
    
    if (moistureValue > WET_THRESHOLD) {
      if (!isWetDetected) { // Triggers exactly once when status changes to wet
        isWetDetected = true;
        sendMp3Command(playTrack1Cmd, 10); // Play notification alert audio track
      }
      displayAlert(moistureValue);
    } else {
      isWetDetected = false;
      displaySafe();
    }
  }
}

void displayAlert(int value) {
  digitalWrite(LED_ALERT_PIN, HIGH);
  lcd.setCursor(0, 0);
  lcd.print("STATUS: WET ALERT");
  lcd.setCursor(0, 1);
  lcd.print("MOTOR: ");
  lcd.print((digitalRead(MOTOR_PIN) == HIGH) ? "ON  " : "OFF ");
}

void displaySafe() {
  digitalWrite(LED_ALERT_PIN, LOW);
  lcd.setCursor(0, 0);
  lcd.print("STATUS: DRY SAFE ");
  lcd.setCursor(0, 1);
  lcd.print("MOTOR: ");
  lcd.print((digitalRead(MOTOR_PIN) == HIGH) ? "ON  " : "OFF ");
}
