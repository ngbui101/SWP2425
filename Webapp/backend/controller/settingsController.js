const Settings = require('../models/Settings');
const User = require('../models/User');

// Update a user's settings by user ID
async function updateSettings(req, res) {
    const userId = req.user.id; // Extract the authenticated user ID
    const { template, language, notifications, trackerFilters, timestampFilters } = req.body;
  
    try {
      // Find the user and populate the settings
      const user = await User.findById(userId).populate('settings').exec();
      if (!user) {
        return res.status(404).json({ message: 'User not found' });
      }
  
      // Retrieve the settings document using the reference in the user's document
      const settings = user.settings;
  
      // Update fields if provided
      if (template) settings.template = template;
      if (language) settings.language = language;
      if (notifications) settings.notifications = { ...settings.notifications, ...notifications };
      if (trackerFilters) settings.trackerFilters = { ...settings.trackerFilters, ...trackerFilters };
      if (timestampFilters) settings.timestampFilters = { ...settings.timestampFilters, ...timestampFilters };
  
      // Save the updated settings
      await settings.save();
      res.status(200).json(settings); // Return updated settings directly
    } catch (error) {
      console.error('Error updating settings:', error);
      res.status(500).json({ message: 'Failed to update settings', error });
    }
  }
  

// Get a user's settings by user ID
async function getSettings(req, res) {
  const userId = req.user.id;

  try {
    // Find the user and populate the settings
    const user = await User.findById(userId).populate('settings').exec();
    if (!user) {
      return res.status(404).json({ message: 'User not found' });
    }

    res.status(200).json(user.settings);
  } catch (error) {
    console.error('Error fetching settings:', error);
    res.status(500).json({ message: 'Server error', error });
  }
}

module.exports = {
  updateSettings,
  getSettings
};
