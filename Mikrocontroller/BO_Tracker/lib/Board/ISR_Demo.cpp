#include <RTCZero.h>

// Erstelle RTCZero-Objekt
RTCZero rtc;

// Variable zur Speicherung des Interrupt-Zustands
volatile bool interruptTriggered = false;

// LED-Pin
const int ledPin = LED_BUILTIN;

// Zähler für Sekunden
unsigned long secondsCounter = 0;
unsigned long lastMillis = 0;
unsigned long sekunde = 10;
// ISR (Interrupt Service Routine)
void rtcInterrupt() {
  interruptTriggered = true;
}

void setup() {
  // Initialisiere serielle Kommunikation für Debug
  Serial.begin(115200);
  while (!Serial);
  
  pinMode(ledPin, OUTPUT);

  // Initialisiere die RTC
  rtc.begin();

  // Stelle die Zeit ein (z.B. 00:00:00)
  rtc.setTime(0, 0, 0);

  // Stelle den Alarm so ein, dass alle 10 Sekunden unabhängig von der Minute ausgelöst wird
  rtc.setAlarmSeconds(sekunde);  // Alarm wird nach 10 Sekunden ausgelöst

  // Alarm jedes Mal wiederholen
  rtc.enableAlarm(rtc.MATCH_SS);  // MATCH_SS bedeutet Sekundenabgleich für wiederholende Alarme

  // Aktiviere den Interrupt
  rtc.attachInterrupt(rtcInterrupt);
}

void loop() {
  // Zähler unabhängig vom Interrupt hochzählen
  if (millis() - lastMillis >= 1000) {
    lastMillis = millis();
    secondsCounter++;
    Serial.print("Sekunden seit Start: ");
    Serial.println(secondsCounter);
  }

  // Überprüfen, ob der Interrupt ausgelöst wurde
  if (interruptTriggered) {
    Serial.println("Interrupt ausgelöst!");
    
    // LED blinkt für 1 Sekunde
    digitalWrite(ledPin, HIGH);
    delay(1000);
    digitalWrite(ledPin, LOW);

    // Zurücksetzen der Interrupt-Variable
    interruptTriggered = false;
    sekunde = (sekunde + 10)%60;
    rtc.setAlarmSeconds(sekunde);
  }
}
