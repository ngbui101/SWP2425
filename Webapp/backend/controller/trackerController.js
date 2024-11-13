const Tracker = require('../models/Tracker')
const Geofence = require('../models/Geofence')
const TrackerHistory = require('../models/Trackerhistory')
const User = require('../models/User');
const device = require('../models/mqttDevice'); // Gerät aus der neuen Datei importieren
const AllTracker = require('../models/AllTrackers');
const Mode = require('../models/Mode');
/**
 * Sends data to a specified tracker via MQTT.
 * 
 * @param {String} userId - The ID of the user sending the data.
 * @param {String} trackerId - The ID (IMEI) of the tracker to send data to.
 * @param {Object} payload - The data to be sent, containing the messages or any other information.
 * 
 * @returns {void}
 */
const sendData = (userId, trackerId, payload) => {
  if (!device) {
    console.error('MQTT device is not defined');
    return;
  }

  const telemetryData = {
    userId: userId,
    timestamp: new Date().toISOString(),
    payload,
  };

  const topic = `tracker/${trackerId}/sub`;

  console.log(`Sending data to AWS IoT Core on topic: ${topic}`, telemetryData);

  if (typeof device.publish !== 'function') {
    console.error('Publish function is not available on the device object');
    return;
  }

  return device.publish(topic, JSON.stringify(telemetryData));
};

/**
 * Controller method that validates the user and tracker ownership,
 * then sends multiple messages (message1, message2, message3) to the specified tracker.
 * 
 * @param {Object} req - Express request object, containing the user and tracker details.
 * @param {Object} res - Express response object, used to send the result back to the client.
 * 
 * @returns {Promise<void>}
 */
async function publishMessageToTracker(req, res) {
  const { imei } = req.params;
  const { message1, message2, message3 } = req.body;
  const userId = req.user.id;

  try {
    const tracker = await Tracker.findOne({ imei }).exec();
    if (!tracker) {
      return res.status(404).json({ message: 'Tracker not found' });
    }

    const user = await User.findById(userId).populate('tracker').exec();
    if (!user) {
      return res.status(404).json({ message: 'User not found' });
    }

    const userOwnsTracker = user.tracker.some(t => t._id.equals(tracker._id));

    if (!userOwnsTracker) {
      return res.status(403).json({ message: 'You do not have permission to send messages to this tracker' });
    }

    const payload = { message1, message2, message3 };
    sendData(userId, imei, payload);

    res.status(200).json({ Response: `Data sent to tracker ${imei}`, payload });
  } catch (error) {
    console.error('Error sending data to tracker:', error);
    res.status(500).json({ Error: 'Failed to send data', error });
  }
}




// Get all trackers
async function getAllTrackers(req, res) {
  try {
    const trackers = await Tracker.find({});
    res.status(200).json(trackers);
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

// Get all trackers for the authenticated user
async function getAllUserTrackers(req, res) {
  const userId = req.user.id; // Get the authenticated user's ID from the request

  try {
    // Find the user by ID and populate their trackers
    const user = await User.findById(userId).populate('tracker').exec();

    if (!user) {
      return res.status(404).json({ message: 'User not found' });
    }

    // Return the trackers linked to the user
    res.status(200).json(user.tracker);
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}


// Get a tracker by ID
async function getTrackerById(req, res) {
  const { id } = req.params;

  try {
    const tracker = await Tracker.findById(id);
    if (!tracker) {
      return res.status(404).json({ message: 'Tracker not found' });
    }
    res.status(200).json(tracker);
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

async function createTracker(req, res) {
  const { name, imei, pin } = req.body;  // Accept name, imei, and pin parameters
  const userId = req.user.id;  // Assuming req.user contains authenticated user info

  try {
    // Check if the provided IMEI and PIN exist in the AllTrackers collection
    const allTracker = await AllTracker.findOne({ imei, pin }).exec();
    if (!allTracker) {
      return res.status(404).json({ message: 'Invalid IMEI or PIN. Tracker not found.' });
    }

    if (allTracker.isRegistered) {
      return res.status(409).json({ message: 'This tracker has already been registered.' });
    }

    // Check if a tracker with the same IMEI already exists in the Trackers collection
    const existingTracker = await Tracker.findOne({ imei }).exec();
    if (existingTracker) {
      return res.status(409).json({ message: 'Tracker with this IMEI already exists.' });
    }

    // Create a new tracker object without saving it yet
    const tracker = new Tracker({
      imei,
      name,
    });

    // Create a default Mode document for this tracker
    const mode = new Mode({
      tracker: tracker._id, // Link the Mode document to this tracker
      GnssMode: false,
      CellInfosMode: true,
      BatteryMode: true,
      TemperatureMode: true,
      NmeaMode: false,
      frequenz: 3600000 // Default frequency in milliseconds
    });
    await mode.save();

    // Assign the mode ID to the tracker
    tracker.mode = mode._id;

    // Save the tracker with the mode linked
    await tracker.save();

    // Update the allTrackers collection to set isRegistered to true
    allTracker.isRegistered = true;
    await allTracker.save();

    // Create a geofence linked to this tracker
    const geofence = new Geofence({
      tracker: tracker._id,
      radius: 0,
      longitude: 0,
      latitude: 0,
    });
    await geofence.save();
    tracker.geofence = geofence._id;
    await tracker.save();

    // Create a TrackerHistory document for the tracker
    const trackerHistory = new TrackerHistory({
      tracker: tracker._id,
      measurements: [], // Start with an empty measurements array
    });
    await trackerHistory.save();
    tracker.trackerhistory = trackerHistory._id;
    await tracker.save();

    // Find the user and add the tracker to their tracker array
    const user = await User.findById(userId);
    if (!user) {
      return res.status(404).json({ message: 'User not found.' });
    }
    user.tracker.push(tracker._id);
    await user.save();

    res.status(201).json({ message: 'Tracker, Mode, and Geofence created successfully', tracker });
  } catch (error) {
    console.error('Error creating tracker:', error);
    res.status(500).json({ message: 'Server error', error });
  }
}


// Update a tracker by ID
async function updateTrackerName(req, res) {
  const { id } = req.params;
  const { name } = req.body;

  try {
    const tracker = await Tracker.findById(id);
    if (!tracker) {
      return res.status(404).json({ message: 'Tracker not found' });
    }
    tracker.name = name || tracker.name;
    await tracker.save();
    res.status(200).json({ message: 'Tracker updated successfully', tracker });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

// Delete a tracker by ID
async function deleteTracker(req, res) {
  const { id } = req.params;
  const userId = req.user.id;

  try {
    const tracker = await Tracker.findByIdAndDelete(id);
    if (!tracker) {
      return res.status(404).json({ message: 'Tracker not found' });
    }

    await User.findByIdAndUpdate(userId, {
      $pull: { tracker: tracker._id }
    });

    res.status(200).json({ message: 'Tracker deleted successfully' });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

async function setTrackerMode(req, res) {
  const { id } = req.params;
  const { mode } = req.body;

  // Ensure the mode is one of the allowed values
  if (!['RT', 'LT', 'NB', 'None'].includes(mode)) {
    return res.status(400).json({ message: 'Invalid mode. Mode should be one of "RT", "LT", or "Test".' });
  }

  try {
    const tracker = await Tracker.findById(id);
    if (!tracker) {
      return res.status(404).json({ message: 'Tracker not found' });
    }

    tracker.mode = mode;
    await tracker.save();
    res.status(200).json({ message: `Tracker mode set to ${mode}`, tracker });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

async function getTrackerGeofence(req, res) {
  const { id } = req.params;  // Tracker ID from the request parameters

  try {
    // Find the geofence linked to this tracker
    const geofence = await Geofence.findOne({ tracker: id }).exec();

    if (!geofence) {
      return res.status(404).json({ message: 'Geofence not found for this tracker' });
    }

    res.status(200).json(geofence);
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

async function getTrackerHistory(req, res) {
  const { id } = req.params;  // Tracker ID from the request parameters

  try {
    // Find the tracker history linked to this tracker
    const trackerHistory = await TrackerHistory.findOne({ tracker: id });


    if (!trackerHistory) {
      return res.status(404).json({ message: 'Tracker history not found for this tracker' });
    }

    res.status(200).json(trackerHistory);
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

module.exports = {
  getAllTrackers,
  getTrackerById,
  createTracker,
  updateTrackerName,
  deleteTracker,
  setTrackerMode,
  getTrackerGeofence,
  getTrackerHistory,
  getAllUserTrackers,
  publishMessageToTracker
};
