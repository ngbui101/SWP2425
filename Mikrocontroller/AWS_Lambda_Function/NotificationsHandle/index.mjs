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

        const { IMEI, BatteryLow, LeavingGeo } = event;
        console.log(`IMEI: ${IMEI}, BatteryLow: ${BatteryLow}, LeavingGeo: ${LeavingGeo}`);

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

        // Überprüfe, ob Benachrichtigungen aktiviert sind
        const smsEnabled = {
            batteryLow: settingsDoc.notifications?.sms?.batteryLow === true,
            geofence: settingsDoc.notifications?.sms?.geofence === true
        };
        const emailEnabled = {
            batteryLow: settingsDoc.notifications?.email?.batteryLow === true,
            geofence: settingsDoc.notifications?.email?.geofence === true
        };

        // Nachricht für die Batterie-Benachrichtigung
        const batteryMessage = `Achtung: Die Batterie des Trackers '${trackerName}' ist unter 10%. Bitte laden Sie den Tracker bald auf.`;

        // Nachricht für die Geofence-Benachrichtigung
        const geoMessage = `Warnung: Der Tracker '${trackerName}' hat den definierten Geofence-Bereich verlassen.`;

        // Sende SMS-Benachrichtigung für Batterie, wenn aktiviert und eine Telefonnummer vorhanden ist
        if (BatteryLow && smsEnabled.batteryLow && userPhoneNumber) {
            const smsParams = {
                Message: batteryMessage,
                PhoneNumber: userPhoneNumber
            };

            try {
                await sns.publish(smsParams).promise();
                console.log(`Battery SMS sent to ${userPhoneNumber}`);
            } catch (error) {
                console.error(`Error sending battery SMS to ${userPhoneNumber}:`, error);
            }
        }

        // Sende E-Mail-Benachrichtigung für Batterie, wenn aktiviert und eine E-Mail-Adresse vorhanden ist
        if (BatteryLow && emailEnabled.batteryLow && userEmail) {
            const emailParams = {
                Message: batteryMessage,
                Subject: "Battery Low Alert for Tracker",
                TopicArn: process.env.SNS_TOPIC_ARN, // Allgemeines SNS-Topic für E-Mails
                MessageAttributes: {
                    "userEmail": {
                        DataType: "String",
                        StringValue: userEmail // Attribut zur Filterung
                    }
                }
            };

            try {
                await sns.publish(emailParams).promise();
                console.log(`Battery Email sent to ${userEmail}`);
            } catch (error) {
                console.error(`Error sending battery email to ${userEmail}:`, error);
            }
        }

        // Sende SMS-Benachrichtigung für Geofence, wenn aktiviert und eine Telefonnummer vorhanden ist
        if (LeavingGeo && smsEnabled.geofence && userPhoneNumber) {
            const smsParams = {
                Message: geoMessage,
                PhoneNumber: userPhoneNumber
            };

            try {
                await sns.publish(smsParams).promise();
                console.log(`Geofence SMS sent to ${userPhoneNumber}`);
            } catch (error) {
                console.error(`Error sending geofence SMS to ${userPhoneNumber}:`, error);
            }
        }

        // Sende E-Mail-Benachrichtigung für Geofence, wenn aktiviert und eine E-Mail-Adresse vorhanden ist
        if (LeavingGeo && emailEnabled.geofence && userEmail) {
            const emailParams = {
                Message: geoMessage,
                Subject: "Geofence Alert for Tracker",
                TopicArn: process.env.SNS_TOPIC_ARN,
                MessageAttributes: {
                    "userEmail": {
                        DataType: "String",
                        StringValue: userEmail
                    }
                }
            };

            try {
                await sns.publish(emailParams).promise();
                console.log(`Geofence Email sent to ${userEmail}`);
            } catch (error) {
                console.error(`Error sending geofence email to ${userEmail}:`, error);
            }
        }

        return { statusCode: 200, body: "Notifications sent based on event conditions." };

    } catch (error) {
        console.error("Error sending notification:", error);
        return { statusCode: 500, body: "Error sending notification." };
    } finally {
        await client.close();
    }
};
