const Measurement = require('../models/Measurement');
const Tracker = require('../models/Tracker');
const TrackerHistory = require('../models/Trackerhistory');

const { updateTrackerHistory } = require('./trackerhistoryController'); // Import the updateTrackerHistory function



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

// Get measurements by Tracker ID
async function getMeasurementsByTrackerId(req, res) {
  const { trackerId } = req.params; // Extract trackerId from the route params

  try {
    // Find measurements by trackerId
    const measurements = await Measurement.find({ tracker: trackerId });

    if (measurements.length === 0) {
      return res.status(404).json({ message: 'No measurements found for this tracker' });
    }

    res.status(200).json(measurements);
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}



// No Update Method since an updated Position is a new Position
// Delete a measurement by ID
async function deleteMeasurement(req, res) {
  const { id } = req.params;

  try {
    // Find and delete the measurement by ID
    const measurement = await Measurement.findByIdAndDelete(id);
    if (!measurement) {
      return res.status(404).json({ message: 'Measurement not found' });
    }

    res.status(200).json({ message: 'Measurement deleted successfully' });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}



// Create a new measurement
async function createMeasurement(req, res) {
  const {
    imei,
    mode,
    latitude,
    longitude,
    temperature,
    cellinfo,
    humidity,
    battery,
    step, // Include step
    speed  // Include speed
  } = req.body;

  try {
    // Verify if the tracker exists by imei
    const tracker = await Tracker.findOne({ imei });
    if (!tracker) {
      return res.status(404).json({ message: 'Tracker not found with the given IMEI' });
    }

    // Find the TrackerHistory for the given tracker
    const trackerHistory = await TrackerHistory.findOne({ tracker: tracker._id });
    if (!trackerHistory) {
      return res.status(404).json({ message: 'Tracker history not found for this tracker' });
    }

    // Create a new Measurement
    const measurement = new Measurement({
      imei,
      mode,
      latitude,
      longitude,
      temperature,
      cellinfo,
      humidity,
      battery,
      step,   // Save step in the measurement
      speed,  // Save speed in the measurement
      tracker: tracker._id // Reference to the Tracker using its ID
    });

    await measurement.save();

    // Push the measurement attributes directly into TrackerHistory
    trackerHistory.measurements.push({
      imei,
      mode,
      latitude,
      longitude,
      temperature,
      cellinfo,
      humidity,
      battery,
      step,  // Add step to trackerHistory
      speed  // Add speed to trackerHistory
    });
    await trackerHistory.save(); // Save the updated tracker history

    res.status(201).json({ message: 'Measurement created successfully', measurement });
  } catch (error) {
    console.error("Error in createMeasurement: ", error); // Log the error
    res.status(500).json({ message: 'Server error', error });
  }
}


module.exports = {
  getAllMeasurements,
  getMeasurementById,
  deleteMeasurement,
  getMeasurementsByTrackerId,
  createMeasurement // Ensure this method is included in exports
};
