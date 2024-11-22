const AllTracker = require('../models/AllTrackers');

// Create a new tracker entry in the all_trackers collection
async function createAllTracker(req, res) {
  const { imei, pin } = req.body;

  try {
    // Check if a tracker with the same IMEI already exists
    const existingTracker = await AllTracker.findOne({ imei }).exec();
    if (existingTracker) {
      return res.status(409).json({ message: 'Tracker with this IMEI already exists' });
    }

    // Create a new tracker entry
    const newTracker = new AllTracker({
      imei,
      pin,
      isRegistered: false, // Default value
    });

    await newTracker.save();
    res.status(201).json({ message: 'Tracker created successfully', tracker: newTracker });
  } catch (error) {
    console.error('Error creating tracker:', error);
    res.status(500).json({ message: 'Server error', error });
  }
}

// Update an existing tracker entry by IMEI
async function updateAllTracker(req, res) {
  const { imei } = req.params;
  const { pin, isRegistered } = req.body;

  try {
    const tracker = await AllTracker.findOne({ imei }).exec();
    if (!tracker) {
      return res.status(404).json({ message: 'Tracker not found' });
    }

    // Update the tracker fields if provided
    if (pin !== undefined) tracker.pin = pin;
    if (isRegistered !== undefined) tracker.isRegistered = isRegistered;

    await tracker.save();
    res.status(200).json({ message: 'Tracker updated successfully', tracker });
  } catch (error) {
    console.error('Error updating tracker:', error);
    res.status(500).json({ message: 'Server error', error });
  }
}

// Delete a tracker entry by IMEI
async function deleteAllTracker(req, res) {
  const { imei } = req.params;

  try {
    const tracker = await AllTracker.findOneAndDelete({ imei }).exec();
    if (!tracker) {
      return res.status(404).json({ message: 'Tracker not found' });
    }

    res.status(200).json({ message: 'Tracker deleted successfully' });
  } catch (error) {
    console.error('Error deleting tracker:', error);
    res.status(500).json({ message: 'Server error', error });
  }
}

// Get all trackers
async function getAllTrackers(req, res) {
  try {
    const trackers = await AllTracker.find({});
    res.status(200).json(trackers);
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

module.exports = {
  createAllTracker,
  updateAllTracker,
  deleteAllTracker,
  getAllTrackers
};
