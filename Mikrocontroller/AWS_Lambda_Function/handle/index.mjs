import { MongoClient, ObjectId } from 'mongodb';

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
    const response = await fetch(url, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(payload)
    });

    if (!response.ok) {
        throw new Error(`Request to ${url} failed with status: ${response.status}`);
    }

    return response.json();
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

    const urls = {
        insertCellInfosUrl: process.env.INSERT_CELL_INFOS_URL,
        insertGNSSInfosUrl: process.env.INSERT_GNSS_INFOS_URL,
        insertEnvironmentDataUrl: process.env.INSERT_ENVIRONMENT_DATA_URL
    };

    const results = [];


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
    const isGnssDataSent = (data.gnss !== undefined);
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
            Cells: data.Cells
        };
        postToLambdaUrl(urls.insertCellInfosUrl, payload);
    }
    if (isGnssDataSent && data.gnss.latitude != undefined) {
        const payload = {
            IMEI: data.IMEI,
            Timestamp: data.Timestamp,
            gnss: data.gnss
        };
        postToLambdaUrl(urls.insertGNSSInfosUrl, payload);
    }
    if (isTempHumDataSent || isBatteryDataSent) {
        const payload = {
            IMEI: data.IMEI,
            Timestamp: data.Timestamp,
            BatteryPercentage: data.BatteryPercentage,
            Temperature: data.Temperature,
            Humidity: data.Humidity
        };
        postToLambdaUrl(urls.insertEnvironmentDataUrl, payload);
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
    // Am Ende: alles OK => {"valid": true}
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
