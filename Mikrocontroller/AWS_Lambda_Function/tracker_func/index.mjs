import { MongoClient, ObjectId } from 'mongodb';
import fetch from 'node-fetch';
import AWS from 'aws-sdk';


const iotData = new AWS.IotData({ endpoint: process.env.AWS_IOT_ENDPOINT });

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
        // Ensure trackerId is a string
        const modeDoc = await modeCollection.findOne({ tracker: new ObjectId(String(trackerId)) });
        
        if (modeDoc) {
            return {
                GnssMode: modeDoc.GnssMode ?? false,
                CellInfosMode: modeDoc.CellInfosMode ?? false,
                BatteryMode: modeDoc.BatteryMode ?? false,
                TemperatureMode: modeDoc.TemperatureMode ?? false,
                NmeaMode: modeDoc.NmeaMode ?? false,
                frequenz: modeDoc.frequenz ?? 5000 
            };
        } else {
            console.log(`Mode document not found for tracker ID: ${trackerId}`);
            return null;
        }
    } catch (error) {
        console.error("Error fetching mode from MongoDB:", error);
        return null;
    }
}

export default getModeFromMongo;


function convertTimestamp(timestamp) {
    try {
        const [datePart, timePartWithZone] = timestamp.split(",");
        const timePart = timePartWithZone.slice(0, 8);
        const [year, month, day] = datePart.split("/");
        const fullYear = `20${year}`;
        const isoString = `${fullYear}-${month}-${day}T${timePart}Z`;
        return new Date(isoString);
    } catch (error) {
        console.error("Timestamp conversion error:", error);
        return null;
    }
}

function convertBandwidth(bandwidthCode) {
    switch (bandwidthCode) {
        case '0': return 1.4;
        case '1': return 3;
        case '2': return 5;
        case '3': return 10;
        case '4': return 15;
        case '5': return 20;
        default: return null;  // Gibt `null` zurück, wenn der Code ungültig ist
    }
}

function calculateDistanceFromRSRP(rsrp, frequencyMHz) {
    if (isNaN(rsrp) || isNaN(frequencyMHz) || frequencyMHz <= 0) {
        console.error("Invalid RSRP or frequency provided for distance calculation.");
        return null;
    }
    const frequencyHz = frequencyMHz * 1e6;
    const speedOfLight = 3e8;  

    // Referenzverlust L0 bei einer Distanz von 1 Meter
    const L0 = 20 * Math.log10((4 * Math.PI * frequencyHz) / speedOfLight);

    // Typischer Pfadverlustexponent für städtische Gebiete
    const pathLossExponent = 3.5;

    // Berechnung der Distanz basierend auf RSRP
    const distance = Math.pow(10, (rsrp - L0) / (10 * pathLossExponent));
    console.log("Distanz:", distance);
    return distance > 0 ? distance : null;  // Sicherheitsüberprüfung für realistische Werte
}




function getRandomOffset(lat, lon, distance) {
    if (isNaN(distance) || distance <= 0) {
        console.error("Invalid distance for random offset calculation.");
        return { latitude: lat, longitude: lon };
    }

    const earthRadius = 6371000; // Erdradius in Metern

    const angle = Math.random() * 2 * Math.PI;
    const deltaLat = (distance * Math.cos(angle)) / earthRadius;
    const deltaLon = (distance * Math.sin(angle)) / (earthRadius * Math.cos((lat * Math.PI) / 180));

    const newLat = lat + (deltaLat * 180) / Math.PI;
    const newLon = lon + (deltaLon * 180) / Math.PI;

    return {
        latitude: isNaN(newLat) ? lat : newLat,
        longitude: isNaN(newLon) ? lon : newLon
    };
}


async function getCellLocationAndEstimatedPosition(cellInfo) {
    const cellInfoParts = cellInfo.split(",");
    const mcc = cellInfoParts[4];
    const mnc = cellInfoParts[5];
    const cellid = parseInt(cellInfoParts[6], 16);
    const lac = parseInt(cellInfoParts[12], 16);
    const rsrp = parseFloat(cellInfoParts[13]);
    const frequencyMHz = parseFloat(cellInfoParts[8]);

    // Debug-Ausgabe der Eingabewerte
    console.log("Parsed Cell Information:", {
        mcc, mnc, cellid, lac, rsrp, frequencyMHz
    });

    const apiKey = process.env.OPENCELLID_API_KEY;
    const url = `https://opencellid.org/cell/get?key=${apiKey}&mcc=${mcc}&mnc=${mnc}&lac=${lac}&cellid=${cellid}&format=json`;

    try {
        const response = await fetch(url);
        if (!response.ok) {
            throw new Error(`Failed to fetch location data: ${response.statusText}`);
        }
        const data = await response.json();

        console.log("Fetched Cell Location Data:", data); // Debug-Ausgabe der Zellstandortdaten

        const distance = calculateDistanceFromRSRP(rsrp, frequencyMHz);
        if (distance === null) {
            console.error("Unable to calculate distance, setting default accuracy.");
            return {
                latitude: data.lat,
                longitude: data.lon,
                accuracy: 1000 // Setze eine Standard-Genauigkeit in Metern
            };
        }

        const estimatedPosition = getRandomOffset(data.lat, data.lon, distance);
        return {
            latitude: estimatedPosition.latitude,
            longitude: estimatedPosition.longitude,
            accuracy: distance * 2
        };
    } catch (error) {
        console.error("Error fetching cell location:", error);
        return null;
    }
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
        Accuracy: event.Accuracy,
        RequestMode:event.RequestMode,
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
            // const hdop = parseFloat(hdopStr);
            // const fix = parseInt(fixStr, 10);
            // const nsat = parseInt(nsatStr, 10);
            // const { pdop, vdop } = parseGSA(data.GSA);
            // const averageSNR = calculateAverageSNR(data.GSV);
            // const { ha, pa, totalAccuracy } = calculateAccuracy(hdop, pdop, vdop, nsat, averageSNR);

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

        if (data.CellInfos) {
            const cellLocation = await getCellLocationAndEstimatedPosition(data.CellInfos);
            if (cellLocation) {
                documentsToInsert.push({
                    imei: data.IMEI,
                    mode: "LTE",
                    latitude: cellLocation.latitude,
                    longitude: cellLocation.longitude,
                    accuracy: cellLocation.accuracy,
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
        
        if(data.RequestMode){
            const modeData = await getModeFromMongo(trackerId, database);
            if (modeData) {
                await publishToAwsIoT(data.IMEI, modeData);
            }
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
