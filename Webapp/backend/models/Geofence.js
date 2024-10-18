//Schema for a Tracker. Mode is managed by String. Should only be RT or LT or Test for Realtime, Longtime or Testing
const mongoose = require('mongoose');
const Schema = mongoose.Schema;

const GeofenceSchema = new Schema ( {
    tracker: { type: Schema.Types.ObjectId, ref: 'Tracker', required: true }, // Verweis auf Tracker
    radius: {type: Number},
    longitude: {type: String},
    latitude: {type: String} 
  })

module.exports = mongoose.model('Geofence', GeofenceSchema);