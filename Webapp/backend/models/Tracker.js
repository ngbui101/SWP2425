const mongoose = require('mongoose');
const Schema = mongoose.Schema;

const TrackerSchema = new Schema({
    imei: { type: String },
    name: { type: String },
    
    // Reference to Mode model instead of enum
    mode: {
        type: Schema.Types.ObjectId,
        ref: 'Mode',
        required: true
    },

    geofence: { type: Schema.Types.ObjectId, ref: 'Geofence' },
    trackerhistory: { type: Schema.Types.ObjectId, ref: 'Trackerhistory' }
});

module.exports = mongoose.model('Tracker', TrackerSchema);
