import { MongoClient, ObjectId } from 'mongodb';
import fetch from 'node-fetch';
import AWS from 'aws-sdk';

const iotData = new AWS.IotData({ endpoint: process.env.AWS_IOT_ENDPOINT });

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


async function publishToAwsIoT(imei, modeData) {
    const topic = `tracker/${imei}/sub`;
    const modePayload = JSON.stringify(modeData);

    try {
        await iotData.publish({
            topic,
            qos: 1,
            payload: modePayload
        }).promise();
        console.log(`Message published to topic ${topic}`);
    } catch (err) {
        console.error("Error publishing to AWS IoT:", err);
    }
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

            await publishToAwsIoT(data.IMEI,mismatches);

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
