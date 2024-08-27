//Schema for a Tracker. Mode is managed by String. Should only be RT or LT or Test for Realtime, Longtime or Testing
const mongoose = require('mongoose');
const Schema = mongoose.Schema;

const TrackerSchema = new Schema ( {
    id: {typ: Number, required: true},
    name : {type: String},
    mode: {
        type: String,
        enum: ['RT','LT','Test'],
        default: 'None'
    },
    positions: [
        {
            position: {type: Schema.Types.ObjectId, ref: 'Position'}
        }
    ]
  })

module.exports = mongoose.model('Tracker', TrackerSchema);