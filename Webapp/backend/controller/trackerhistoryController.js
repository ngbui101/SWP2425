const TrackerHistory = require('../models/Trackerhistory');
const Tracker = require('../models/Tracker');

// Get all tracker histories
async function getAllTrackerHistories(req, res) {
  try {
    const trackerHistories = await TrackerHistory.find({}).populate('tracker');
    res.status(200).json(trackerHistories);
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

// Get a tracker history by ID
async function getTrackerHistoryById(req, res) {
  const { id } = req.params;

  try {
    const trackerHistory = await TrackerHistory.findById(id);
    if (!trackerHistory) {
      return res.status(404).json({ message: 'Tracker history not found' });
    }
    res.status(200).json(trackerHistory);
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

// Create a new tracker history
async function createTrackerHistory(req, res) {
  const { trackerId, measurements } = req.body;

  try {
    // Check if the tracker exists
    const tracker = await Tracker.findById(trackerId);
    if (!tracker) {
      return res.status(404).json({ message: 'Tracker not found' });
    }

    const trackerHistory = new TrackerHistory({
      tracker: trackerId,
      measurements, // Measurements should be an array of objects as per the new schema
    });

    await trackerHistory.save();
    res.status(201).json({ message: 'Tracker history created successfully', trackerHistory });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

// Update a tracker history by ID (e.g., add new measurements)
async function updateTrackerHistory(req, res) {
  const { id } = req.params;
  const { measurements } = req.body;

  try {
    const trackerHistory = await TrackerHistory.findById(id);
    if (!trackerHistory) {
      return res.status(404).json({ message: 'Tracker history not found' });
    }

    // Add new measurements to the existing tracker history
    trackerHistory.measurements.push(...measurements); // Push measurement objects directly
    await trackerHistory.save();

    res.status(200).json({ message: 'Tracker history updated successfully', trackerHistory });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

// Delete a tracker history by ID
async function deleteTrackerHistory(req, res) {
  const { id } = req.params;

  try {
    const trackerHistory = await TrackerHistory.findByIdAndDelete(id);
    if (!trackerHistory) {
      return res.status(404).json({ message: 'Tracker history not found' });
    }

    res.status(200).json({ message: 'Tracker history deleted successfully' });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

// Get tracker history by tracker ID
async function getTrackerHistoriesByTrackerId(req, res) {
  const { trackerId } = req.params;

  try {
    const trackerHistories = await TrackerHistory.find({ tracker: trackerId });

    if (trackerHistories.length === 0) {
      return res.status(404).json({ message: 'No tracker histories found for this tracker' });
    }

    res.status(200).json(trackerHistories);
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

module.exports = {
  getAllTrackerHistories,
  getTrackerHistoryById,
  createTrackerHistory,
  updateTrackerHistory,
  deleteTrackerHistory,
  getTrackerHistoriesByTrackerId,
};
