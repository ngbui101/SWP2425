import { MongoClient, ObjectId } from "mongodb";
import AWS from "aws-sdk";

const iotData = new AWS.IotData({ endpoint: process.env.AWS_IOT_ENDPOINT });

async function publishToAwsIoT(imei, updatedFields) {
    const topic = `tracker/${imei}/sub`;
    const payload = JSON.stringify(updatedFields);

    try {
        await iotData.publish({
            topic,
            qos: 1,
            payload
        }).promise();
        console.log(`Message published to topic ${topic} with payload: ${payload}`);
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

        // Log the extracted IMEI for debugging
        console.log(`Extracted IMEI: ${imei}`);

        // Check if there are any fields to publish
        if (Object.keys(updateDescription.updatedFields).length === 0) {
            console.log("No fields to publish, skipping AWS IoT publish.");
            return {
                statusCode: 200,
                body: JSON.stringify("No fields to publish.")
            };
        }

        // Publish all updated fields in a single message
        await publishToAwsIoT(imei, updateDescription.updatedFields);

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
