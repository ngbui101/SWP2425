import { MongoClient } from 'mongodb';
import fetch from 'node-fetch';

export async function getCellLocationAndEstimatedPosition(cells) {
    const apiUrl = process.env.API_URL;
    // Fünf mögliche Token-Umgebungsvariablen sammeln
    const tokens = [
        process.env.TOKEN_1,
        process.env.TOKEN_2,
        process.env.TOKEN_3,
        process.env.TOKEN_4,
        process.env.TOKEN_5
    ];

    // Zufallsindex wählen und Token zufällig auswählen
    const randomIndex = Math.floor(Math.random() * tokens.length);
    const selectedToken = tokens[randomIndex];

    // Optionales Minimal-Check: Falls kein Array oder leer (obwohl laut Vorgabe nicht nötig)
    if (!Array.isArray(cells) || cells.length === 0) {
        console.error("No valid cell information provided.");
        return null;
    }

    // Anzahl Zellen (ncells)
    const ncells = cells.length;

    // Durchschnittlichen Signalwert berechnen (falls "signal" in den Cells vorhanden ist)
    const signals = cells
        .filter(cell => typeof cell.signal === 'number')
        .map(cell => cell.signal);
    const avgSignal = signals.length > 0
        ? signals.reduce((acc, val) => acc + val, 0) / signals.length
        : null;

    // Anfrage-Payload mit zufällig ausgewähltem Token
    const payload = {
        token: selectedToken,
        cells,
        address: 1
    };

    try {
        const response = await fetch(apiUrl, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(payload)
        });

        if (!response.ok) {
            throw new Error(`API error: ${response.statusText}`);
        }

        const responseData = await response.json();
        if (responseData.status === "ok") {
            console.log("Location data from Unwired Labs API:", responseData);

            return {
                latitude: responseData.lat,
                longitude: responseData.lon,
                accuracy: responseData.accuracy / 5,
                ncells: ncells,                // Anzahl verwendeter Cells
                avg_signal: avgSignal  // Durchschnittliches Signal
            };
        } else {
            console.error("Unwired Labs API returned an error:", responseData);
            return null;
        }
    } catch (error) {
        console.error("Error calling Unwired Labs API:", error);
        return null;
    }
}



export const handler = async (event) => {
    const { IMEI, Timestamp, Cells } = typeof event.body === 'string' ? JSON.parse(event.body) : event;
    const mongoURI = process.env.MONGO_URI;
    const client = new MongoClient(mongoURI);
    if (Cells === undefined)
        return {
            statusCode: 500,
            body: JSON.stringify('cell information missing')
        }
    try {
        await client.connect();
        const database = client.db('SOP');
        const collection = database.collection('measurements');
        const location = await getCellLocationAndEstimatedPosition(Cells);

        if (location) {

            // const modeMap = {
            //     "lte": "LTE",
            //     "gsm": "GSM",
            //     "nbiot": "NBIOT"
            // };
            // const resolvedMode = modeMap[radioType.toLowerCase()] || radioType.toUpperCase();

            await collection.insertOne({
                imei: IMEI,
                mode: "LTE",
                latitude: location.latitude,
                longitude: location.longitude,
                accuracy: location.accuracy,
                ncells: location.ncells,
                avg_signal: location.avg_signal,
                createdAt: Timestamp,
                updatedAt: Timestamp
            });
        }
    } catch (error) {
        console.error("Error inserting cell info: ", error);
        return {
            statusCode: 500,
            body: JSON.stringify('Error inserting cell information')
        };
    } finally {
        await client.close();
        return {
            statusCode: 200,
            body: JSON.stringify({ message: 'Cell information inserted successfully' })
        };
    }
};
