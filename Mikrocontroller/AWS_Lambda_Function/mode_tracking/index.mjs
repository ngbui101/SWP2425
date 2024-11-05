import { MongoClient, ObjectId } from "mongodb";
import AWS from "aws-sdk";

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

export const handler = async (event) => {
    const mongoURI = process.env.MONGO_URI;
    const client = new MongoClient(mongoURI);

    try {
        await client.connect();
        const database = client.db('SOP');
        
        // Access the fullDocument in the event
        const { fullDocument } = event.detail;
        if (!fullDocument) {
            throw new Error("Missing fullDocument in the event payload.");
        }

        // Extract the IMEI from the tracker document
        const trackerId = fullDocument.tracker;
        const trackersCollection = database.collection('trackers');
        const trackerDoc = await trackersCollection.findOne({ _id: new ObjectId(trackerId) });

        if (!trackerDoc) {
            console.error(`Tracker not found for tracker ID: ${trackerId}`);
            return {
                statusCode: 404,
                body: JSON.stringify(`Tracker not found for tracker ID: ${trackerId}`)
            };
        }

        const imei = trackerDoc.imei;
        const modeData = {
            GnssMode: fullDocument.GnssMode,
            CellInfosMode: fullDocument.CellInfosMode,
            BatteryMode: fullDocument.BatteryMode,
            TemperatureMode: fullDocument.TemperatureMode,
            NmeaMode: fullDocument.NmeaMode,
            frequenz: fullDocument.frequenz || 5000
        };

        // Publish updated mode to AWS IoT for the device
        await publishToAwsIoT(imei, modeData);

        console.log("Mode update processed successfully.");
        return {
            statusCode: 200,
            body: JSON.stringify("Mode update processed successfully.")
        };

    } catch (error) {
        console.error("Error handling mode change:", error);
        return {
            statusCode: 500,
            body: JSON.stringify("Error handling mode change.")
        };
    } finally {
        await client.close();
    }
};
