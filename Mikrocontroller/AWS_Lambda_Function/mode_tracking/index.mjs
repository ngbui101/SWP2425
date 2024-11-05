import { MongoClient, ObjectId } from "mongodb";
import AWS from "aws-sdk";

const iotData = new AWS.IotData({ endpoint: process.env.AWS_IOT_ENDPOINT });

async function publishToAwsIoT(imei, field, value) {
    const topic = `tracker/${imei}/sub`;
    const payload = JSON.stringify({ [field]: value });

    try {
        await iotData.publish({
            topic,
            qos: 1,
            payload
        }).promise();
        console.log(`Message published to topic ${topic} with ${field}: ${value}`);
    } catch (err) {
        console.error(`Error publishing ${field} to AWS IoT:`, err);
    }
}

export const handler = async (event) => {
    const mongoURI = process.env.MONGO_URI;
    const client = new MongoClient(mongoURI);

    try {
        await client.connect();
        const database = client.db('SOP');
        
        // Log the event for debugging
        console.log("Event received:", JSON.stringify(event, null, 2));
        
        // Access the update description to get changed fields
        const { updateDescription, fullDocument } = event.detail;

        if (!updateDescription || !updateDescription.updatedFields) {
            throw new Error("Missing updatedFields in the event payload.");
        }

        console.log("Update Description:", JSON.stringify(updateDescription, null, 2));

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

        // Publish each updated field individually
        for (const [field, value] of Object.entries(updateDescription.updatedFields)) {
            await publishToAwsIoT(imei, field, value);
        }

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
