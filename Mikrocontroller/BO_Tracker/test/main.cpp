
#include <Tracker.h>
#define DSerial SerialUSB
#define ATSerial Serial1

JsonDocument docOutput;

Tracker tracker(ATSerial, DSerial, docOutput);
_Motion motion_sensor;
float x, y, z;
void setup(void)
{
    DSerial.begin(115200);
    while (DSerial.read() >= 0)
        ;

    ATSerial.begin(115200);
    while (ATSerial.read() >= 0)
        ;
    delay(1000);
    // Serial.println("BMA456 Step Counter");
    delay(1000);

    tracker.startModem();
    motion_sensor.initialize();
    motion_sensor.stepCounterEnable();
}

void loop(void)
{
    // Serial.print("Accerleration");
    // float acc = 0;
    // motion_sensor.getDynamicAcceleration(&acc);
    // Serial.println(acc);
    // Serial.print("Acc: ");
    // Serial.println(motion_sensor.getAcceleration());
    // Serial.print("Step");
    // Serial.println(motion_sensor.getStepCounterOutput());
    // delay(100);
    // motion_sensor.getAcceleration(&x, &y, &z);
    // motion_sensor.getAcceleration(&x,&y,&z);
    // Serial.print("X: ");
    // Serial.print(x);
    // Serial.print(", Y: ");
    // Serial.print(y);
    // Serial.print(", Z: ");
    // Serial.println(z);
    // delay(1000);
    if (ATSerial.available())
    {
        char at = ATSerial.read();
        DSerial.write(at);
    }

    if (DSerial.available())
    {
        char d = DSerial.read();
        ATSerial.write(d);
        DSerial.write(d);
    }
}
