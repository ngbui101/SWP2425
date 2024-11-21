const mongoose = require('mongoose');
const Schema = mongoose.Schema;

const GeofenceSchema = new Schema({
  tracker: { type: Schema.Types.ObjectId, ref: 'Tracker', required: true }, // Reference to Tracker
  radius: { type: Number },
  longitude: { type: String },
  latitude: { type: String },
  active: { type: Boolean, default: false } // Boolean to track if geofence is active
});

module.exports = mongoose.model('Geofence', GeofenceSchema);
