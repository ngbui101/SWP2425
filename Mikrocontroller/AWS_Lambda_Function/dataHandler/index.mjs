import fetch from 'node-fetch';

async function postToLambdaUrl(url, payload) {
    console.log(`Sende Daten an URL ${url} mit Payload:`, payload);
    try {
        const response = await fetch(url, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(payload)
        });

        if (!response.ok) {
            const errorText = await response.text();
            console.error(`Fehlerhafte Antwort von ${url}:`, errorText);
            return { success: false, error: errorText };
        }

        return { success: true };
    } catch (error) {
        console.error(`Fehler beim Senden an URL ${url}:`, error);
        return { success: false, error: error.message };
    }
}

export const handler = async (event) => {
    // Body parsen
    const requestData = typeof event.body === 'string'
        ? JSON.parse(event.body)
        : event;

    // URLs aus den ENV-Variablen (o.Ä.)
    const urls = {
        insertCellInfosUrl: process.env.INSERT_CELL_INFOS_URL,
        insertGNSSInfosUrl: process.env.INSERT_GNSS_INFOS_URL,
        insertEnvironmentDataUrl: process.env.INSERT_ENVIRONMENT_DATA_URL
    };

    const results = [];

    // Cell-Infos
    if (requestData.Cells) {
        const payload = {
            IMEI: requestData.IMEI,
            Timestamp: requestData.Timestamp,
            trackerId: requestData.trackerId,
            Cells: requestData.Cells
        };

        const result = await postToLambdaUrl(urls.insertCellInfosUrl, payload);
        results.push({ step: 'CellInfos', result });
    }

    // GNSS-Infos
    if (requestData.Gnss && requestData.Gnss.latitude !== undefined) {
        const payload = {
            IMEI: requestData.IMEI,
            Timestamp: requestData.Timestamp,
            trackerId: requestData.trackerId,
            gnss: requestData.Gnss
        };

        const result = await postToLambdaUrl(urls.insertGNSSInfosUrl, payload);
        results.push({ step: 'GnssInfos', result });
    }

    // Battery/Temperature/Humidity
    if (
        requestData.BatteryPercentage !== undefined ||
        requestData.Temperature !== undefined ||
        requestData.Humidity !== undefined
    ) {
        const payload = {
            IMEI: requestData.IMEI,
            Timestamp: requestData.Timestamp,
            trackerId: requestData.trackerId,
            BatteryPercentage: requestData.BatteryPercentage,
            Temperature: requestData.Temperature,
            Humidity: requestData.Humidity
        };

        const result = await postToLambdaUrl(urls.insertEnvironmentDataUrl, payload);
        results.push({ step: 'EnvironmentData', result });
    }

    // Sammelresultat zurückgeben
    return {
        statusCode: 200,
        body: JSON.stringify({
            message: 'DataHandler finished sending data',
            results
        })
    };
};
