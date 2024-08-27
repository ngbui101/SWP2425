//Schema for a Position. Mode should only be GPS / LTE /IOT 
const mongoose = require('mongoose');
const Schema = mongoose.Schema;

const PositionSchema = new Schema ( {
    id: {type: Number, required: true},
    name : {type: String},
    latitude : {type: Number},
    longitude : {type: Number},
    altitude : {type: Number},
    mode: {
        type: String,
        enum: ['GPS','LTE','IOT'],
        default: 'None'
    },
    
  }, {timestamps: true})

module.exports = mongoose.model('Position', PositionSchema);