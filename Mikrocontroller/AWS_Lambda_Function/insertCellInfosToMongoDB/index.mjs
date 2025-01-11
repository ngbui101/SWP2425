import { MongoClient, ObjectId } from 'mongodb';
import fetch from 'node-fetch';

async function getCellLocationAndEstimatedPosition(cells) {
  const apiUrl = process.env.API_URL;
  // Fünf mögliche Token-Umgebungsvariablen sammeln
  const tokens = [
    process.env.TOKEN_1,
    process.env.TOKEN_2,
    process.env.TOKEN_3,
    process.env.TOKEN_4,
    process.env.TOKEN_5
  ];

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

  let finalResult = null;   // Hier speichern wir das finale Ergebnis (falls erfolgreich).
  let attempts = 0;         // Zählt die Versuche hoch.

  // Bis zu 5 Wiederholungen versuchen
  while (attempts < 5 && finalResult === null) {
    attempts++;

    // Zufallsindex wählen und Token zufällig auswählen
    const randomIndex = Math.floor(Math.random() * tokens.length);
    const selectedToken = tokens[randomIndex];

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

      // Prüfe, ob die API Erfolg meldet
      if (responseData.status === "ok") {
        console.log("Location data from Unwired Labs API:", responseData);

        finalResult = {
          latitude: responseData.lat,
          longitude: responseData.lon,
          accuracy: responseData.accuracy / 5,
          ncells: ncells,
          avg_signal: avgSignal
        };
      }
      else if (responseData.status === "error" && responseData.balance === 0) {
        console.error(`Unwired Labs API returned an error (zero balance) on attempt ${attempts}. Trying next token...`);
      }
      else {
        console.error("Unwired Labs API returned an error:", responseData);
        return null;
      }

    } catch (error) {
      console.error(`Error calling Unwired Labs API on attempt ${attempts}:`, error);
    }
  }

  // Wenn nach 5 Versuchen kein gültiges Ergebnis kam
  if (!finalResult) {
    console.error(`Failed to get a valid response after ${attempts} attempts.`);
    return null;
  }

  return finalResult;
}




export const handler = async (event) => {
  const { IMEI, Timestamp, trackerId, Cells } = typeof event.body === 'string' ? JSON.parse(event.body) : event;
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
        tracker:new ObjectId(trackerId),
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
