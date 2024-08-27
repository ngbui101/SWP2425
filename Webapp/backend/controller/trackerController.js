const Tracker = require('backend/models/Tracker.js');

// Get all trackers
async function getAllTrackers(req, res) {
  try {
    const trackers = await Tracker.find({});
    res.status(200).json(trackers);
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

// Create a new tracker
async function createTracker(req, res) {
  const { id, name} = req.body;

  try {
    const tracker = new Tracker({
      id,
      name,
    });

    await tracker.save();
    res.status(201).json({ message: 'Tracker created successfully', tracker });
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

  try {
    const tracker = await Tracker.findByIdAndDelete(id);
    if (!tracker) {
      return res.status(404).json({ message: 'Tracker not found' });
    }

    res.status(200).json({ message: 'Tracker deleted successfully' });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

async function setTrackerMode(req, res) {
    const { id } = req.params;
    const { mode } = req.body;
  
    // Ensure the mode is one of the allowed values
    if (!['RT', 'LT', 'Test'].includes(mode)) {
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

module.exports = {
  getAllTrackers,
  getTrackerById,
  createTracker,
  updateTrackerName,
  deleteTracker,
  setTrackerMode,
};
