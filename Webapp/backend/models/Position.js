//Schema for a Position. Mode should only be GPS / LTE /IOT 
const mongoose = require('mongoose');
const Schema = mongoose.Schema;

const PositionSchema = new Schema ( {
   
    latitude : {type: Number},
    longitude : {type: Number},
    altitude : {type: Number},
    mode: {
        type: String,
        enum: ['GPS','LTE','IOT','None'],
        default: 'None'
    },
    
  }, {timestamps: true})

module.exports = mongoose.model('Position', PositionSchema);