const Geofence = require('../models/Geofence');
const Tracker = require('../models/Tracker');



// Get a geofence by ID
async function getGeofenceById(req, res) {
  const { id } = req.params;

  try {
    const geofence = await Geofence.findById(id);
    if (!geofence) {
      return res.status(404).json({ message: 'Geofence not found' });
    }
    res.status(200).json(geofence);
  } catch (error) {a
    res.status(500).json({ message: 'Server error', error });
  }
}

// Create a new geofence
async function createGeofence(req, res) {
  const { trackerId, radius, longitude, latitude } = req.body;  // Geofence attributes

  try {
    // Check if tracker exists
    const tracker = await Tracker.findById(trackerId);
    if (!tracker) {
      return res.status(404).json({ message: 'Tracker not found' });
    }

    // Create the geofence linked to the tracker
    const geofence = new Geofence({
      tracker: tracker._id,
      radius,
      longitude,
      latitude
    });

    // Save the geofence
    await geofence.save();

    res.status(201).json({ message: 'Geofence created successfully', geofence });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

// Update a geofence by ID
async function updateGeofence(req, res) {
  const { id } = req.params;
  const { radius, longitude, latitude } = req.body;  // Geofence attributes

  try {
    const geofence = await Geofence.findById(id);
    if (!geofence) {
      return res.status(404).json({ message: 'Geofence not found' });
    }

    // Update the geofence details
    geofence.radius = radius || geofence.radius;
    geofence.longitude = longitude || geofence.longitude;
    geofence.latitude = latitude || geofence.latitude;

    // Save the updated geofence
    await geofence.save();

    res.status(200).json({ message: 'Geofence updated successfully', geofence });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

// Delete a geofence by ID
async function deleteGeofence(req, res) {
  const { id } = req.params;

  try {
    const geofence = await Geofence.findByIdAndDelete(id);
    if (!geofence) {
      return res.status(404).json({ message: 'Geofence not found' });
    }

    res.status(200).json({ message: 'Geofence deleted successfully' });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

module.exports = {
  getGeofenceById,
  createGeofence,
  updateGeofence,
  deleteGeofence
};
