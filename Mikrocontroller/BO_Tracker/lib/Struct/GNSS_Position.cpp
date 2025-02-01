#ifndef GNSS_POSITION_H
#define GNSS_POSITION_H

#include <ArduinoJson.h>
#include <math.h>

#define EARTH_RADIUS 6371000.0 // Erdradius in Metern

class GNSS_Position {
public:
    // Attribute
    float latitude;   // Breitengrad
    float longitude;  // Längengrad
    float hdop;       // Horizontal Dilution of Precision
    int nsat;         // Anzahl der Satelliten
    float accuracy;   // Genauigkeit in Metern
    float speed;

    // Konstruktoren
    GNSS_Position() : latitude(0.0), longitude(0.0), hdop(0.0), nsat(0), accuracy(0.0), speed(0.0) {}

    GNSS_Position(float lat, float lon, float hd, int sat, float acc, float sp) 
        : latitude(lat), longitude(lon), hdop(hd), nsat(sat), accuracy(acc), speed(sp) {}

    void toJson(JsonObject& json) const {
        json["latitude"] = latitude;
        json["longitude"] = longitude;
        json["hdop"] = hdop;
        json["nsat"] = nsat;
        json["accuracy"] = accuracy;
        json["speed"] = speed;
    }

    // Methode zur Initialisierung
    void InitPosition(float lat, float lon, float hd, int sat, float acc, float sp) {
        latitude = lat;
        longitude = lon;
        hdop = hd;
        nsat = sat;
        accuracy = acc;
        speed = sp / 3.6; // in m/s umrechnen
    }

    float getLatitude() const {
        return latitude;
    }

    float getLongitude() const {
        return longitude;
    }

    float getSpeed() const {
        return speed;
    }

    void setSpeed(float sp) {
        this->speed = sp;
    }

    float getAccuracy() const {
        return accuracy;
    }

    // Methode zur Berechnung der Distanz zwischen zwei Positionen (Haversine-Formel)
    float distanceTo(const GNSS_Position& other) const {
        float lat1 = latitude * M_PI / 180.0;
        float lon1 = longitude * M_PI / 180.0;
        float lat2 = other.latitude * M_PI / 180.0;
        float lon2 = other.longitude * M_PI / 180.0;

        float dlat = lat2 - lat1;
        float dlon = lon2 - lon1;

        float a = sin(dlat / 2) * sin(dlat / 2) +
                  cos(lat1) * cos(lat2) *
                  sin(dlon / 2) * sin(dlon / 2);
        float c = 2 * atan2(sqrt(a), sqrt(1 - a));

        return EARTH_RADIUS * c;
    }
};

#endif // GNSS_POSITION_H
