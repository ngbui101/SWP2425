const Mode = require('../models/Mode');
const Tracker = require('../models/Tracker');
const mongoose = require('mongoose');
// Update a mode configuration by tracker ID
const updateMode = async (req, res) => {
    const trackerId = req.params.trackerId;
    const updateData = {
        GnssMode: req.body.GnssMode,
        CellInfosMode: req.body.CellInfosMode,
        frequenz: req.body.frequenz
    };

    try {
        await Mode.updateOne(
            { tracker: trackerId },
            { $set: updateData }
        );

        // Fetch the updated document
        const modeConfig = await Mode.findOne({ tracker: trackerId });
        if (!modeConfig) {
            return res.status(404).json({ message: "Mode configuration not found for this tracker" });
        }

        res.status(200).json(modeConfig);
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
