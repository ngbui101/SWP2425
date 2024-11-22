const mongoose = require('mongoose');
const Schema = mongoose.Schema;

const ModeSchema = new Schema({
  // Unique identifier for the Mode document
  _id: {
    type: Schema.Types.ObjectId,
    required: true,
    auto: true,
  },
  
  // Boolean fields for different tracking modes
  GnssMode: {
    type: Boolean,
    default: false
  },
  CellInfosMode: {
    type: Boolean,
    default: true
  },
  BatteryMode: {
    type: Boolean,
    default: true
  },
  TemperatureMode: {
    type: Boolean,
    default: true
  },
  NmeaMode: {
    type: Boolean,
    default: false
  },

  // Reference to the associated tracker
  tracker: {
    type: Schema.Types.ObjectId,
    ref: 'Tracker',
    required: true
  },

  // Frequency setting in milliseconds
  frequenz: {
    type: Number,
    default: 3600000
  }
}, { collection: 'mode' }); // Explicitly set the collection name to "mode"

module.exports = mongoose.model('Mode', ModeSchema);
