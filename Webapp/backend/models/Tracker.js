//Schema for a Tracker. Mode is managed by String. Should only be RT or LT or Test for Realtime, Longtime or Testing
const mongoose = require('mongoose');
const Schema = mongoose.Schema;

const TrackerSchema = new Schema ( {
    imei : {type: String},
    name : {type: String},
    mode: {
        type: String,
        enum: ['RT','LT','NB'],
        default: 'LT'
    },
    geofence: { type: Schema.Types.ObjectId, ref: 'Geofence' },
    trackerhistory: {type: Schema.Types.ObjectId, ref: 'Trackerhistory'}
  })

module.exports = mongoose.model('Tracker', TrackerSchema);