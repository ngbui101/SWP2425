import { MongoClient, ObjectId } from 'mongodb';

// Konvertiert NMEA-Format zu Dezimalgrad
function parseNMEACoordinates(nmeaString) {
    const match = nmeaString.match(/^\$GPGGA,\d+\.\d+,(.*),(N|S),(.*),(E|W),/);
    if (!match) return null;

    const [latitudeRaw, latitudeDir, longitudeRaw, longitudeDir] = match.slice(1);

    const latitude = parseFloat(latitudeRaw.slice(0, 2)) + parseFloat(latitudeRaw.slice(2)) / 60;
    const longitude = parseFloat(longitudeRaw.slice(0, 3)) + parseFloat(longitudeRaw.slice(3)) / 60;

    return {
        latitude: latitudeDir === 'S' ? -latitude : latitude,
        longitude: longitudeDir === 'W' ? -longitude : longitude,
    };
}

function convertTimestamp(timestamp) {
    const [datePart, timePartWithZone] = timestamp.split(",");
    const timePart = timePartWithZone.slice(0, 8);
    const [year, month, day] = datePart.split("/");
    const fullYear = `20${year}`;
    const isoString = `${fullYear}-${month}-${day}T${timePart}Z`;
    return new Date(isoString);
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
        NMEA: event.NMEA,
        TimeToGetFirstFix: event.TimeToGetFirstFix
    };

    console.log("Received IMEI: ", data.IMEI);
    console.log("Converted Timestamp: ", data.Timestamp);

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

        // Separate MongoDB documents for each type of data received
        const collection = database.collection('measurements');
        
        // 1. GPS-Daten verarbeiten
        if (data.NMEA) {
            const coordinates = parseNMEACoordinates(data.NMEA);
            if (coordinates) {
                const gpsData = {
                    imei: data.IMEI,
                    mode: "GPS",
                    latitude: coordinates.latitude,
                    longitude: coordinates.longitude,
                    nmea: data.NMEA,
                    tracker: new ObjectId(trackerId),
                    timeToGetFirstFix: data.TimeToGetFirstFix,
                    createdAt: data.Timestamp,
                    updatedAt: data.Timestamp
                };
                await collection.insertOne(gpsData);
                console.log("GPS data inserted");
            } else {
                console.error("Invalid NMEA format");
            }
        }

        // 2. Temperatur und Feuchtigkeit verarbeiten
        if (data.Temperature !== undefined || data.Humidity !== undefined) {
            const tempHumidityData = {
                imei: data.IMEI,
                mode: "TemperatureHumidity",
                temperature: data.Temperature,
                humidity: data.Humidity,
                tracker: new ObjectId(trackerId),
                createdAt: data.Timestamp,
                updatedAt: data.Timestamp
            };
            await collection.insertOne(tempHumidityData);
            console.log("Temperature and Humidity data inserted");
        }

        // 3. Batteriedaten verarbeiten
        if (data.BatteryPercentage !== undefined) {
            const batteryData = {
                imei: data.IMEI,
                mode: "Battery",
                battery: data.BatteryPercentage,
                tracker: new ObjectId(trackerId),
                createdAt: data.Timestamp,
                updatedAt: data.Timestamp
            };
            await collection.insertOne(batteryData);
            console.log("Battery data inserted");
        }

        // 4. Zellinformationen verarbeiten
        if (data.CellInfos) {
            const cellInfoData = {
                imei: data.IMEI,
                mode: "CellInfos",
                cellinfo: data.CellInfos,
                tracker: new ObjectId(trackerId),
                createdAt: data.Timestamp,
                updatedAt: data.Timestamp
            };
            await collection.insertOne(cellInfoData);
            console.log("Cell info data inserted");
        }

        return {
            statusCode: 200,
            body: JSON.stringify('Data successfully inserted')
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
