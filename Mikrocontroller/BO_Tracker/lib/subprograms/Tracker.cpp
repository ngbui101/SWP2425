#include "Tracker.h"

// Konstruktor ruft den Basisklassen-Konstruktor (GNSS) auf
Tracker::Tracker(Stream &atSerial, Stream &dSerial, JsonDocument &doc)
    : MQTT_AWS(atSerial, dSerial, doc)
{
}

// Beispiel-Implementierung der InitModule()-Funktion
void Tracker::InitModule()
{
    initBoard();

    if (initModem())
    {
        funkModuleEnable = true;
    }

    initHTTP();

    setCurrentTimeToRTC();

    InitGNSS();

    // initMQTT();

    DSerial.print("\nTotal Error: ");
    int totalError = initLogger.getErrorCount();
    DSerial.println(totalError);
    char allErrorsBuffer[440];
    initLogger.getAllError(allErrorsBuffer);
    DSerial.println(allErrorsBuffer);
    initLogger.clear();
}

bool Tracker::setCurrentTimeToRTC()
{
    const char *modemTime = _BG96.GetCurrentTime();
    DSerial.println(modemTime); // Ausgabe der Modem-Zeit

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
    setHTTPURL(http_url);

    modeHandle();

    char payload[1028];
    char response[1028];

    if(!serializeJsonPretty(docInput, payload))
        return;

    if(!sendAndReadResponse(payload, response))
        return;

    if (setMode(response))
        setModeRequest(false);
    docInput.clear();
}

bool Tracker::setMode(char *payload)
{
    JsonDocument docOutput;
    DeserializationError error = deserializeJson(docOutput, payload);

    if (error == DeserializationError::Ok)
    {
        if (docOutput["RequestMode"].is<boolean>())
        {
            modeRequest = docOutput["GnssMode"];
        }
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
                DSerial.print("Updated publishing frequency to: ");
                DSerial.println(trackerModes.period);
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
        DSerial.println("\r\n Error in Deserialization!");
        DSerial.println(error.c_str());
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
            ;
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
    // GNSS-Modus verwalten
    // else if (gnssData.isOn)
    // {
    //     tracker.TurnOff();
    // }

    // Temperatur- und Feuchtigkeitsdaten sammeln

    // Zeitstempel hinzufügen
    docInput["IMEI"] = modemIMEI;
    docInput["Timestamp"] = getDateTime();
    // Daten veröffentlichen
    // if (tracker.publishData("/pub"))
    // {
    //     // delay(300);
    // }
    // if (trackerModes.GeoFenMode)
    // {
    //     GEOFENCE_STATUS_t status = _BG96.getGeoFencingStatus(gnssData.geoID);
    //     switch (status)
    //     {
    //     case OUTSIDE_GEOFENCE:
    //         docInput["LeavingGeo"] = true;
    //         if (publishData(pub_time, AT_LEAST_ONCE, "/notifications"))
    //         {
    //             delay(300);
    //         }
    //         break;
    //     case INSIDE_GEOFENCE:
    //     case NOFIX:
    //         break;
    //     default:
    //         break;
    //     }
    // }
    return true;
}