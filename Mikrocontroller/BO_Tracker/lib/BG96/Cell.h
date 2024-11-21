#ifndef CELL_H
#define CELL_H

#include <ArduinoJson.h>

// Basisklasse Cell
class Cell
{
protected:
    char rat[4];    // Mobilfunktechnologie ("GSM", "LTE", etc.)
    int mcc;        // Mobile Country Code
    int mnc;        // Mobile Network Code
    int lac;        // Location Area Code
    long cellid;    // Zellen-ID
    int signal;     // Signalstärke (in dBm)

public:
    // Konstruktor
    Cell(const char* rat_, int mcc_, int mnc_, int lac_, long cellid_, int signal_)
    {
        strncpy(rat, rat_, sizeof(rat) - 1);
        rat[sizeof(rat) - 1] = '\0';
        mcc = mcc_;
        mnc = mnc_;
        lac = lac_;
        cellid = cellid_;
        signal = signal_;
    }

    // Methode, die alle Attribute als JSON-Objekt ausgibt
    virtual void toJson(JsonObject &json) const
    {
        json["radio"] = rat;
        json["mcc"] = mcc;
        json["mnc"] = mnc;
        json["lac"] = lac;
        json["cellid"] = cellid;
        json["signal"] = signal;
    }
    long getCellID() const
    {
        return cellid;
    }
};

#endif // CELL_H
