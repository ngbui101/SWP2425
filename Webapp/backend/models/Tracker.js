//Schema for a Tracker. Mode is managed by String. Should only be RT or LT or Test for Realtime, Longtime or Testing
const mongoose = require('mongoose');
const Schema = mongoose.Schema;

const TrackerSchema = new Schema ( {
   
    name : {type: String},
    mode: {
        type: String,
        enum: ['RT','LT','Test', 'None'],
        default: 'None'
    },
  })

module.exports = mongoose.model('Tracker', TrackerSchema);