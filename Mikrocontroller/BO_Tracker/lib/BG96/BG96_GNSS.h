/*
*Implementierung der GNSS-Funktion 
*mehr: Quectel_BG96_GNSS_AT_Commands_Manual_V1.1
 */

#ifndef __BG96_GNSS_H_
#define __BG96_GNSS_H_

#include "BG96_MQTT.h"
#include <cmath>

typedef enum gnss_work_mode{
    STAND_ALONE = 1,
    MS_BASED = 2,
    MS_ASSISTED = 3,
    SPEED_OPTIMAL = 4,
}GNSS_Work_Mode_t;

typedef enum xtraenable{
    DISABLE = 0,
    ENABLE = 1,
}GNSS_Xtra_Enable_t;

typedef enum gnss_constellation{
    GPS_ONLY = 0,
    GPS_GLONASS_BEIDOU_GALILEO = 1,
    GPS_GLONASS_BEIDOU = 2,
    GPS_GLONASS_GALILEO = 3,
    GPS_GLONASS = 4,
    GPS_BEIDOU_GALILEO = 5,
    GPS_GALILEO = 6,
}GNSS_Constellation_t;

typedef enum gnss_output_port{
	NOPORT = 1,
	USBNMEA = 2,
	UARTNMEA = 3,
}GNSS_OutputPort_t;

typedef enum gnss_deletetype{
	ALLNOXTRA = 0,
	NONEHOTSTART = 1,
	SOMEWARMSTART = 2,
    XTRA = 3,
}GNSS_Delete_t;

typedef enum nmea_sentences_type {
	GPGGA = 1,
	GPRMC = 2,
	GPGSV = 4,
	GPGSA = 8,
	GPVTG = 16,
}NMEA_Type_t;

typedef enum geofence_shape {
    CIRLE = 0,
    TRIANGLE = 2,
    QUADRANGLE = 3,
}GEOFENCE_SHAPE_t;

typedef enum geofence_mode {
    DISABLE_GEOFENCE = 0,
    ENTER_GEOFENCE = 1,
    EXIT_GEOFENCE = 2,
    ENTER_AND_EXIT_GEOFENCE = 3,
}GEOFENCE_MODE_t;
typedef enum geofence_status {
    NOFIX = 0,
    INSIDE_GEOFENCE = 1,
    OUTSIDE_GEOFENCE = 2,
}GEOFENCE_STATUS_t;

class _BG96_GNSS : public _BG96_MQTT
{
  public:
    _BG96_GNSS();

    ~_BG96_GNSS();

    _BG96_GNSS(Stream &atserial, Stream &dserial);

    bool SetGNSSConstellation(GNSS_Constellation_t constellation);

    bool SetGNSSEnableNMEASentences(bool enable);

    bool SetGNSSAutoRun(bool auto_run);

    bool TurnOnGNSS(GNSS_Work_Mode_t mode, Cmd_Status_t status);

    bool GetGNSSPositionInformation(char *position);

    // bool GetGnssJsonPositionInformation(JsonDocument &json, unsigned long starttime); 
    bool TurnOffGNSS();

    bool GetGNSSNMEASentences(NMEA_Type_t type, char *sentences);
	
    bool SetGNSSOutputPort(GNSS_OutputPort_t outport);

    bool EnableGpsOneXTRA(GNSS_Xtra_Enable_t mode,Cmd_Status_t status);

    bool InjectGpsOneXTRATime(const char* timey);

    bool InjectGpsOneXTRAData(const char* filename,Cmd_Status_t status, char *currentTimestamp);

    bool IsGpsOneXtraDataUptoDate();

    bool UpdateGpsOneXtraData(char *currentTimestamp);

    bool InitGpsOneXTRA(char *currentTimestamp);

    bool DeleteAssistanceData(GNSS_Delete_t deletetype);

    bool GetEstimationError(float &accuracy);

    bool AddGeoFence(unsigned int geoID, GEOFENCE_MODE_t mode, GEOFENCE_SHAPE_t shape, float latituide, float longituide, unsigned int radius);

    bool QueryGeoFence(unsigned int geoID);

    GEOFENCE_STATUS_t getGeoFencingStatus(unsigned int geoID);

    bool SetAGPSPlan(int mode = 0);
    
    bool SetAGPSUrl(const char *supurl);
    
    bool SetAGPSAPN(const char *apn);

    bool InitAGPS(const char *supurl, const char *apn);

   private:
    
   ///GPSOneXTRA Update Liste
    const char* xtra_links[3] = {
        "http://xtrapath1.izatcloud.net/xtra2.bin",
        "http://xtrapath2.izatcloud.net/xtra2.bin",
        "http://xtrapath3.izatcloud.net/xtra2.bin"
    };
};

#endif