
#include "arduino_bma456.h"
#include <ArduinoLowPower.h>
#include <board.h>

PinStatus state = LOW;

const int wakeUpPin = 0;
const int pin2 = 1;

uint32_t step = 0;
float x = 0, y = 0, z = 0;
int32_t temp = 0;
uint8_t data[2] = {0};
bool motion = false;
_Board board = _Board();
void wakeUp()
{
  detachInterrupt(digitalPinToInterrupt(wakeUpPin));
  motion = true;
}
void setup(void)
{
  Serial.begin(9600);
  pinMode(wakeUpPin, INPUT_PULLUP);

  board.initBoard();
  pinMode(LED_BUILTIN, OUTPUT);
  bma456.initialize(RANGE_4G, ODR_50_HZ, NORMAL_AVG4, CIC_AVG);
  bma456.initialize();
  if (bma456.enableWakeOnMotion() == 0)
  {
    Serial.println("Wake on Motion enabled");
  };
  LowPower.attachInterruptWakeup(digitalPinToInterrupt(wakeUpPin), wakeUp, CHANGE);
}

void loop(void)
{
  
  if (bma456.waitForMotion())
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000); // wait for a second
    digitalWrite(LED_BUILTIN, LOW); 
  }
  LowPower.sleep(10000);
}