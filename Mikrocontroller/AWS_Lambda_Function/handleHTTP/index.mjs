import { MongoClient, ObjectId } from 'mongodb';
import fetch from 'node-fetch';

async function getModeAndGeofencesFromMongo(trackerId, database) {
    const modeCollection = database.collection('mode');
    const geofencesCollection = database.collection('geofences');

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

        // Hole Geofence-Informationen
        const geofencesCursor = geofencesCollection.find({
            tracker: new ObjectId(String(trackerId))
        });
        const geofences = await geofencesCursor.toArray();
        const geofenceData = geofences.map(geo => ({
            geoRadius: parseInt(geo.radius),
            geoLongitude: parseFloat(geo.longitude),
            geoLatitude: parseFloat(geo.latitude),
            GeoFenMode: geo.active
        }));

        return { modeData, geofenceData };
    } catch (error) {
        console.error("Error fetching mode and geofences from MongoDB:", error);
        return { modeData: {}, geofenceData: [] };
    }
}


async function getCellLocationAndEstimatedPositionFromUnwiredLabs(cells) {
    const apiUrl = "https://eu1.unwiredlabs.com/v2/process";
    const token = "pk.11e2626f93a4d4ee0bb5ab37ee6b25a4";

    if (!Array.isArray(cells) || cells.length === 0) {
        console.error("No valid cell information provided.");
        return null;
    }

    // Ensure that each cell has the required fields
    const requiredFields = ['radio', 'mcc', 'mnc', 'lac', 'cid'];
    for (const cell of cells) {
        for (const field of requiredFields) {
            if (!(field in cell)) {
                console.error(`Cell is missing required field '${field}':`, cell);
                return null;
            }
        }
    }

    const payload = {
        token,
        cells: cells.slice(0, 6),
        address: 1
    };

    try {
        const response = await fetch(apiUrl, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(payload)
        });

        if (!response.ok) {
            throw new Error(`API error: ${response.statusText}`);
        }

        const responseData = await response.json();
        if (responseData.status === "ok") {
            console.log("Location data from Unwired Labs API:", responseData);
            return {
                latitude: responseData.lat,
                longitude: responseData.lon,
                accuracy: responseData.accuracy / 5
            };
        } else {
            console.error("Unwired Labs API returned an error:", responseData);
            return null;
        }
    } catch (error) {
        console.error("Error calling Unwired Labs API:", error);
        return null;
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
        gnss: requestData.gnss,
        GSA: requestData.GSA,
        GSV: requestData.GSV,
        Accuracy: requestData.Accuracy,
        RequestMode: requestData.RequestMode,
        BatteryLow: requestData.BatteryLow,
        TimeToGetFirstFix: requestData.TimeToGetFirstFix,
        gnss: requestData.gnss,        
        frequenz: requestData.frequenz 
    };

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
        // Hole modeData & geofenceData
        const { modeData, geofenceData } = await getModeAndGeofencesFromMongo(trackerId, database);

        // Checks, ob Daten gesendet wurden
        const isBatteryDataSent = (data.BatteryPercentage !== undefined);
        const isTempHumDataSent  = (data.Temperature !== undefined || data.Humidity !== undefined);
        const isCellDataSent     = (data.Cells && Array.isArray(data.Cells) && data.Cells.length > 0);
        const isGnssDataSent     = (data.gnss !== undefined);
        const isNmeaDataSent     = (data.GSA !== undefined || data.GSV !== undefined);
        const isFrequenzSent     = (data.frequenz !== undefined);

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
        // Hier: Wenn Frequenz gesendet, aber != modeData.frequenz => mismatch
        //       Wenn Frequenz nicht gesendet, aber modeData.frequenz != null => mismatch
        if (isFrequenzSent) {
            if (data.frequenz !== modeData.frequenz) {
                // Wir geben hier in der Antwort die **erwartete** Frequenz als number zurück
                mismatches.frequenz = modeData.frequenz; 
            }
        } else {
            // Frequenz gar nicht gesendet
            if (modeData.frequenz !== undefined && modeData.frequenz !== null) {
                mismatches.frequenz = modeData.frequenz;
            }
        }

        // Falls Mismatch => zurückgeben mit Status 200
        // (Daten werden NICHT inserted)
        if (Object.keys(mismatches).length > 0) {
            return {
                statusCode: 200,
                body: JSON.stringify(mismatches)
            };
        }

        // Wenn RequestMode=true, soll Mode-Data & Geofences zurückkommen (Option)
        if (data.RequestMode === true) {
            const payload = { ...modeData, geofences: geofenceData };
            return {
                statusCode: 200,
                body: JSON.stringify(payload)
            };
        }

        // => Kein Mismatch => Daten verarbeiten
        const collection = database.collection('measurements');
        const documentsToInsert = [];

        // GNSS
        if (isGnssDataSent) {
            documentsToInsert.push({
                imei: data.IMEI,
                mode: "GPS",
                latitude: data.gnss.latitude,
                longitude: data.gnss.longitude,
                hdop: data.gnss.hdop,
                nsat: data.gnss.nsat,
                tracker: new ObjectId(trackerId),
                accuracy: data.gnss.accuracy,
                timeToGetFirstFix: data.gnss.TTFF,
                createdAt: data.Timestamp,
                updatedAt: data.Timestamp
            });
        }

        // Cell
        if (isCellDataSent) {
            const location = await getCellLocationAndEstimatedPositionFromUnwiredLabs(data.Cells);
            if (location) {
                const radioType = data.Cells[0].radio || "unknown";
                const modeMap = {
                    "lte": "LTE",
                    "gsm": "GSM",
                    "nbiot": "NBIOT"
                };
                const resolvedMode = modeMap[radioType.toLowerCase()] || radioType.toUpperCase();

                documentsToInsert.push({
                    imei: data.IMEI,
                    mode: "LTE",
                    latitude: location.latitude,
                    longitude: location.longitude,
                    accuracy: location.accuracy,
                    tracker: new ObjectId(trackerId),
                    createdAt: data.Timestamp,
                    updatedAt: data.Timestamp
                });
            }
        }

        // Temperature/Humidity
        if (isTempHumDataSent) {
            documentsToInsert.push({
                imei: data.IMEI,
                mode: "TemperatureHumidity",
                temperature: data.Temperature,
                humidity: data.Humidity,
                tracker: new ObjectId(trackerId),
                createdAt: data.Timestamp,
                updatedAt: data.Timestamp
            });
        }

        // Battery
        if (isBatteryDataSent) {
            documentsToInsert.push({
                imei: data.IMEI,
                mode: "Battery",
                battery: data.BatteryPercentage,
                tracker: new ObjectId(trackerId),
                createdAt: data.Timestamp,
                updatedAt: data.Timestamp
            });
        }

        // Daten einfügen
        if (documentsToInsert.length > 0) {
            await collection.insertMany(documentsToInsert);
            console.log(`${documentsToInsert.length} document(s) inserted.`);
        } else {
            console.log("No valid measurement data found; no documents were inserted.");
        }

        // Am Ende: alles OK => {"valid": true}
        return {
            statusCode: 200,
            body: JSON.stringify({ valid: true })
        };
    } catch (error) {
        console.error("Error inserting data: ", error);
        return {
            statusCode: 500,
            body: JSON.stringify('Error inserting data')
        };
    } finally {
        await client.close();
    }
};
