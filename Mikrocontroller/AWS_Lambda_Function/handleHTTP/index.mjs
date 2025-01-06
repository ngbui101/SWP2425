import { MongoClient, ObjectId } from 'mongodb';
import fetch from 'node-fetch';

async function getModeAndGeofencesFromMongo(trackerId, database) {
    const modeCollection = database.collection('mode');
    const geofencesCollection = database.collection('geofences');

    try {
        // Hole Modusinformationen
        const modeDoc = await modeCollection.findOne({ tracker: new ObjectId(String(trackerId)) });

        const modeData = modeDoc ? {
            GnssMode: modeDoc.GnssMode,
            CellInfosMode: modeDoc.CellInfosMode,
            BatteryMode: modeDoc.BatteryMode,
            TemperatureMode: modeDoc.TemperatureMode,
            NmeaMode: modeDoc.NmeaMode,
            frequenz: modeDoc.frequenz
        } : {};

        // Hole Geofence-Informationen
        const geofencesCursor = geofencesCollection.find({ tracker: new ObjectId(String(trackerId))});
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

    // Bei HTTP-Events kommt der Body meist als string, ggf. parsen
    const requestData = typeof event.body === 'string' ? JSON.parse(event.body) : event;

    const data = {
        IMEI: requestData.IMEI,
        Timestamp: requestData.Timestamp,
        Cells: requestData.cells,
        Temperature: requestData.Temperature,
        Humidity: requestData.Humidity,
        BatteryPercentage: requestData.BatteryPercentage,
        Position: requestData.Position,
        GSA: requestData.GSA,
        GSV: requestData.GSV,
        Accuracy: requestData.Accuracy,
        RequestMode: requestData.RequestMode,
        BatteryLow: requestData.BatteryLow,
        TimeToGetFirstFix: requestData.TimeToGetFirstFix
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
        const collection = database.collection('measurements');
        const documentsToInsert = [];

        if (data.Position) {
            const [_, latitudeStr, longitudeStr, hdopStr, __, fixStr, ___, ____, _____, ______, nsatStr] = data.Position.split(",");
            const latitude = parseFloat(latitudeStr);
            const longitude = parseFloat(longitudeStr);

            documentsToInsert.push({
                imei: data.IMEI,
                mode: "GPS",
                latitude,
                longitude,
                tracker: new ObjectId(trackerId),
                accuracy: data.Accuracy,
                timeToGetFirstFix: data.TimeToGetFirstFix,
                createdAt: data.Timestamp,
                updatedAt: data.Timestamp
            });
        }

        if (data.Cells && Array.isArray(data.Cells) && data.Cells.length > 0) {
            const location = await getCellLocationAndEstimatedPositionFromUnwiredLabs(data.Cells);
            if (location) {
                const radioType = data.Cells[0].radio || "unknown";

                const modeMap = {
                    "lte": "LTE",
                    "gsm": "GSM",
                    "nbiot": "NBIOT"
                };
                const mode = modeMap[radioType.toLowerCase()] || radioType.toUpperCase();

                documentsToInsert.push({
                    imei: data.IMEI,
                    mode: mode,
                    latitude: location.latitude,
                    longitude: location.longitude,
                    accuracy: location.accuracy,
                    tracker: new ObjectId(trackerId),
                    createdAt: data.Timestamp,
                    updatedAt: data.Timestamp
                });
            }
        }

        if (data.Temperature !== undefined || data.Humidity !== undefined) {
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

        if (data.BatteryPercentage !== undefined) {
            documentsToInsert.push({
                imei: data.IMEI,
                mode: "Battery",
                battery: data.BatteryPercentage,
                tracker: new ObjectId(trackerId),
                createdAt: data.Timestamp,
                updatedAt: data.Timestamp
            });
        }

        if (data.RequestMode === true) {
            const { modeData, geofenceData } = await getModeAndGeofencesFromMongo(trackerId, database);

            const payload = { ...modeData, geofences: geofenceData };
            
            // Anstatt publishToAwsIoT: direkter Return als HTTP-Response
            return {
                statusCode: 200,
                body: JSON.stringify(payload)
            };
        }

        if (documentsToInsert.length > 0) {
            await collection.insertMany(documentsToInsert);
            console.log(`${documentsToInsert.length} document(s) inserted.`);
        } else {
            console.log("No valid measurement data found; no documents were inserted.");
        }

        return {
            statusCode: 200,
            body: JSON.stringify('Data successfully processed and inserted')
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
