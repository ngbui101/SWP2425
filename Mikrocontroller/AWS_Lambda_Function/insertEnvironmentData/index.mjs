import { MongoClient } from 'mongodb';

export const handler = async (event) => {
    const { IMEI, Timestamp, Temperature, Humidity, BatteryPercentage } = typeof event.body === 'string' ? JSON.parse(event.body) : event;
    const mongoURI = process.env.MONGO_URI;
    const client = new MongoClient(mongoURI);
    if(Temperature < 0 || Humidity < 0 || BatteryPercentage < 0){
        return {
            statusCode: 400,
            body: JSON.stringify('Invalid data: Temperature, Humidity and BatteryPercentage must be positive values')
        };
    }
    if(Temperature === undefined && Humidity === undefined && BatteryPercentage === undefined){
        return {
            statusCode: 400,
            body: JSON.stringify('Missing data: Temperature, Humidity or BatteryPercentage')
        };
    }
    
    try {
        await client.connect();
        const database = client.db('SOP');
        const collection = database.collection('measurements');

        if(Temperature !== undefined || Humidity !== undefined){
            await collection.insertOne({
                imei: IMEI,
                mode: "TemperatureHumidity",
                temperature: Temperature,
                humidity: Humidity,
                createdAt: Timestamp,
                updatedAt: Timestamp
            });
        }

        if(BatteryPercentage !== undefined){
            await collection.insertOne({
                imei: IMEI,
                mode: "Battery",
                battery: BatteryPercentage,
                createdAt: Timestamp,
                updatedAt: Timestamp
            });
        }
        return {
            statusCode: 200,
            body: JSON.stringify({ message: 'Environment data inserted successfully' })
        };
    } catch (error) {
        console.error("Error inserting environment data: ", error);
        return {
            statusCode: 500,
            body: JSON.stringify('Error inserting environment data')
        };
    } finally {
        await client.close();
    }
};