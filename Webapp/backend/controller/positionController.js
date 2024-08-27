const Position = require('backend/models/Position.js');

// Get all positions
async function getAllPositions(req, res) {
  try {
    const positions = await Position.find({});
    res.status(200).json(positions);
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

// Get a position by ID
async function getPositionById(req, res) {
  const { id } = req.params;

  try {
    const position = await Position.findById(id);
    if (!position) {
      return res.status(404).json({ message: 'Position not found' });
    }
    res.status(200).json(position);
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

// Create a new position
async function createPosition(req, res) {
  const { id, name, latitude, longitude, altitude, mode } = req.body;

  try {
    const position = new Position({
      id,
      name,
      latitude,
      longitude,
      altitude,
      mode,
    });

    await position.save();
    res.status(201).json({ message: 'Position created successfully', position });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

//  No Update Method since an updated Position is a new Position


// Delete a position by ID
async function deletePosition(req, res) {
  const { id } = req.params;

  try {
    const position = await Position.findByIdAndDelete(id);
    if (!position) {
      return res.status(404).json({ message: 'Position not found' });
    }

    res.status(200).json({ message: 'Position deleted successfully' });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

// Set position mode by ID
async function setPositionMode(req, res) {
  const { id } = req.params;
  const { mode } = req.body;

  // Ensure the mode is one of the allowed values
  if (!['GPS', 'LTE', 'IOT'].includes(mode)) {
    return res.status(400).json({ message: 'Invalid mode. Mode should be one of "GPS", "LTE", or "IOT".' });
  }

  try {
    const position = await Position.findById(id);
    if (!position) {
      return res.status(404).json({ message: 'Position not found' });
    }

    position.mode = mode;
    await position.save();
    res.status(200).json({ message: `Position mode set to ${mode}`, position });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

module.exports = {
  getAllPositions,
  getPositionById,
  createPosition,
  deletePosition,
  setPositionMode,
};
