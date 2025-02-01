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
    mode: {
      type: [String],  // Array of strings to store selected modes
      enum: ['RT', 'LT', 'GSM', 'NBIOT'],  // Allowed values: Real-Time ('RT') and Long-Time ('LT')
      default: []  // Default to an empty array, meaning no filter applied
    }
  },
  timestampFilters: { 
    mode: {
      type: [String],  // Array of strings to store selected modes
      enum: ['RT', 'LT','GSM', 'NBIOT'],  // Allowed values: Real-Time ('RT') and Long-Time ('LT')
      default: []  // Default to an empty array, meaning no filter applied
    },
    validPosition: {
      type: Boolean,  // Boolean to filter by valid position or not
      default: false
    }
  }
});

module.exports = mongoose.model('Settings', SettingsSchema);
