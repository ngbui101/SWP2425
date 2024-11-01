import { MongoClient, ObjectId } from 'mongodb';

function convertTimestamp(timestamp) {
    const [datePart, timePartWithZone] = timestamp.split(",");
    const timePart = timePartWithZone.slice(0, 8);
    const [year, month, day] = datePart.split("/");
    const fullYear = `20${year}`;
    const isoString = `${fullYear}-${month}-${day}T${timePart}Z`;
    return new Date(isoString);
}

function parseGSA(gsaString) {
    const cleanString = gsaString.split('*')[0];
    const fields = cleanString.split(",");
    const pdop = parseFloat(fields[fields.length - 3]);
    const hdop = parseFloat(fields[fields.length - 2]);
    const vdop = parseFloat(fields[fields.length - 1]);

    return { pdop, hdop, vdop };
}


function calculateAverageSNR(gsvData) {
    const snrValues = [];
    const sentences = gsvData.split(/\$GPGSV/).filter(Boolean).map(s => "$GPGSV" + s);

    sentences.forEach(sentence => {
        if (sentence.startsWith("$GPGSV")) {
            const fields = sentence.split(",");
            for (let i = 4; i < fields.length; i += 4) {
                const snr = parseInt(fields[i + 3], 10);
                if (!isNaN(snr)) {
                    snrValues.push(snr);
                }
            }
        }
    });

    if (snrValues.length === 0) {
        return null;
    }

    const averageSNR = snrValues.reduce((acc, snr) => acc + snr, 0) / snrValues.length;
    return averageSNR;
}



function calculateAccuracy(hdop, pdop, vdop, nsat, averageSNR) {
    const sigmaH = 3;
    const sigmaP = 4;
    let ha = null, pa = null, totalAccuracy = null;

    if (nsat < 4) {
        ha = sigmaH * hdop * 2 * (3 / nsat) * (averageSNR/30);
        console.log(`2D Fix: Calculated horizontal accuracy (ha) = ${ha}`);
    } else {
        ha = sigmaH * hdop * (4 / nsat) * (averageSNR/30);
        pa = sigmaP * pdop * (4 / nsat) * (averageSNR/30);
    }

    if (ha !== null && pa !== null) {
        totalAccuracy = Math.sqrt(Math.pow(ha, 2) + Math.pow(vdop * sigmaH, 2));
        console.log(`Combined accuracy calculated as totalAccuracy = ${totalAccuracy}`);
    } else {
        totalAccuracy = ha || pa || null;
    }

    return {
        ha: ha || null,
        pa: pa || null,
        totalAccuracy: totalAccuracy || null
    };
}

export const handler = async (event) => {
    const mongoURI = process.env.MONGO_URI;
    const client = new MongoClient(mongoURI);

    const convertedTimestamp = convertTimestamp(event.Timestamp);

    if (!convertedTimestamp || isNaN(convertedTimestamp.getTime())) {
        return {
            statusCode: 400,
            body: JSON.stringify("Invalid timestamp format")
        };
    }

    const data = {
        IMEI: event.IMEI,
        Timestamp: convertedTimestamp,
        CellInfos: event.CellInfos,
        Temperature: event.Temperature,
        Humidity: event.Humidity,
        BatteryPercentage: event.BatteryPercentage,
        Position: event.Position,
        GSA: event.GSA,
        GSV: event.GSV,
        TimeToGetFirstFix: event.TimeToGetFirstFix
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

        if (data.Position && data.GSA && data.GSV) {
            const [_, latitudeStr, longitudeStr, hdopStr, __, fixStr, ___, ____, _____, ______, nsatStr] = data.Position.split(",");
            const latitude = parseFloat(latitudeStr);
            const longitude = parseFloat(longitudeStr);
            const hdop = parseFloat(hdopStr);
            const fix = parseInt(fixStr, 10);
            const nsat = parseInt(nsatStr, 10);

            const { pdop, vdop } = parseGSA(data.GSA);
            const averageSNR = calculateAverageSNR(data.GSV);
            const { ha, pa, totalAccuracy } = calculateAccuracy(hdop, pdop, vdop, nsat, averageSNR);

            documentsToInsert.push({
                imei: data.IMEI,
                mode: "GPS",
                latitude,
                longitude,
                gsa: data.GSA,
                gsv: data.GSV,
                tracker: new ObjectId(trackerId),
                accuracy: totalAccuracy,
                averageSNR: averageSNR,
                timeToGetFirstFix: data.TimeToGetFirstFix,
                createdAt: data.Timestamp,
                updatedAt: data.Timestamp
            });
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

        if (data.CellInfos) {
            documentsToInsert.push({
                imei: data.IMEI,
                mode: "CellInfos",
                cellinfo: data.CellInfos,
                tracker: new ObjectId(trackerId),
                createdAt: data.Timestamp,
                updatedAt: data.Timestamp
            });
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
