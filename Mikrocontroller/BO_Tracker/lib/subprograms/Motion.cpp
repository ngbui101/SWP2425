#include "Motion.h"

_Motion::_Motion() : _RTC()
{
}

_Motion::~_Motion()
{
}

// Hier neu

static uint16_t bma_i2c_write(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len)
{
    Wire.beginTransmission(addr);
    Wire.write(reg);
    for (uint16_t i = 0; i < len; i++)
    {
        Wire.write(data[i]);
    }
    Wire.endTransmission();

    return 0;
}

static uint16_t bma_i2c_read(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t len)
{
    uint16_t i = 0;

    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom((int16_t)addr, len);
    while (Wire.available())
    {
        data[i++] = Wire.read();
    }

    return 0;
}

static void bma_delay_ms(uint32_t ms)
{
    delay(ms);
}

void _Motion::initialize(MA456_RANGE range, MBA456_ODR odr, MA456_BW bw, MA456_PERF_MODE mode)
{
    Wire.begin();

    accel.dev_addr = BMA4_I2C_ADDR_SECONDARY;
    accel.interface = BMA4_I2C_INTERFACE;
    accel.bus_read = bma_i2c_read;
    accel.bus_write = bma_i2c_write;
    accel.delay = bma_delay_ms;
    accel.read_write_len = 8;
    accel.resolution = 16;
    accel.feature_len = BMA456_FEATURE_SIZE;

    bma456_init(&accel);

    bma4_set_command_register(0xB6, &accel); // reset device

    delay(500); // wait for POR finish

    bma456_write_config_file(&accel);

    accel_conf.odr = (uint8_t)odr;
    accel_conf.range = (uint8_t)range;
    accel_conf.bandwidth = (uint8_t)bw;
    accel_conf.perf_mode = (uint8_t)mode;

    bma4_set_accel_config(&accel_conf, &accel);

    if (range == RANGE_2G)
    {
        devRange = 2000;
    }
    else if (range == RANGE_4G)
    {
        devRange = 4000;
    }
    else if (range == RANGE_8G)
    {
        devRange = 8000;
    }
    else if (range == RANGE_16G)
    {
        devRange = 16000;
    }

    bma4_set_accel_enable(BMA4_ENABLE, &accel);
}

void _Motion::stepCounterEnable(MA456_PLATFORM_CONF conf, bool cmd)
{
    bma456_reset_step_counter(&accel);
    bma456_select_platform(conf, &accel);
    bma456_feature_enable(BMA456_STEP_CNTR, cmd, &accel);
}

void _Motion::getAcceleration(float *x, float *y, float *z)
{
    struct bma4_accel sens_data;

    bma4_read_accel_xyz(&sens_data, &accel);

    *x = (float)sens_data.x * devRange / 32768;
    *y = (float)sens_data.y * devRange / 32768;
    *z = (float)sens_data.z * devRange / 32768;
}

int32_t _Motion::getTemperature(void)
{
    int32_t temp = 0;

    bma4_get_temperature(&temp, BMA4_DEG, &accel);

    return (temp / 1000);
}

uint32_t _Motion::getStepCounterOutput(void)
{
    uint32_t step = 0;

    bma456_step_counter_output(&step, &accel);

    return step;
}

int _Motion::enableWakeOnMotion()
{
    uint16_t rslt;

    // Wakeup-Feature aktivieren/deaktivieren

    rslt = bma456_map_interrupt(BMA4_INTR1_MAP, BMA456_WAKEUP_INT, BMA4_ENABLE, &accel);
    rslt = bma456_wakeup_set_sensitivity(0x00, &accel);
    if (rslt == BMA4_OK)
    {
        rslt = bma456_feature_enable(BMA456_WAKEUP, BMA4_ENABLE, &accel);
    }
    return rslt;
}

int _Motion::enableAnyNoMotion()
{
    uint16_t rslt;

    // Wakeup-Feature aktivieren/deaktivieren

    rslt = bma456_map_interrupt(BMA4_INTR1_MAP, BMA456_ANY_NO_MOTION_INT, BMA4_ENABLE, &accel);
    if (rslt == BMA4_OK)
    {
        // rslt = bma456_step_detector_enable(BMA456_ALL_AXIS_EN,&accel);
        rslt = bma456_feature_enable(BMA456_ANY_MOTION, BMA4_ENABLE, &accel);
    }
    return rslt;
}

bool _Motion::waitForMotion()
{
    uint16_t rslt;
    uint16_t int_status = 0;

    /* Read the interrupt status */
    rslt = bma456_read_int_status(&int_status, &accel);
    /* Check if sig-motion interrupt is received */
    if ((rslt == BMA4_OK) && (int_status & BMA456_WAKEUP_INT))
    {
        return true;
    }
    return false;
}

bool _Motion::checkForMotionInMillis(unsigned long time, float threshold)
{
    unsigned long interval = time / 10;
    float x0, y0, z0;
    float x, y, z;
    int count = 0;
    float sumMagnitude = 0;
    float avgMagnitude = 0;
    getAcceleration(&x0, &y0, &z0);

    while (count <= 10)
    {
        getAcceleration(&x, &y, &z);
        float xx0 = x-x0;
        float yy0 = y-y0;
        float zz0 = z-z0;
        // Serial.print("xx0: ");
        // Serial.println(xx0);
        // Serial.print("yy0: ");
        // Serial.println(yy0);
        // Serial.print("zz0: ");
        // Serial.println(zz0);

        float magnitude = sqrt(xx0 * xx0 + yy0*yy0 + zz0*zz0);
        
        x0 = x;
        y0 = y;
        z0 = z;
        sumMagnitude += magnitude;
        count++;
        delay(interval);
    }
    avgMagnitude = sumMagnitude / count;
    // Serial.println(avgMagnitude);
    return (avgMagnitude > threshold);
}

bool _Motion::isMovementAboveThreshold(float threshold)
{
    float x, y, z;
    getAcceleration(&x, &y, &z);

    // Berechnung des Betrags der Beschleunigung aus den drei Achsen
    float magnitude = sqrt((x * x) + (y * y) + (z * z));

    // Wenn der Betrag die Grenze überschreitet, geben wir true zurück
    return (magnitude > threshold);
}

