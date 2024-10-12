const mongoose = require('mongoose');
const Schema = mongoose.Schema;

const TrackerHistorySchema = new Schema({
  tracker: { type: Schema.Types.ObjectId, ref: 'Tracker', required: true, unique: true }, // Unique Tracker ID
  measurements: [
    {
      imei: { type: String },        // Store imei directly
      mode: { type: String },        // Store mode directly
      latitude: { type: Number },    // Store latitude directly
      longitude: { type: Number },   // Store longitude directly
      temperature: { type: Number },  // Store temperature directly
      cellinfo: { type: String },    // Store cell info directly
      humidity: { type: Number },     // Store humidity directly
      battery: { type: Number },      // Store battery level directly
      createdAt: { type: Date, default: Date.now } // Optionally track when the measurement was made
    }
  ]
});

module.exports = mongoose.model('TrackerHistory', TrackerHistorySchema);
