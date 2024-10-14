import { MongoClient, ObjectId } from 'mongodb';

// Funktion zum Konvertieren des Zeitstempels in das ISO-Format "yy/MM/dd,hh:mm:ss±zz"
function convertTimestamp(timestamp) {
    // Trenne Datum und Zeit
    const [datePart, timePartWithZone] = timestamp.split(",");
    const timePart = timePartWithZone.slice(0, 8); // Nur die Uhrzeit (ohne Zeitzone)
    
    // Trenne Datum in Tag, Monat und Jahr
    const [year, month, day] = datePart.split("/");
    const fullYear = `20${year}`;  // Füge '20' für das Jahr hinzu

    // Erzeuge das ISO-Format in UTC
    const isoString = `${fullYear}-${month}-${day}T${timePart}Z`;  // Z für UTC
    
    // Erzeuge ein Date-Objekt in UTC
    return new Date(isoString);
}

export const handler = async (event) => {
    const mongoURI = process.env.MONGO_URI;  
    const client = new MongoClient(mongoURI);

    // Konvertiere den Timestamp in das richtige Format
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
        BatteryPercentage: event.BatteryPercentage,
        Position: event.Position,
        Humidity: event.Humidity 
    };

    console.log("Received IMEI: ", data.IMEI);
    console.log("Converted Timestamp: ", data.Timestamp);

    try {
        // Verbindung zu MongoDB herstellen
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

        const positionParts = data.Position.split(",");
        const latitude = parseFloat(positionParts[1]);  
        const longitude = parseFloat(positionParts[2]);  

        const mongoData = {
            imei: data.IMEI, 
            mode: "GPS",
            latitude: latitude,
            longitude: longitude,
            temperature: data.Temperature,
            cellinfo: data.CellInfos,
            humidity: data.Humidity,
            battery: data.BatteryPercentage,
            tracker: new ObjectId(trackerId),
            createdAt: data.Timestamp,  // Verwende den konvertierten Timestamp
            updatedAt: data.Timestamp   // Verwende den konvertierten Timestamp
        };

        const collection = database.collection('measurements'); 
        const result = await collection.insertOne(mongoData);

        console.log(`New document created with _id: ${result.insertedId}`);
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
