#include "Tracker.h"

// Konstruktor ruft den Basisklassen-Konstruktor (GNSS) auf
Tracker::Tracker(Stream &atSerial, Stream &dSerial, JsonDocument &doc)
    : GNSS(atSerial, dSerial, doc)
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
    handleErrors();
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

bool Tracker::modeHandle()
{   
    if (trackerModes.CellInfosMode && !handleCellInfosMode())
    {
        return false;
    }
    if (trackerModes.GnssMode && !handleGNSSMode(POWERSAVING_DISABLE))
    {
        return false;
    }

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

    docInput["IMEI"] = modemIMEI;
    docInput["frequenz"] = trackerModes.period;
    // docInput["Timestamp"] = getDateTime();
    // handle = true;
    return true;
}

bool Tracker::pubAndsubMQTT(unsigned long interval)
{
    char response[1028];
    // pub_time = millis();
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
    if ((millis() - measure_time) < interval)
        return true;

    if (!modeHandle())
    {
        return false;
    }

    measure_time = millis();

    if (!publishData("/pub"))
    {   
        return false;
    }
    delay(100);
    return true;
}

bool Tracker::sendAndWaitResponseHTTP(unsigned long interval)
{
    char payload[4096];
    char response[1028];

    if ((millis() - measure_time) < interval)
        return true;

    if (!modeHandle())
    {
        return false;
    }
    else
    {
        measure_time = millis();
        if (!serializeJsonPretty(docInput, payload))
        {
            return false;
        }
    }

    if (!sendAndReadResponse(payload, response))
    {
        // Serial.println("Fehler bei: sendAndReadResponse");
        return false;
    }
    docInput.clear();

    if (!responseValid(response))
    {
        Serial.println("Response invalid");
        setMode(response);
        return false;
    }
    Serial.println("Response valid");
    // post_time = millis();
    // pub_time = millis();
    // measure_time = 0;
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

bool Tracker::turnOnFunctionality(bool useMQTT)
{
    bool success = true;
    measure_time = -999999; //measure_time zurücksetzen

    // Modem einschalten (falls noch nicht verfügbar)
    success &= isModemAvailable() || turnOnModem();

    // GNSS nur einschalten, wenn trackerModes.GnssMode == true
    // (bzw. wenn es nicht already enabled ist)
    success &= !trackerModes.GnssMode || isGnssModuleEnable() || TurnOnGNSS();

    // success &= !trackerModes.MotionMode || enable_step_counter || stepCounterEnable();
    // Netzwerkverbindung herstellen
    success &= isConnected() || startConnect();

    // Entweder MQTT oder HTTP
    if (!useMQTT)
    {
        // Wenn wir MQTT verwenden, MQTT initialisieren
        success &= isUrlSetted() || (setHTTPURL(http_url) && pingServer());
    }
    else
    {
        // Ansonsten HTTP verwenden
        success &= mqtt_available || startMQTT();
    }

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
        // if (docOutput["MotionMode"].is<boolean>()){
        //     trackerModes.MotionMode = docOutput["MotionMode"];
        // }
        if (docOutput["NmeaMode"].is<boolean>())
        {
            trackerModes.NmeaMode = docOutput["NmeaMode"];
        }
        // Frequenz aktualisieren
        if (docOutput["frequenz"].is<unsigned long>())
        {
            unsigned long newFrequenz = docOutput["frequenz"];
            if (newFrequenz > 0)
            {
                trackerModes.period = newFrequenz;
                if(trackerModes.period < trackerModes.maxRealTime){
                    stepCounterEnable();
                    trackerModes.realtime = true;
                }else{
                    trackerModes.realtime = false;
                }
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

bool Tracker::pingServer()
{
    char send_data[64];
    char recv_data[258];
    JsonDocument ping;
    ping["IMEI"]= modemIMEI;
    if(!serializeJsonPretty(ping, send_data)){
        return false;
    }
    // strcpy(send_data, PING);
    memset(recv_data, 0, sizeof(recv_data));

    if (sendAndReadResponse(send_data, recv_data) && setMode(recv_data))
    {   
        return true;
    }
    runningLogger.logError("pingServer");
    connect = false;
    return false;
}

bool Tracker::resetModem()
{
    if (turnOffModem() && turnOnModem())
    {
        countReset++;
        return true;
    }
    return false;
}

