const mongoose = require('mongoose');
const Schema = mongoose.Schema;

const SettingsSchema = new Schema({
  // Notification settings for SMS and email alerts
  notifications: {
    sms: {
      geofence: { type: Boolean, default: false },
      motionSensor: { type: Boolean, default: false },
      batteryLow: { type: Boolean, default: false }
    },
    email: {
      geofence: { type: Boolean, default: false },
      motionSensor: { type: Boolean, default: false },
      batteryLow: { type: Boolean, default: false }
    }
  },

  // User preferences
  template: {
    type: String,
    default: 'dark'
  },
  language: {
    type: String,
    enum: ['EN', 'DE'],
    default: 'EN'
  },

  // Filter settings
  trackerFilters: {
    mode: { type: Boolean, default: false }  // Filter tracker by mode
  },
  timestampFilters: {
    mode: { type: Boolean, default: false },  // Filter timestamps by mode
    validPosition: { type: Boolean, default: false }  // Filter timestamps by valid position
  }
});

module.exports = mongoose.model('Settings', SettingsSchema);
