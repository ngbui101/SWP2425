#include "EventHandler.h"

// Konstruktor der Klasse Mode_Handle
EventHandler::EventHandler(Stream &serial, Tracker &tracker, JsonDocument &docInput, JsonDocument &docOutput )
    : DSerial(serial),tracker(tracker), docInput(docInput), docOutput(docOutput) {}

// Tägliche Updates
void EventHandler::dailyUpdates()
{
    tracker.GPSOneXtraCheckForUpdate();
    double temperature = tracker.readTemperature();
    if (temperature < -20)
    {
        docInput["TemperatureLow"] = true;
    }
    else if (temperature > 60)
    {
        docInput["TemperatureHigh"] = true;
    }
    if (tracker.calculateBatteryPercentage() <= 10)
    {
        docInput["BatteryLow"] = true;
    }
    if (tracker.publishData("/notifications"))
    {
        
    }
}

// Behandelt MQTT-Ereignisse
void EventHandler::setMode(char *payload)
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

// Wartet und prüft MQTT-URC-Events
// void EventHandler::waitAndCheck()
// {
//     char payload[1028];
//     Mqtt_URC_Event_t ret = tracker.WaitCheckMQTTURCEvent(payload, 2);

//     switch (ret)
//     {
//     case MQTT_RECV_DATA_EVENT:
//         DSerial.println("RECV_DATA_EVENT");
//         eventHandle(payload);
//         break;
//     case MQTT_STATUS_EVENT:
//         handleMQTTStatusEvent(payload);
//         break;
//     default:
//         break;
//     }
// }

// Handhabt das Aufwachen aus dem Schlafmodus
// bool EventHandler::handleWakeUp()
// {
//     if (!tracker.startModem())
//     {
//         DSerial.println("Fehler beim Starten des Modems.");
//         return false;
//     }

//     // _BG96.ScanCells(RAT, trackerModes.cells);

//     char apn_error[64];
//     if (!_BG96.TurnOnInternet(PDPIndex, apn_error))
//     {
//         DSerial.println(apn_error);
//         return false;
//     }

//     if (!tracker.initMQTT())
//     {
//         DSerial.println("Fehler beim Starten von MQTT.");
//         return false;
//     }

//     modemOff = false;
//     return true;
// }

// void EventHandler::collectData()
// {
//     // GNSS-Modus verwalten
//     if (trackerModes.GnssMode)
//     {
//         tracker.handleGNSSMode();
//     }
//     // else if (gnssData.isOn)
//     // {
//     //     tracker.TurnOff();
//     // }

//     // Temperatur- und Feuchtigkeitsdaten sammeln
//     if (trackerModes.TemperatureMode)
//     {
//         docInput["Temperature"] = tracker.readTemperature();
//         docInput["Humidity"] = tracker.readHumidity();
//     }

//     // Batteriestand erfassen
//     if (trackerModes.BatteryMode)
//     {
//         docInput["BatteryPercentage"] = tracker.calculateBatteryPercentage();
//         ;
//     }

//     // Zellinformationen erfassen
//     if (trackerModes.CellInfosMode)
//     {
//         JsonArray cellsArray = docInput["cells"].to<JsonArray>();
//         for (Cell *&cell : trackerModes.cells)
//         {
//             if (cell != nullptr)
//             {
//                 // JSON-Objekt für jede Zelle erstellen
//                 JsonObject cellObj = cellsArray.add<JsonObject>();
//                 cell->toJson(cellObj);
//             }
//         }
//     }

//     // Request-Modus setzen
//     trackerModes.updateRequestMode();
//     if (trackerModes.RequestMode)
//     {
//         docInput["RequestMode"] = true;
//     }
//     // Zeitstempel hinzufügen
//     docInput["Timestamp"] = tracker.getDateTime();
//     // Daten veröffentlichen
//     if (tracker.publishData("/pub"))
//     {
//         // delay(300);
//     }
//     // if (trackerModes.GeoFenMode)
//     // {
//     //     GEOFENCE_STATUS_t status = _BG96.getGeoFencingStatus(gnssData.geoID);
//     //     switch (status)
//     //     {
//     //     case OUTSIDE_GEOFENCE:
//     //         docInput["LeavingGeo"] = true;
//     //         if (publishData(pub_time, AT_LEAST_ONCE, "/notifications"))
//     //         {
//     //             delay(300);
//     //         }
//     //         break;
//     //     case INSIDE_GEOFENCE:
//     //     case NOFIX:
//     //         break;
//     //     default:
//     //         break;
//     //     }
//     // }
// }