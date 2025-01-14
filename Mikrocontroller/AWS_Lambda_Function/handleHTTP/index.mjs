import { MongoClient, ObjectId } from 'mongodb';

function isValidTimestamp(timestamp) {
    const date = new Date(timestamp);
    const now = new Date();
    const diffInMinutes = (now - date) / 1000 / 60;

    return (
        date instanceof Date &&
        !isNaN(date.getTime()) &&
        diffInMinutes <= 3
    );
}
async function invokeDataHandler(data) {
    const dataHandlerUrl = process.env.DATA_HANDLER_URL; 

    const response = await fetch(dataHandlerUrl, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(data)
    });

    return response.json();
}

async function getModeFromMongo(trackerId, database) {
    const modeCollection = database.collection('mode');
    try {
        const modeDoc = await modeCollection.findOne({
            tracker: new ObjectId(String(trackerId))
        });

        return modeDoc
            ? {
                GnssMode: modeDoc.GnssMode,
                CellInfosMode: modeDoc.CellInfosMode,
                BatteryMode: modeDoc.BatteryMode,
                TemperatureMode: modeDoc.TemperatureMode,
                NmeaMode: modeDoc.NmeaMode,
                frequenz: modeDoc.frequenz
            }
            : {};
    } catch (error) {
        console.error('Error fetching mode from MongoDB:', error);
        return {};
    }
}

export const handler = async (event) => {
    const mongoURI = process.env.MONGO_URI;
    const client = new MongoClient(mongoURI);

    // Body parsen
    const requestData = typeof event.body === 'string'
        ? JSON.parse(event.body)
        : event;

    // Relevante Felder aus dem Request
    const data = {
        IMEI: requestData.IMEI,
        Timestamp: requestData.Timestamp,
        Ping: requestData.Ping,
        Cells: requestData.cells,
        Temperature: requestData.Temperature,
        Humidity: requestData.Humidity,
        BatteryPercentage: requestData.BatteryPercentage,
        Gnss: requestData.gnss,
        GSA: requestData.GSA,
        GSV: requestData.GSV,
        frequenz: requestData.frequenz
    };
    
    if(data.Ping !== undefined ){
        return {
            statusCode: 200,
            body: JSON.stringify("OK")
        };
    }

    // Timestamp validieren
    if (!isValidTimestamp(data.Timestamp)) {
        console.warn(`Ungültiger Timestamp empfangen: ${data.Timestamp}. Ersetze durch aktuelle Zeit.`);
        data.Timestamp = new Date().toISOString();
    }

    try {
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
        const modeData = await getModeFromMongo(trackerId, database);

        // Checks, ob Daten gesendet wurden
        const isBatteryDataSent = (data.BatteryPercentage !== undefined);
        const isTempHumDataSent = (data.Temperature !== undefined || data.Humidity !== undefined);
        const isCellDataSent = (data.Cells);
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

        // Frequenz
        if (isFrequenzSent) {
            if (data.frequenz !== modeData.frequenz) {
                mismatches.frequenz = modeData.frequenz;
            }
        } else {
            if (modeData.frequenz !== undefined && modeData.frequenz !== null) {
                mismatches.frequenz = modeData.frequenz;
            }
        }

        // Bei Mismatch: sofortige Rückgabe
        if (Object.keys(mismatches).length > 0) {
            return {
                statusCode: 200,
                body: JSON.stringify(mismatches)
            };
        }

        // Wenn kein Mismatch => Aufruf der zweiten Funktion
        // Die zweite Funktion übernimmt das „Daten versenden“ (Cells, Gnss, etc.)
        const resultFromDataHandler = await invokeDataHandler({
            IMEI: data.IMEI,
            Timestamp: data.Timestamp,
            trackerId,
            Cells: data.Cells,
            Gnss: data.Gnss,
            BatteryPercentage: data.BatteryPercentage,
            Temperature: data.Temperature,
            Humidity: data.Humidity,
            GSA: data.GSA,
            GSV: data.GSV
        });

        // Ergebnis an den Client zurück
        return {
            statusCode: 200,
            body: JSON.stringify({
                valid: true,
                dataHandlerResponse: resultFromDataHandler
            })
        };

    } catch (error) {
        console.error('Error in ModeHandler:', error);
        return {
            statusCode: 500,
            body: JSON.stringify('Internal Server Error')
        };
    } finally {
        await client.close();
    }
};
