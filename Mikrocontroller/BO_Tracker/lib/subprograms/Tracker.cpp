#include "Tracker.h"

// Konstruktor ruft den Basisklassen-Konstruktor (GNSS) auf
Tracker::Tracker(Stream &atSerial, Stream &dSerial, JsonDocument &doc)
    : MQTT_AWS(atSerial, dSerial, doc)
{
}
Tracker::~Tracker() {}

// Beispiel-Implementierung der InitModule()-Funktion
bool Tracker::InitModule()
{
    if (initBoard() && initModem() && startConnect())
    {
        return initHTTP() && InitGNSS();
    }
    return false;
}

void Tracker::firstStart()
{
    // !!! Fehler nicht behebel, Internet erforderllich
    if (InitModule())
    {
        setCurrentTimeToRTC();
    }
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
        if (docOutput["frequenz"].is<unsigned long>())
        {
            unsigned long newFrequenz = docOutput["frequenz"];
            // if (newFrequenz > trackerModes.maxRealTime)
            // {
            trackerModes.period = newFrequenz;
            trackerModes.realtime = (trackerModes.period < trackerModes.maxRealTime);
            // }
        }
        // if (docOutput["rat"].is<String>())
        // {
        //     const char *rat = docOutput["rat"];
        //     if (strcmp(trackerModes.RAT, rat) != 0)
        //     {
        //         strncpy(trackerModes.RAT, rat, sizeof(trackerModes.RAT) - 1);
        //         trackerModes.RAT[sizeof(trackerModes.RAT) - 1] = '\0';
        //         if (resetModem())
        //         {
        //             firstStart();
        //             return false;
        //         }
        //     }
        // }
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
    // if (modeRequest)
    // {
    //     docInput["RequestMode"] = true;
    //     modeRequest = false;
    // }
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
    if (trackerModes.CellInfosMode && !handleCellInfosMode())
    {
        return false;
    }
    if (trackerModes.GnssMode)
    {
        handleGNSSMode();
    }

    docInput["IMEI"] = modemIMEI;
    docInput["frequenz"] = trackerModes.period;
    // docInput["Timestamp"] = getDateTime();
    // handle = true;
    return true;
}

// bool Tracker::sendAndCheck()
// {
//     char response[1028];

//     while (abs(millis() - pub_time) > trackerModes.period - 1000)
//     {
//         if (!modeHandle() || !publishData("/pub"))
//         {
//             return false;
//         }
//         pub_time = millis();
//         // delay(300);
//     }
//     Mqtt_URC_Event_t ret = _BG96.WaitCheckMQTTURCEvent(response, 2);
//     // Serial.print("ret:");
//     // Serial.println(ret);
//     // Serial.println("response");
//     // Serial.println(response);

//     switch (ret)
//     {
//     case MQTT_RECV_DATA_EVENT:
//         setMode(response);
//         return false;
//     case MQTT_STATUS_EVENT:
//         if (handleMQTTStatusEventClose(response)){
//             mqtt_available = false;
//             return false;
//         }
//         break;
//     default:
//         break;
//     }
//     return true;
// }
bool Tracker::sendAndCheck()
{
    bool keepRunning = false;

    // Serial.println("sendAndWaitResponseHTTP");
    /////
    keepRunning = sendAndWaitResponseHTTP(); /// isMQTTAvaliable() ? pubAndsubMQTT() : sendAndWaitResponseHTTP();

    if (!trackerModes.realtime)
    {
        return false;
    }
    while (abs(millis() - pub_time) <= trackerModes.period - 1000)
    {
        // delay(1000);
        // Serial.println("less than interval");
    }

    return keepRunning;
}

bool Tracker::pubAndsubMQTT()
{
    char response[1028];

    if (!modeHandle() || !publishData("/pub"))
    {
        return false;
    }
    pub_time = millis();
    Mqtt_Event_t event = waitForResponse(response);
    switch (event)
    {
    case MQTT_RECV_DATA_EVENT:
        return setMode(response);
    case MQTT_CLIENT_CLOSED:
        return false;
    default:
        break;
    }

    return true;
}

bool Tracker::sendAndWaitResponseHTTP()
{
    char payload[4096];
    char response[1028];

    if (!modeHandle())
    {
        return false;
    }
    if (!serializeJsonPretty(docInput, payload))
    {
        return false;
    }
    docInput.clear();

    if (!sendAndReadResponse(payload, response))
    {
        // Serial.println("Fehler bei: sendAndReadResponse");
        return false;
    }

    if (!responseValid(response))
    {
        // Serial.println("Response invalid");
        setMode(response);
        return false;
    }
    // Serial.println("Response valid");

    pub_time = millis();
    // handle = false;
    return true;
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
    if (!turnOffModem() && !turnOnModem())
    {
        countReset++;
        return true;
    }
    return false;
}
bool Tracker::turnOnFunctionality()
{
    bool success = true;

    // Modem einschalten (falls noch nicht verfügbar)
    success &= isModemAvailable() || turnOnModem();

    // GNSS nur einschalten, wenn trackerModes.GnssMode == true
    // (bzw. wenn es nicht already enabled ist)
    success &= !trackerModes.GnssMode || isGnssModuleEnable() || TurnOnGNSS();

    // Netzwerkverbindung herstellen
    success &= isConnected() || startConnect();

    // Entweder MQTT oder HTTP
    // if (useMQTT)
    // {
    //     // Wenn wir MQTT verwenden, MQTT initialisieren
    //     success &= isMQTTAvaliable() || startMQTT();
    // }
    // else
    // {
    // Ansonsten HTTP verwenden
    success &= isUrlSetted() || (setHTTPURL(http_url) && pingServer());
    // }

    // Fehlerbehandlung
    handleErrors();

    return success;
}

bool Tracker::wakeUp()
{
    return (waitForMotion() || trackerModes.wakeUp);
}

bool Tracker::handleCellInfosMode()
{
    // _BG96.ScanCells(RAT, cells);

    JsonArray cellsArray = docInput["cells"].to<JsonArray>();

    if (!fillCellsQueue())
    {
        runningLogger.logError("FillCells");
        return false;
    };

    cells_queue.addCellsToJsonArray(&cellsArray);

    // Serial.println("handleCellInfosMode()");

    return true;
}

bool Tracker::retryIn1Hour()
{
    countReset = 0;
    deepSleep(3600 * 1000);
    return true;
}

int Tracker::getResetCount()
{
    return this->countReset;
}

bool Tracker::handleErrors()
{
    if (countReset > 3)
    {
        trackerModes.wakeUp = false;
        countReset = 0;
        return true;
    }
    if (checkForError() > 0)
    {
        return resetModem();
    }
    return true;
}

bool Tracker::handleIniTErrors()
{

    return true;
}