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

    initModem();
    
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

void Tracker::setModeRequest(bool modeRequest){
    this->modeRequest = modeRequest;
}

void Tracker::firstStart(){
    InitModule();
    setHTTPURL();
    docInput["IMEI"] = modemIMEI;
    docInput["RequestMode"] = true;
    char payload[1028];
    char response[1028];

    serializeJsonPretty(docInput, payload);
    sendAndReadResponse(payload,response);

    if(setMode(response))
        setModeRequest(false);

    docInput.clear();
}

bool Tracker::setMode(char *payload){
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
                    DSerial.print("Updated geoRadius to: ");
                    DSerial.println(trackerModes.geoRadius);
                }

                if (geofence["geoLatitude"].is<float>())
                {
                    trackerModes.geoLatitude = geofence["geoLatitude"];
                    DSerial.print("Updated geoLatitude to: ");
                    DSerial.println(trackerModes.geoLatitude);
                }

                if (geofence["geoLongitude"].is<float>())
                {
                    trackerModes.geoLongitude = geofence["geoLongitude"];
                    DSerial.print("Updated geoLongitude to: ");
                    DSerial.println(trackerModes.geoLongitude);
                }

                if (geofence["GeoFenMode"].is<boolean>())
                {
                    trackerModes.GeoFenMode = geofence["GeoFenMode"];
                    DSerial.print("Updated GeoFenMode to: ");
                    DSerial.println(trackerModes.GeoFenMode);
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
            else
            {
                DSerial.println("Geofences array is empty!");
            }
        }
        else
        {
            DSerial.println("Geofences not found or invalid format!");
        }
    }
    else
    {
        DSerial.println("\r\n Error in Deserialization!");
        DSerial.println(error.c_str());
    }
    docOutput.clear();
}