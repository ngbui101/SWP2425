#include "Tracker.h"

// Konstruktor ruft den Basisklassen-Konstruktor (GNSS) auf
Tracker::Tracker(Stream &atSerial, Stream &dSerial, JsonDocument &doc)
    : MQTT_AWS(atSerial, dSerial, doc)
{
}
Tracker::~Tracker() {}

// Beispiel-Implementierung der InitModule()-Funktion
void Tracker::InitModule()
{
    initBoard();

    initModem();

    initHTTP();

    InitGNSS();

    connect = true;
}

bool Tracker::setCurrentTimeToRTC()
{
    const char *modemTime = _BG96.GetCurrentTime();
    if (!setcurrentTime(modemTime))
    { // RTC im Board setzen
        initLogger.logError("SetTime");
        return false;
    }
    return true;
}

void Tracker::setModeRequest(bool modeRequest)
{
    this->modeRequest = modeRequest;
}

void Tracker::firstStart()
{
    InitModule();
    // startConnect();

    setCurrentTimeToRTC();

    setHTTPURL(http_url);

    modeHandle();

    char payload[1028];
    char response[1028];

    if (!serializeJsonPretty(docInput, payload))
        return;

    if (!sendAndReadResponse(payload, response))
        return;

    if (setMode(response))
        setModeRequest(false);
    docInput.clear();
    checkForError();
}

int Tracker::checkForError()
{
    int totalErrors = getInitErrorCount() + getRunningErrorCount();
    if (totalErrors != 0)
    {
        initLogger.flushErrors();
        runningLogger.flushErrors();
    }
    return totalErrors;
}
bool Tracker::responseValid(char *payload)
{
    JsonDocument docOutput;
    DeserializationError error = deserializeJson(docOutput, payload);

    if (error == DeserializationError::Ok)
    {
        if (docOutput["valid"].is<boolean>() && docOutput["valid"] == true)
        {
            return true;
        }
    }
    return false;
}

bool Tracker::setMode(char *payload)
{
    JsonDocument docOutput;
    DeserializationError error = deserializeJson(docOutput, payload);

    if (error == DeserializationError::Ok)
    {
        if (docOutput["GnssMode"].is<boolean>())
        {
            trackerModes.GnssMode = docOutput["GnssMode"];
        }
        if (docOutput["CellInfosMode"].is<boolean>())
        {
            trackerModes.CellInfosMode = docOutput["CellInfosMode"];
        }
        if (docOutput["BatteryMode"].is<boolean>())
        {
            trackerModes.BatteryMode = docOutput["BatteryMode"];
        }
        if (docOutput["TemperatureMode"].is<boolean>())
        {
            trackerModes.TemperatureMode = docOutput["TemperatureMode"];
        }
        if (docOutput["NmeaMode"].is<boolean>())
        {
            trackerModes.NmeaMode = docOutput["NmeaMode"];
        }
        if (docOutput["GeoFenMode"].is<boolean>())
        {
            trackerModes.GeoFenMode = docOutput["GeoFenMode"];
        }
        // Frequenz aktualisieren
        if (docOutput["frequenz"].is<unsigned int>())
        {
            unsigned int newFrequenz = docOutput["frequenz"];
            if (newFrequenz > 0)
            {
                trackerModes.period = newFrequenz;
            }
        }
        // GeoUpdate
        if (docOutput["geofences"].is<JsonArray>())
        {
            JsonArray geofences = docOutput["geofences"].as<JsonArray>();
            if (geofences.size() > 0) // Prüfen, ob das Array nicht leer ist
            {
                JsonObject geofence = geofences[0];
                if (geofence["geoRadius"].is<int>())
                {
                    trackerModes.geoRadius = geofence["geoRadius"].as<unsigned int>();
                }

                if (geofence["geoLatitude"].is<float>())
                {
                    trackerModes.geoLatitude = geofence["geoLatitude"];
                }

                if (geofence["geoLongitude"].is<float>())
                {
                    trackerModes.geoLongitude = geofence["geoLongitude"];
                }

                if (geofence["GeoFenMode"].is<boolean>())
                {
                    trackerModes.GeoFenMode = geofence["GeoFenMode"];
                }
                //         if (addGeo())
                //         {
                //             DSerial.println("Geo added successfully!");
                //         }
                //         else
                //         {
                //             DSerial.println("Failed to add geo!");
                //         };
            }
        }
    }
    else
    {
        runningLogger.logError("Deserialization");
        return false;
    }
    docOutput.clear();
    return true;
}

bool Tracker::modeHandle()
{
    if (modeRequest)
    {
        docInput["RequestMode"] = true;
    }
    else
    {
        if (trackerModes.TemperatureMode)
        {
            docInput["Temperature"] = readTemperature();
            docInput["Humidity"] = readHumidity();
        }

        // Batteriestand erfassen
        if (trackerModes.BatteryMode)
        {
            docInput["BatteryPercentage"] = calculateBatteryPercentage();
        }

        // Zellinformationen erfassen
        if (trackerModes.CellInfosMode)
        {
            JsonArray cellsArray = docInput["cells"].to<JsonArray>();
            for (Cell *&cell : cells)
            {
                if (cell != nullptr)
                {
                    // JSON-Objekt für jede Zelle erstellen
                    JsonObject cellObj = cellsArray.add<JsonObject>();
                    cell->toJson(cellObj);
                }
            }
        }
        if (trackerModes.GnssMode)
        {
            handleGNSSMode();
        }
    }
    docInput["IMEI"] = modemIMEI;
    docInput["frequenz"] = trackerModes.period;
    docInput["Timestamp"] = getDateTime();
    return true;
}

bool Tracker::sendAndCheck()
{
    if (isMQTTAvaliable())
    {
        return pubAndsubMQTT();
    }
    return sendAndWaitResponseHTTP();
}

bool Tracker::pubAndsubMQTT()
{
    char response[1028];
    Mqtt_Event_t event = waitForResponse(response);
    switch (event)
    {
    case MQTT_RECV_DATA_EVENT:
        setMode(response);
        return false;
    case MQTT_CLIENT_CLOSED:
        return false;
    default:
        break;
    }
    if (abs(millis() - pub_time) >= trackerModes.period - 1000)
    {
        modeHandle();
        if (!publishData("/pub"))
        {
            return false;
        }
        pub_time = millis();
    }
    return true;
}

bool Tracker::sendAndWaitResponseHTTP()
{
    while (abs(millis() - pub_time) >= trackerModes.period - 1000)
    {
        char payload[1028];
        char response[1028];
        if (!modeHandle())
        {
            return false;
        };
        if (!serializeJsonPretty(docInput, payload))
            return false;

        docInput.clear();

        if (!sendAndReadResponse(payload, response))
            return false;

        if (!responseValid(response))
        {
            setMode(response);
            return false;
        }
        pub_time = millis();
        return true;
    }
    return false;
}

int Tracker::getInitErrorCount()
{
    return initLogger.getErrorCount();
}

int Tracker::getRunningErrorCount()
{
    return runningLogger.getErrorCount();
}

bool Tracker::turnOffModem()
{
    if (!_BG96.PowerOffModule())
    {
        return false;
    };
    funkModuleEnable = false;
    connect = false;
    getFirstFix = false;
    gpsModuleEnable = false;
    urlSetted = false;
    mqtt_available = false;
    return true;
}

bool Tracker::resetModem()
{

    if (!turnOffModem())
        return false;
    delay(3000);
    if (!turnOnModem())
        return false;

    return true;
}
bool Tracker::turnOnFunctionality()
{
    if (!isModemAvailable() && !turnOnModem())
    {
        return false;
    }
    if (trackerModes.GnssMode && !isGnssModuleEnable() && !TurnOnGNSS())
    {
        // Serial.println("TurnOnGNSS");
        return false;
    }
    if (!isConnected() && !startConnect())
    {
        // Serial.println("startConnect");
        return false;
    }
    if (!isUrlSetted() && !setHTTPURL(http_url))
    {
        // Serial.println("setHTTPURL");
        return false;
    }
    if (!isMQTTAvaliable() && useMQTT && !startMQTT())
    {
        return false;
    }
    return true;
}