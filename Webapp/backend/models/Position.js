const mongoose = require('mongoose');
const Schema = mongoose.Schema;

const PositionSchema = new Schema({
  latitude: { type: Number },
  longitude: { type: Number },
  mode: {
    type: String,
    enum: ['GPS', 'LTE', 'IOT'],
    default: 'None'
  },
  tracker: { type: Schema.Types.ObjectId, ref: 'Tracker', required: true } // Verweis auf Tracker
}, { timestamps: true });

module.exports = mongoose.model('Position', PositionSchema);
