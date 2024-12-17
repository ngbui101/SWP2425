#ifndef __Mode_Handle_H
#define __Mode_Handle_H
#include <GNSS.hpp>

void initModul(Stream &DSerial, _BG96_Module &_BG96, _Board &_ArdruinoZero)
{
    if (initModem(DSerial, _BG96, _ArdruinoZero) && InitModemMQTT(DSerial, _BG96) && InitGNSS(DSerial, _BG96) && _ArdruinoZero.initBoard())
    {
        Serial.println("Modul initialized");
    }
}
void modeHandle(Stream &DSerial, _BG96_Module &_BG96, JsonDocument &docInput, _Board &_ArdruinoZero)
{
    pub_time = millis();
    // GNSS-Modus verwalten
    if (trackerModes.GnssMode)
    {
        handleGNSSMode(DSerial, _BG96, docInput);
    }
    else if (gnssTracker.isOn)
    {
        _BG96.TurnOffGNSS();
        gnssTracker.isOn = false;
        gnssTracker.startMillis = 0;
    }

    // Temperatur- und Feuchtigkeitsdaten sammeln
    if (trackerModes.TemperatureMode)
    {
        docInput["Temperature"] = _ArdruinoZero.readTemperature();
        docInput["Humidity"] = _ArdruinoZero.readHumidity();
    }

    // Batteriestand erfassen
    if (trackerModes.BatteryMode)
    {
        docInput["BatteryPercentage"] = _ArdruinoZero.calculateBatteryPercentage();
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

    // Request-Modus setzen
    trackerModes.updateRequestMode();
    if (trackerModes.RequestMode)
    {
        docInput["RequestMode"] = true;
    }
    // Zeitstempel hinzufügen
    docInput["Timestamp"] = _ArdruinoZero.getDateTime();
    // Daten veröffentlichen
    if (publishData(DSerial, _BG96, docInput, pub_time, AT_LEAST_ONCE, "/pub"))
    {
        delay(300);
    }
    if (trackerModes.GeoFenMode)
    {
        if (!gnssTracker.geoFenceInit)
            addGeo(DSerial, _BG96);
        GEOFENCE_STATUS_t status = _BG96.getGeoFencingStatus(gnssTracker.geoID);
        switch (status)
        {
        case OUTSIDE_GEOFENCE:
            docInput["LeavingGeo"] = true;
            if (publishData(DSerial, _BG96, docInput, pub_time, AT_LEAST_ONCE, "/notifications"))
            {
                delay(300);
            }
            break;
        case INSIDE_GEOFENCE:
        case NOFIX:
            break;
        default:
            break;
        }
    }
}
void DailyUpdates(Stream &DSerial, _BG96_Module &_BG96, JsonDocument &docInput, _Battery _BoardBattery, _Temperature _BoardTemperature)
{
    GPSOneXtraCheckForUpdate(DSerial, _BG96);
    double temperature = _BoardTemperature.readTemperature();
    if (temperature < -20)
    {
        docInput["TemperatureLow"] = true;
    }
    else if (temperature > 60)
    {
        docInput["TemperatureHigh"] = true;
    }
    if (_BoardBattery.calculateBatteryPercentage() <= 10)
    {
        docInput["BatteryLow"] = true;
    }
    if (publishData(DSerial, _BG96, docInput, pub_time, AT_LEAST_ONCE, "/notifications"))
    {
        delay(300);
    }
}
void handleEvent(Stream &DSerial, JsonDocument &docOutput, char *payload)
{
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
                trackerModes.frequenz = newFrequenz;
                DSerial.print("Updated publishing frequency to: ");
                DSerial.println(trackerModes.frequenz);
            }
        }
        if (docOutput["geoRadius"].is<int>())
        {
            trackerModes.geoRadius = docOutput["geoRadius"].as<unsigned int>();
            DSerial.print("Updated geoRadius to: ");
            DSerial.println(trackerModes.geoRadius);
        }

        if (docOutput["geoLatitude"].is<float>())
        {
            trackerModes.geoLatitude = docOutput["geoLatitude"];
            DSerial.print("Updated geoLatitude to: ");
            DSerial.println(trackerModes.geoLatitude);
        }

        if (docOutput["geoLongitude"].is<float>())
        {
            trackerModes.geoLongitude = docOutput["geoLongitude"];
            DSerial.print("Updated geoLongitude to: ");
            DSerial.println(trackerModes.geoLongitude);
        }
    }
    else
    {
        DSerial.println("\r\n Error in Deserialization!");
        DSerial.println(error.c_str());
    }
    docOutput.clear();
}
void waitAndCheck(Stream &DSerial, _BG96_Module &_AWS, JsonDocument &docOutput)
{
    char payload[1028];
    Mqtt_URC_Event_t ret = _AWS.WaitCheckMQTTURCEvent(payload, 2);

    switch (ret)
    {
    case MQTT_RECV_DATA_EVENT:
        DSerial.println("RECV_DATA_EVENT");
        handleEvent(DSerial, docOutput, payload);
        // modeHandle(docInput,_BoardBattery);
        break;
    case MQTT_STATUS_EVENT:
        handleMQTTStatusEvent(DSerial, _AWS, payload);
        break;
    default:
        break;
    }
}
#endif
