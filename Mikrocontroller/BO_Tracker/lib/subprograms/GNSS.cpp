#include "GNSS.h"

// Konstruktor der Klasse GNSS
GNSS::GNSS(Stream &atSerial, Stream &dSerial, JsonDocument &docInput)
    : MQTT_AWS(atSerial, dSerial, docInput) {}

// Initialisiert GNSS
bool GNSS::InitGNSS()
{
    char currentTimestamp[64];
    _BG96.GetLatestGMTTime(currentTimestamp);

    if (!_BG96.InitGpsOneXTRA(currentTimestamp))
    {
        initLogger.logError("GPS_OneXTRA");
        return false;
    }

    if (!_BG96.InitAGPS(SUPURL, APN))
    {
        initLogger.logError("AGPS");
        return false;
    }

    if (!_BG96.SetGNSSEnableNMEASentences(true))
    {
        initLogger.logError("Enable_NMEA");
        return false;
    }
    return true;
}

// Prüft auf Updates für GPSOneXTRA-Daten
void GNSS::GPSOneXtraCheckForUpdate()
{
    char currentTimestamp[64];
    _BG96.GetLatestGMTTime(currentTimestamp);
    _BG96.InjectGpsOneXTRAData("UFS:xtra2.bin", WRITE_MODE, currentTimestamp);
}

// Handhabt GNSS-Modus
bool GNSS::handleGNSSMode(Powersaving_Strategy gnss_strategy)
{
    if (!trackerModes.GnssMode)
    {
        TurnOff();
    }
    else
    {
        if (!gpsModuleEnable)
        {
            TurnOnGNSS();
        }
    }
    switch (gnss_strategy)
    {
    case POWERSAVING_DISABLE:
        return updatePosition();
        break;
    case PERIOD_WITH_MOTION:
        return handlePeriod();
        break;
    case DISTANT_BASED:
        return handleDistantBased();
        break;
    case DISTANT_BASED_WITH_SPEED:
        return handleDistantBasedWithSpeed();
        break;
    case DISTANT_BASED_WITH_MOTION:
        return handleDistantBasedWithMotion();
        break;
    default:
        return false;
        break;
    }
    // return handleDistantBasedWithMotion();
    return false;
}

bool GNSS::handlePeriod()
{   
    if (!last_position || last_position->getAccuracy() >= max_distant)
        return updatePosition();

    while (noMotion())
    {
        /* code */
    }
    
    return updatePosition();
}
bool GNSS::handleDistantBased()
{
    unsigned long start_time = millis();
    GNSS_Position position;
    while (!GetGnssPositionAndError(position))
    {
        if ((millis() - start_time) > 10 * 1000)
        {
            // timeout
            return false;
        }
        delay(1000);
    }
    if (!last_position)
    {
        last_position = new GNSS_Position(position);
        Serial.println("First fix position");
    }
    else if (position.distanceTo(*last_position) > (max_distant - last_position->getAccuracy()))
    {
        Serial.print("Distant to last position: ");
        Serial.println(position.distanceTo(*last_position));
        *last_position = position;
    }
    else
    {
        Serial.print("Distant to last position: ");
        Serial.println(position.distanceTo(*last_position));
        return false;
    }
    JsonObject gnss = docInput["gnss"].to<JsonObject>();
    last_position->toJson(gnss);
    gnss["TTFF"] = TTFF;
    return true;
}

bool GNSS::handleDistantBasedWithSpeed()
{
    unsigned long start_time = millis();
    GNSS_Position position;
    if ((last_position != nullptr))
    {
        while (assumed_speed * (millis() - start_time) / 1000 < (max_distant - last_position->getAccuracy())) //25 Sekunde warten
            ;
    }
    while (!GetGnssPositionAndError(position))
    {
        if ((millis() - start_time) > 10 * 1000)
        {
            return false;
        }
        delay(1000);
    }

    updatePosition();
    return true;
}

bool GNSS::handleDistantBasedWithMotion()
{
    // position update immediately, when last_position null or error more than max_distant
    if (!last_position || last_position->getAccuracy() >= max_distant)
        return updatePosition();
     // exist when nomotion
    if(noMotion())
        return false;
    
    // if onMotion
    unsigned long start_time = millis();
    float last_position_accuracy = last_position->getAccuracy();
    float threshold = max_distant - last_position_accuracy;
    float last_speed = last_position->getSpeed();
    max_speed = std::max(max_speed, last_speed);
    float distant = 0;
    while (distant < threshold)
    {
        distant = (max_speed * (millis() - start_time) / 1000) + distant_from_last;

        unsigned long no_motion_start = millis();
        while (noMotion()) /// If NoMotion occur
        {
            if ((millis() - no_motion_start) > 3000) // Falls 3 Sekunden keine Bewegung
            {
                distant_from_last = distant;
                return updatePositionAndResetSpeed();
            }
        }
    }
    if (distant == 0)
        return false;

    distant_from_last = 0;
    return updatePosition();
}

bool GNSS::TurnOff()
{
    if (PowerDownGNSS())
    {
        getFirstFix = false;
        gnss_start_time = 0;
        // fixCount = 0;
        max_speed = 0;
        last_position = nullptr;
        return true;
    }
    return false;
}
bool GNSS::PowerDownGNSS(){
    if (_BG96.TurnOffGNSS())
    {
        gpsModuleEnable = false;
        return true;
    }
    return false;
}

bool GNSS::TurnOnGNSS()
{
    if (_BG96.TurnOnGNSS(gnssData.workMode, WRITE_MODE))
    {
        gpsModuleEnable = true;
        gnss_start_time = millis();
        return true;
    }
    initLogger.logError("TurnOnGNSS");
    gpsModuleEnable = false;
    return false;
}

bool GNSS::isGnssModuleEnable()
{
    return gpsModuleEnable;
}

bool GNSS::GetGnssPositionAndError(GNSS_Position &current_fix)
{
    char position[256];
    float accuracy;
    if (!_BG96.GetGNSSPositionInformation(position))
    {
        return false;
    }
    _BG96.GetEstimationError(accuracy);
    position[sizeof(position) - 1] = '\0';

    const char *delimiter = ",";
    char *token = strtok(position, delimiter);
    int fieldIndex = 0;

    float latitude = 0.0f;
    float longitude = 0.0f;
    float hdop = 0.0f;
    int nsat = 0;
    float speed = 0.0f;
    while (token != nullptr)
    {
        switch (fieldIndex)
        {
        case 1:
            latitude = atof(token);
            break;
        case 2:
            longitude = atof(token);
            break;
        case 3:
            hdop = atof(token);
            break;
        case 7:
            speed = atof(token);
            break;
        case 10:
            nsat = atoi(token);
            break;
        default:
            break;
        }
        fieldIndex++;
        token = strtok(nullptr, delimiter);
    }

    if (TTFF == 0)
    {
        TTFF = millis() - gnss_start_time;
    }

    // fixCount++;
    getFirstFix = true;

    current_fix.InitPosition(latitude, longitude, hdop, nsat, accuracy, speed);
    // Erstellen des verschachtelten "gnss" Objekts
    return true;
}

bool GNSS::updatePosition()
{
    unsigned long start_time = millis();
    GNSS_Position position;
    while (!GetGnssPositionAndError(position))
    {
        if ((millis() - start_time) > 10 * 1000){
            docInput["gnss"] = "NoFix";
            return true;
        }
        delay(1000);
    }
    delete last_position;
    last_position = new GNSS_Position(position);

    JsonObject gnss = docInput["gnss"].to<JsonObject>();
    last_position->toJson(gnss);
    gnss["TTFF"] = TTFF;
    gnss["Step"] = getStepCounterOutput();
    // PowerDownGNSS();
    return true;
}

bool GNSS::updatePositionAndResetSpeed()
{
    if (updatePosition())
    {
        last_position->setSpeed(0);
        return true;
    }
    return false;
}


bool GNSS::turnOffModem()
{   
    funkModuleEnable = false;
    connect = false;
    gpsModuleEnable = false;
    urlSetted = false;
    mqtt_available = false;

    getFirstFix = false;
    
    if (!_BG96.PowerOffModule())
    {
        return false;
    };
    return true;
}