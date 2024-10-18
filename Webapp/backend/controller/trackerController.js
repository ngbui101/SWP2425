const Tracker = require('../models/Tracker')
const Geofence = require('../models/Geofence')
const TrackerHistory = require('../models/Trackerhistory')
const User = require('../models/User');

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
      return   res.status(404).json({ message: 'Tracker not found' });
    }
    res.status(200).json(tracker);
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

// Create a new tracker
async function createTracker(req, res) {
  const { name, imei} = req.body;  // Accept name, imei, and geofence parameters
  const userId = req.user.id;  // Assuming req.user contains authenticated user info

  try {
    // Check if a tracker with the same imei already exists
    const existingTracker = await Tracker.findOne({ imei }).exec();
    if (existingTracker) {
      return res.status(409).json({ message: 'Tracker with this imei already exists' });
    }

  
    // Create a new tracker with mode set to 'LT' (default mode) and link tracker history
    const tracker = new Tracker({
      imei,
      name,
      mode: 'LT',  // Mode is set to 'LT' as requested  // Link to the newly created Trackerhistory
    });

    // Save the tracker first
    await tracker.save();
    // Now create a geofence linked to this tracker
    const geofence = new Geofence({
      tracker: tracker._id,
      radius:0,
      longitude:0,
      latitude:0
    });
    await geofence.save();
    tracker.geofence = geofence._id;
    await tracker.save();


    // Create new Trackerhistory for the tracker
    const trackerhistory = new TrackerHistory({
      tracker: tracker._id,
      measurements: [], // Beginne mit der neuen Position
    })
    await trackerhistory.save();
    tracker.trackerhistory = trackerhistory._id;
    await tracker.save();

    // Find the user and add the tracker to their tracker array
    const user = await User.findById(userId);
    if (!user) {
      return res.status(404).json({ message: 'User not found' });
    }

    user.tracker.push(tracker._id);  // Add the tracker ID to the user's trackers
    await user.save();

    res.status(201).json({ message: 'Tracker and Geofence created successfully', tracker });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

// Update a tracker by ID
async function updateTrackerName(req, res) {
  const { id } = req.params;
  const { name} = req.body;

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
  getAllUserTrackers
};
