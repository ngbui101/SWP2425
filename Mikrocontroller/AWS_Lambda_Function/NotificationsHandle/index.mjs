import { MongoClient, ObjectId } from 'mongodb';
import AWS from 'aws-sdk';

const sns = new AWS.SNS();
const mongoURI = process.env.MONGO_URI; // MongoDB-URI aus Umgebungsvariablen

export const handler = async (event) => {
    const client = new MongoClient(mongoURI);

    try {
        await client.connect();
        const database = client.db('SOP');
        const trackersCollection = database.collection('trackers');
        const usersCollection = database.collection('users');
        const settingsCollection = database.collection('settings');

        const { IMEI, BatteryLow } = event;
        console.log(`IMEI: ${IMEI}, BatteryLow: ${BatteryLow}`);

        if (!BatteryLow) {
            console.log("BatteryLow is not set to true; no notification will be sent.");
            return { statusCode: 200, body: "No notification required." };
        }

        // Finde den Tracker mit der IMEI
        const trackerDoc = await trackersCollection.findOne({ imei: IMEI });
        if (!trackerDoc) {
            console.error(`Tracker with IMEI ${IMEI} not found.`);
            return { statusCode: 404, body: "Tracker not found" };
        }

        const trackerName = trackerDoc.name;
        const trackerId = trackerDoc._id;

        // Finde den Benutzer mit dem Tracker
        const userDoc = await usersCollection.findOne({ tracker: new ObjectId(trackerId) });
        if (!userDoc) {
            console.error(`User for tracker ID ${trackerId} not found.`);
            return { statusCode: 404, body: "User not found" };
        }

        const userPhoneNumber = userDoc.number;
        const userEmail = userDoc.email;

        // Hole die Benachrichtigungseinstellungen des Benutzers
        const settingsDoc = await settingsCollection.findOne({ _id: userDoc.settings });
        if (!settingsDoc) {
            console.error(`Settings for user with tracker ID ${trackerId} not found.`);
            return { statusCode: 404, body: "Settings not found" };
        }

        // Überprüfe, ob die Benachrichtigung aktiviert ist
        const smsEnabled = settingsDoc.notifications?.sms?.batteryLow === true;
        const emailEnabled = settingsDoc.notifications?.email?.batteryLow === true;

        // Nachricht für die Benachrichtigung
        const message = `Achtung: Die Batterie des Trackers '${trackerName}' ist unter 10%. Bitte laden Sie den Tracker bald auf.`;

        // Sende SMS-Benachrichtigung, wenn aktiviert und eine Telefonnummer vorhanden ist
        if (smsEnabled && userPhoneNumber) {
            const smsParams = {
                Message: message,
                PhoneNumber: userPhoneNumber
            };

            try {
                await sns.publish(smsParams).promise();
                console.log(`SMS sent to ${userPhoneNumber}`);
            } catch (error) {
                console.error(`Error sending SMS to ${userPhoneNumber}:`, error);
            }
        }

        // Sende E-Mail-Benachrichtigung, wenn aktiviert und eine E-Mail-Adresse vorhanden ist
        if (emailEnabled && userEmail) {
            const emailParams = {
                Message: message,
                Subject: "Battery Low Alert for Tracker",
                TopicArn: process.env.SNS_TOPIC_ARN, // Allgemeines SNS-Topic für E-Mails
                MessageAttributes: {
                    "userEmail": {
                        DataType: "String",
                        StringValue: userEmail // Dieses Attribut kann für Filter-Policies verwendet werden
                    }
                }
            };

            try {
                await sns.publish(emailParams).promise();
                console.log(`Email sent to ${userEmail}`);
            } catch (error) {
                console.error(`Error sending email to ${userEmail}:`, error);
            }
        }

        return { statusCode: 200, body: "BatteryLow notification sent to specific user." };

    } catch (error) {
        console.error("Error sending notification:", error);
        return { statusCode: 500, body: "Error sending notification." };
    } finally {
        await client.close();
    }
};
