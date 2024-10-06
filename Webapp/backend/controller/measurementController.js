const Measurement = require('../models/Measurement');
const Tracker = require('../models/Tracker');
const TrackerHistory = require('../models/Trackerhistory')


// Get all measurements
async function getAllMeasurements(req, res) {
  try {
    const measurements = await Measurement.find({});
    res.status(200).json(measurements);
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

// Get a measurement by ID
async function getMeasurementById(req, res) {
  const { id } = req.params;

  try {
    const measurement = await Measurement.findById(id);
    if (!measurement) {
      return res.status(404).json({ message: 'Measurement not found' });
    }
    res.status(200).json(measurement);
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

async function measurePosition(req, res) {
  const { latitude, longitude, mode, trackerId } = req.body;

  try {
    // Überprüfen, ob der Tracker existiert
    console.log(trackerId)
    const tracker = await Tracker.findById(trackerId);
    if (!tracker) {
      return res.status(404).json({ message: 'Tracker not found' });
    }

    // Measurement erstellen und dem Tracker zuweisen
    const measurement = new Measurement({
      type: 'Position',
      latitude,
      longitude,
      mode: mode,
      tracker: trackerId // Verweis auf den Tracker
    });

    await measurement.save();
    
    // Hier die Tracker-Historie aktualisieren oder erstellen
    await updateOrCreateTrackerHistory(trackerId, { latitude, longitude });

    res.status(201).json({ message: 'Measurement created successfully and added to tracker', measurement });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

// Funktion zum Erstellen oder Aktualisieren der Tracker-Historie
async function updateOrCreateTrackerHistory(trackerId, position) {
  try {
    // Überprüfen, ob bereits eine Historie für den Tracker existiert
    let trackerHistory = await TrackerHistory.findOne({ tracker: trackerId });

    if (trackerHistory) {
      // Historie existiert, also aktualisieren wir sie
      trackerHistory.positions.push(position); // Neuen Positionseintrag hinzufügen
      await trackerHistory.save();
    } else {
      // Historie existiert nicht, also erstellen wir eine neue
      trackerHistory = new TrackerHistory({
        tracker: trackerId,
        positions: [position], // Beginne mit der neuen Position
      });
      await trackerHistory.save();
    }
  } catch (error) {
    console.error('Error updating or creating tracker history:', error);
  }
}

async function measureTemperature(req, res) {
  const { temperature, trackerId } = req.body;

  try {
    // Überprüfen, ob der Tracker existiert
    const tracker = await Tracker.findById(trackerId);
    if (!tracker) {
      return res.status(404).json({ message: 'Tracker not found' });
    }

    // Measurement erstellen und dem Tracker zuweisen
    const measurement = new Measurement({
      type: 'Temperature',
      temperature,
      tracker: trackerId // Verweis auf den Tracker
    });

    await measurement.save();
    res.status(201).json({ message: 'Measurement created successfully and added to tracker', measurement });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

async function measureHumidity(req, res) {
  const { humidity, trackerId } = req.body;

  try {
    // Überprüfen, ob der Tracker existiert
    const tracker = await Tracker.findById(trackerId);
    if (!tracker) {
      return res.status(404).json({ message: 'Tracker not found' });
    }

    // Measurement erstellen und dem Tracker zuweisen
    const measurement = new Measurement({
      type: 'Humidity',
      humidity,
      tracker: trackerId // Verweis auf den Tracker
    });

    await measurement.save();
    res.status(201).json({ message: 'Measurement created successfully and added to tracker', measurement });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}


// Set measurement mode by ID
async function setPositionMode(req, res) {
  const { id } = req.params;
  const { mode } = req.body;

  // Ensure the mode is one of the allowed values
  if (!['GPS', 'LTE', 'IOT'].includes(mode)) {
    return res.status(400).json({ message: 'Invalid mode. Mode should be one of "GPS", "LTE", or "IOT".' });
  }

  try {
    const measurement = await Measurement.findById(id);
    if (!measurement) {
      return res.status(404).json({ message: 'Measurement not found' });
    }

    measurement.mode = mode;
    await measurement.save();
    res.status(200).json({ message: `Measurement mode set to ${mode}`, measurement });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

//  No Update Method since an updated Position is a new Position

// Delete a measurement by ID
async function deleteMeasurement(req, res) {
  const { id } = req.params;

  try {
    // Find and delete the measurement by ID
    const measurement = await Measurement.findByIdAndDelete(id);
    if (!measurement) {
      return res.status(404).json({ message: 'Measurement not found' });
    }

    // Remove the measurement from the associated tracker's measurements array
    await Tracker.findByIdAndUpdate(measurement.tracker, {
      $pull: { measurements: id } // Correct usage of $pull to remove the ID
    });

    res.status(200).json({ message: 'Measurement deleted successfully' });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

async function getMeasurementsByTrackerId(req, res) {
  const { trackerId } = req.params; // Extract trackerId from the route params
  const { type } = req.query; // Optional query parameter to filter by type

  try {
    // Find measurements by trackerId, optionally filtering by type if provided
    const query = { tracker: trackerId };

    // If the type is provided, add it to the query
    if (type && ['Position', 'Temperature', 'Humidity', 'None'].includes(type)) {
      query.type = type;
    }

    // Fetch the measurements from the database
    const measurements = await Measurement.find(query);

    if (measurements.length === 0) {
      return res.status(404).json({ message: 'No measurements found for this tracker' });
    }

    res.status(200).json(measurements);
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}



module.exports = {
  getAllMeasurements,
  getMeasurementById,
  measurePosition,
  measureTemperature,
  measureHumidity,
  deleteMeasurement,
  setPositionMode,
  getMeasurementsByTrackerId // Export the new method

};
