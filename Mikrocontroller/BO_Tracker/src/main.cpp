
#include "arduino_bma456.h"

uint32_t step = 0;
float x = 0, y = 0, z = 0;
void setup(void)
{
  Serial.begin(115200);
  Serial.println("BMA456 Step Counter");

  // bma456.initialize(RANGE_4G, ODR_50_HZ, NORMAL_AVG4, CIC_AVG);
  bma456.initialize();
  // bma456.enableWakeOnMotion();
}

void loop(void)
{
  bma456.getAcceleration(&x, &y, &z);

  Serial.print("X: ");
  Serial.print(x);
  Serial.print(", Y: ");
  Serial.print(y);
  Serial.print(", Z: ");
  Serial.print(z);
}
