#ifndef CELL_H
#define CELL_H

#include <Arduino.h>
#include <ArduinoJson.h>

// Basisklasse Cell
class Cell
{
protected:
    String state; // Zellenstatus
    String rat;   // Mobilfunktechnologie ("GSM", "LTE", etc.)
    int mcc;      // Mobile Country Code
    int mnc;      // Mobile Network Code
    long cellid;  // Zellen-ID
    int signal;   // Signalstärke (in dBm)

public:
    Cell(const String &state, const String &rat, int mcc, int mnc, long cellid, int signal)
        : state(state), rat(rat), mcc(mcc), mnc(mnc), cellid(cellid), signal(signal) {}

    virtual ~Cell() {}

    // Getter
    long getCellID() const { return cellid; }
    String getRat() const { return rat; }
    int getSignal() const { return signal; }

    // Methode, um die Basisattribute zurückzugeben
    virtual String getDetails() const
    {
        return ", RAT: " + rat +
               ", MCC: " + String(mcc) +
               ", MNC: " + String(mnc) +
               ", Cell ID: " + String(cellid) +
               ", Signal: " + String(signal);
    }

    // JSON-Serialisierung
    virtual void toJson(JsonObject &json) const
    {
        json["state"] = state;
        json["rat"] = rat;
        json["mcc"] = mcc;
        json["mnc"] = mnc;
        json["cellid"] = cellid;
        json["signal"] = signal;
    }
};

// GSMCell erbt von Cell
class GSMCell : public Cell
{
private:
    int lac;   // Location Area Code
    int bsic;  // Base Station Identity Code
    int arfcn; // Absolute Radio Frequency Channel Number
    int band;  // Frequenzband
    int rxlev; // Empfangsleistung (Wert zwischen 0 und 63)

    // Umrechnungsfunktion für RxLev in dBm
    static int calculateSignal(int rxlev)
    {
        return -110 + rxlev; // Standard-Umrechnung: dBm = -110 + RxLev
    }

public:
    GSMCell(const String &state, const String &rat, int mcc, int mnc, long cellid, int lac,
            int bsic, int arfcn, int band, int rxlev)
        : Cell(state, rat, mcc, mnc, cellid, calculateSignal(rxlev)),
          lac(lac), bsic(bsic), arfcn(arfcn), band(band), rxlev(rxlev) {}

    // Getter
    int getLac() const { return lac; }
    int getBsic() const { return bsic; }
    int getArfcn() const { return arfcn; }

    // Detail-String erweitern
    String getDetails() const override
    {
        return Cell::getDetails() +
               ", LAC: " + String(lac) +
               ", BSIC: " + String(bsic) +
               ", ARFCN: " + String(arfcn) +
               ", RxLev: " + String(rxlev);
    }

    // JSON-Serialisierung erweitern
    void toJson(JsonObject &json) const override
    {
        Cell::toJson(json);
        json["lac"] = lac;
        json["bsic"] = bsic;
        json["arfcn"] = arfcn;
        json["band"] = band;
        json["rxlev"] = rxlev;
    }
};

// LTECell erbt von Cell
class LTECell : public Cell
{
private:
    bool isTdd;      // TDD oder FDD
    int pcid;        // Physical Cell ID
    int earfcn;      // E-UTRA Absolute Radio Frequency Channel Number
    int freqBandInd; // Frequenzband-Indikator
    int ulBandwidth; // Uplink-Bandbreite
    int dlBandwidth; // Downlink-Bandbreite
    int tac;         // Tracking Area Code
    int rsrp;        // Reference Signal Received Power
    int rsrq;        // Reference Signal Received Quality
    int rssi;        // Received Signal Strength Indicator
    int sinr;        // Signal-to-Interference-plus-Noise Ratio
    int srxlev;      // Signal Level

public:
    LTECell(const String &state, const String &rat, int mcc, int mnc, long cellid, bool isTdd,
            int pcid, int earfcn, int freqBandInd, int ulBandwidth, int dlBandwidth, int tac,
            int rsrp, int rsrq, int rssi, int sinr, int srxlev)
        : Cell(state, rat, mcc, mnc, cellid, rsrp), isTdd(isTdd), pcid(pcid), earfcn(earfcn),
          freqBandInd(freqBandInd), ulBandwidth(ulBandwidth), dlBandwidth(dlBandwidth), tac(tac),
          rsrp(rsrp), rsrq(rsrq), rssi(rssi), sinr(sinr), srxlev(srxlev) {}

    // Getter
    int getPcid() const { return pcid; }
    int getTac() const { return tac; }
    int getRsrp() const { return rsrp; }
    int getRsrq() const { return rsrq; }

    // Detail-String erweitern
    String getDetails() const override
    {
        return Cell::getDetails() +
               ", PCID: " + String(pcid) +
               ", RSRP: " + String(rsrp) +
               ", RSRQ: " + String(rsrq) +
               ", RSSI: " + String(rssi);
    }

    // JSON-Serialisierung erweitern
    void toJson(JsonObject &json) const override
    {
        Cell::toJson(json);
        json["pcid"] = pcid;
        json["tac"] = tac;
        json["rsrp"] = rsrp;
        json["rsrq"] = rsrq;
        json["rssi"] = rssi;
        json["sinr"] = sinr;
    }
};

#endif // CELL_H
