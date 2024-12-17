
#ifndef __BOSCH_BMA456_H__
#define __BOSCH_BMA456_H__

#include <Arduino.h>
#include <Wire.h>
#include "bma456.h"

typedef enum
{
    CIC_AVG = 0,
    CONTINUOUS,
} MA456_PERF_MODE;

typedef enum
{
    OSR4_AVG1 = 0,
    OSR2_AVG2,
    NORMAL_AVG4,
    CIC_AVG8,
    RES_AVG16,
    RES_AVG32,
    RES_AVG64,
    RES_AVG128,
} MA456_BW;

typedef enum
{
    ODR_0_78_HZ = 1,
    ODR_1_5_HZ,
    ODR_3_1_HZ,
    ODR_6_25_HZ,
    ODR_12_5_HZ,
    ODR_25_HZ,
    ODR_50_HZ,
    ODR_100_HZ,
    ODR_200_HZ,
    ODR_400_HZ,
    ODR_800_HZ,
    ODR_1600_HZ,
} MBA456_ODR;

typedef enum
{
    RANGE_2G = 0,
    RANGE_4G,
    RANGE_8G,
    RANGE_16G,
} MA456_RANGE;

typedef enum
{
    PHONE_CONFIG = 0,
    WRIST_CONFIG = 1,
} MA456_PLATFORM_CONF;

class BMA456
{
public:
    BMA456(void) {}

    void initialize(MA456_RANGE range = RANGE_4G, MBA456_ODR odr = ODR_100_HZ,
                    MA456_BW bw = NORMAL_AVG4, MA456_PERF_MODE mode = CIC_AVG);

    void stepCounterEnable(MA456_PLATFORM_CONF conf = WRIST_CONFIG, bool cmd = true);

    void getAcceleration(float *x, float *y, float *z);

    int32_t getTemperature(void);

    uint32_t getStepCounterOutput(void);

    int enableWakeOnMotion();

    int enableAnyNoMotion();
    void attachInterruptWakeOnMotion(uint8_t int_line = 0);

    int readPinStatus(uint8_t *data);
    bool waitForMotion();

    void checkForAnyMotion();

    bool isMovementAboveThreshold(float threshold);
private:
    float devRange;
    struct bma4_dev accel;
    struct bma4_accel_config accel_conf;
};

extern BMA456 bma456;

#endif