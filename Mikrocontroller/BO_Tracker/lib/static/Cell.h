#ifndef CELL_H
#define CELL_H

#include <ArduinoJson.h>

// Basisklasse Cell
class Cell
{
protected:
    char rat[8];    // Mobilfunktechnologie ("GSM", "LTE", etc.)
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
    virtual ~Cell()
    {
    }
    // Methode, die alle Attribute als JSON-Objekt ausgibt
    virtual void toJson(JsonObject &json) const
    {
        if (strcmp(rat, "cat-m") == 0){
            json["radio"] = "lte";
        }else if( strcmp(rat, "nb-iot") == 0){
            json["radio"] = "nbiot";
        }else
        {
            json["radio"] = "gsm";
        }
        json["mcc"] = mcc;
        json["mnc"] = mnc;
        json["lac"] = lac;
        json["cid"] = cellid;
        json["signal"] = signal;
    }
    const char* getRat() const { return rat; }
    int getMnc() const { return mnc; }
    int getLac() const { return lac; }
    long getCellID() const { return cellid; }
    int getSignal() const { return signal; }

    int getOperator() { return (mcc*100+mnc);}
};
#endif 
