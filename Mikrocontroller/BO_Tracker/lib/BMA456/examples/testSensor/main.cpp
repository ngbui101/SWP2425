
#include "arduino_bma456.h"
#include <ArduinoLowPower.h>
#include <board.h>

#define INTERRUPT_PIN D0
const int pin1 = 0;
const int pin2 = 1;

uint32_t step = 0;
float x = 0, y = 0, z = 0;
int32_t temp = 0;
uint8_t data[2] = {0};
bool motion = false;
_Board board = _Board();
void ledOn()
{
  motion = true;
}
void setup(void)
{
  board.initBoard();
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  attachInterrupt(0, ledOn, RISING);
  digitalWrite(pin1, LOW);
  bma456.initialize(RANGE_4G, ODR_50_HZ, NORMAL_AVG4, CIC_AVG);
  bma456.initialize();
  if (bma456.enableWakeOnMotion() == 0)
  {
    Serial.println("Wake on Motion enabled");
  };
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);                    // wait for a second
  digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
  delay(1000);
}

void loop(void)
{
  bma456.getAcceleration(&x, &y, &z);
  temp = bma456.getTemperature();

  Serial.print("X: ");
  Serial.print(x);
  Serial.print(", Y: ");
  Serial.print(y);
  Serial.print(", Z: ");
  Serial.print(z);
  Serial.print(", T: ");
  Serial.println(temp);
  int status_1 = digitalRead(pin1);
  int status_2 = digitalRead(pin2);
  Serial.println(status_1);
  Serial.println(status_2);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  if (motion)
  {
    Serial.println("On Motion...");
    motion = false;
  }
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  Serial.println(board.calculateBatteryPercentage());
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  bma456.waitForMotion();
  LowPower.sleep(5000);
  // Serial.print("Sleep....");
  // LowPower.sleep(5000);
  // Serial.print("Wake up...");
  // bma456.waitForMotion();
  //  delay(1000);
  // digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
  // delay(1000);
}