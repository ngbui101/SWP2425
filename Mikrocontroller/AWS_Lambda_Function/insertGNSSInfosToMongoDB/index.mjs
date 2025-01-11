import { MongoClient} from 'mongodb';

export const handler = async (event) => {
    const { IMEI, Timestamp, gnss } = typeof event.body === 'string' ? JSON.parse(event.body) : event;
    const mongoURI = process.env.MONGO_URI;
    const client = new MongoClient(mongoURI);

    try {
        await client.connect();
        const database = client.db('SOP');
        const collection = database.collection('measurements');

        if (gnss.latitude !== undefined) {
            await collection.insertOne({
                imei: IMEI,
                mode: "GPS",
                latitude: gnss.latitude,
                longitude: gnss.longitude,
                hdop: gnss.hdop,
                nsat: gnss.nsat,
                accuracy: gnss.accuracy,
                timeToGetFirstFix: gnss.TTFF,
                createdAt: Timestamp,
                updatedAt: Timestamp
            });
        }

        return {
            statusCode: 200,
            body: JSON.stringify({ message: 'GNSS information inserted successfully' })
        };
    } catch (error) {
        console.error("Error inserting GNSS info: ", error);
        return {
            statusCode: 500,
            body: JSON.stringify('Error inserting GNSS information')
        };
    } finally {
        await client.close();
    }
};