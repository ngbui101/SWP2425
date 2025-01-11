import { MongoClient, ObjectId } from 'mongodb';
import fetch from 'node-fetch';

function isValidTimestamp(timestamp) {
    const date = new Date(timestamp);
    const now = new Date();
    const diffInMinutes = (now - date) / 1000 / 60;

    // Überprüft, ob das Datum gültig ist und ob es nicht älter als 3 Minuten ist
    return date instanceof Date && !isNaN(date.getTime()) && diffInMinutes <= 3;
}

async function getModeFromMongo(trackerId, database) {
    const modeCollection = database.collection('mode');

    try {
        // Hole Modusinformationen
        const modeDoc = await modeCollection.findOne({
            tracker: new ObjectId(String(trackerId))
        });

        const modeData = modeDoc ? {
            GnssMode: modeDoc.GnssMode,
            CellInfosMode: modeDoc.CellInfosMode,
            BatteryMode: modeDoc.BatteryMode,
            TemperatureMode: modeDoc.TemperatureMode,
            NmeaMode: modeDoc.NmeaMode,
            frequenz: modeDoc.frequenz // => long/number
        } : {};

        return { modeData };
    } catch (error) {
        console.error("Error fetching mode and geofences from MongoDB:", error);
        return { modeData: {} };
    }
}

async function postToLambdaUrl(url, payload) {
    console.log(`Sende Daten an URL ${url} mit Payload:`, payload); // Logge die URL und Payload
    try {
        const response = await fetch(url, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        });

        if (!response.ok) {
            console.error(`Fehlerhafte Antwort von ${url}:`, await response.text());
        }
    } catch (error) {
        console.error(`Fehler beim Senden an URL ${url}:`, error);
    }
}


export const handler = async (event) => {
    const mongoURI = process.env.MONGO_URI;
    const client = new MongoClient(mongoURI);

    // Body parsen bei HTTP-Events
    const requestData = typeof event.body === 'string'
        ? JSON.parse(event.body)
        : event;

    // Relevante Felder aus dem Request
    const data = {
        IMEI: requestData.IMEI,
        Timestamp: requestData.Timestamp,
        Cells: requestData.cells,
        Temperature: requestData.Temperature,
        Humidity: requestData.Humidity,
        BatteryPercentage: requestData.BatteryPercentage,
        Gnss: requestData.gnss,
        GSA: requestData.GSA,
        GSV: requestData.GSV,
        frequenz: requestData.frequenz
    };

    if (!isValidTimestamp(data.Timestamp)) {
        console.warn(`Ungültiger Timestamp empfangen: ${data.Timestamp}. Ersetze durch aktuelle Zeit.`);
        data.Timestamp = new Date().toISOString();
    }

    const urls = {
        insertCellInfosUrl: process.env.INSERT_CELL_INFOS_URL,
        insertGNSSInfosUrl: process.env.INSERT_GNSS_INFOS_URL,
        insertEnvironmentDataUrl: process.env.INSERT_ENVIRONMENT_DATA_URL
    };

    await client.connect();
    const database = client.db('SOP');
    const trackersCollection = database.collection('trackers');
    const trackerDoc = await trackersCollection.findOne({ imei: data.IMEI });

    if (!trackerDoc) {
        console.error(`Tracker not found for IMEI: ${data.IMEI}`);
        return {
            statusCode: 404,
            body: JSON.stringify(`Tracker not found for IMEI: ${data.IMEI}`)
        };
    }

    const trackerId = trackerDoc._id;
    // Hole modeData & geofenceData
    const { modeData } = await getModeFromMongo(trackerId, database);

    // Checks, ob Daten gesendet wurden
    const isBatteryDataSent = (data.BatteryPercentage !== undefined);
    const isTempHumDataSent = (data.Temperature !== undefined || data.Humidity !== undefined);
    const isCellDataSent = (data.Cells)  //&& Array.isArray(data.Cells) && data.Cells.length > 0);
    const isGnssDataSent = (data.Gnss !== undefined);
    const isNmeaDataSent = (data.GSA !== undefined || data.GSV !== undefined);
    const isFrequenzSent = (data.frequenz !== undefined);

    // Mismatch-Objekt
    const mismatches = {};

    // BatteryMode
    if (isBatteryDataSent && modeData.BatteryMode === false) {
        mismatches.BatteryMode = false;
    }
    if (!isBatteryDataSent && modeData.BatteryMode === true) {
        mismatches.BatteryMode = true;
    }

    // TemperatureMode
    if (isTempHumDataSent && modeData.TemperatureMode === false) {
        mismatches.TemperatureMode = false;
    }
    if (!isTempHumDataSent && modeData.TemperatureMode === true) {
        mismatches.TemperatureMode = true;
    }

    // CellInfosMode
    if (isCellDataSent && modeData.CellInfosMode === false) {
        mismatches.CellInfosMode = false;
    }
    if (!isCellDataSent && modeData.CellInfosMode === true) {
        mismatches.CellInfosMode = true;
    }

    // GnssMode
    if (isGnssDataSent && modeData.GnssMode === false) {
        mismatches.GnssMode = false;
    }
    if (!isGnssDataSent && modeData.GnssMode === true) {
        mismatches.GnssMode = true;
    }

    // NmeaMode
    if (isNmeaDataSent && modeData.NmeaMode === false) {
        mismatches.NmeaMode = false;
    }
    if (!isNmeaDataSent && modeData.NmeaMode === true) {
        mismatches.NmeaMode = true;
    }

    if (isCellDataSent) {
        const payload = {
            IMEI: data.IMEI,
            Timestamp: data.Timestamp,
            trackerId: trackerId,
            Cells: data.Cells
        };
    
        // Fire-and-Forget: ohne auf den Abschluss zu warten
        postToLambdaUrl(urls.insertCellInfosUrl, payload)
            .catch((error) => console.error("Fehler beim Senden der Zellinformationen:", error));
    }
    
    if (isGnssDataSent && data.Gnss && data.Gnss.latitude !== undefined) {
        const payload = {
            IMEI: data.IMEI,
            Timestamp: data.Timestamp,
            trackerId: trackerId,
            gnss: data.Gnss
        };
    
        postToLambdaUrl(urls.insertGNSSInfosUrl, payload)
            .catch((error) => console.error("Fehler beim Senden der GNSS-Daten:", error));
    }
    
    if (isTempHumDataSent || isBatteryDataSent) {
        const payload = {
            IMEI: data.IMEI,
            Timestamp: data.Timestamp,
            trackerId: trackerId,
            BatteryPercentage: data.BatteryPercentage,
            Temperature: data.Temperature,
            Humidity: data.Humidity
        };
    
        postToLambdaUrl(urls.insertEnvironmentDataUrl, payload)
            .catch((error) => console.error("Fehler beim Senden der Umweltdaten:", error));
    }
    

    if (isFrequenzSent) {
        if (data.frequenz !== modeData.frequenz) {
            mismatches.frequenz = modeData.frequenz;
        }
    } else {
        if (modeData.frequenz !== undefined && modeData.frequenz !== null) {
            mismatches.frequenz = modeData.frequenz;
        }
    }

    if (Object.keys(mismatches).length > 0) {
        return {
            statusCode: 200,
            body: JSON.stringify(mismatches)
        };
    }
    await client.close();
    
    return {
        statusCode: 200,
        body: JSON.stringify({ valid: true })
    };
}
