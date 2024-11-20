const Mode = require('../models/Mode');
const Tracker = require('../models/Tracker');
const mongoose = require('mongoose');
// Update a mode configuration by tracker ID
const updateMode = async (req, res) => {
  const trackerId = req.params.trackerId;

  // Include BatteryMode and TemperatureMode in the update data
  const updateData = {
      GnssMode: req.body.GnssMode,
      CellInfosMode: req.body.CellInfosMode,
      frequenz: req.body.frequenz,
      BatteryMode: req.body.BatteryMode, // New field
      TemperatureMode: req.body.TemperatureMode // New field
  };

  try {
      // Update the mode configuration for the specified tracker
      const result = await Mode.updateOne(
          { tracker: trackerId },
          { $set: updateData }
      );

      // Check if a document was updated
      if (result.nModified === 0) {
          return res.status(404).json({ message: "Mode configuration not found or no changes made" });
      }

      // Fetch the updated document
      const modeConfig = await Mode.findOne({ tracker: trackerId });
      if (!modeConfig) {
          return res.status(404).json({ message: "Mode configuration not found for this tracker" });
      }

      res.status(200).json(modeConfig); // Return the updated mode configuration
  } catch (error) {
      console.error("Failed to update mode configuration:", error);
      res.status(500).json({ message: "Failed to update mode configuration", error });
  }
};

// Get a mode configuration by tracker ID
async function getMode(req, res) {
  const trackerId = req.params.trackerId; // Extract the tracker ID from the request parameters

  try {
    // Find the mode configuration associated with the specified tracker
    const mode = await Mode.findOne({ tracker: trackerId }).exec();
    if (!mode) {
      return res.status(404).json({ message: 'Mode configuration not found for this tracker' });
    }

    res.status(200).json(mode); // Return the mode configuration
  } catch (error) {
    console.error('Error fetching mode configuration:', error);
    res.status(500).json({ message: 'Server error', error });
  }
}

module.exports = {
  updateMode,
  getMode
};
