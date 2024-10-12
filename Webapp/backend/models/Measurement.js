const mongoose = require('mongoose');
const Schema = mongoose.Schema;

const MeasurementSchema = new Schema(
  {
    imei: {type: String},
    mode: {
      type: String,
      enum: ['GPS', 'GSM', 'LTE M', 'NBIOT'],
    },
    latitude: {type: Number},
    longitude: {type: Number},
    temperature: {type: Number},
    cellinfo: {type: String},
    humidity: { type: Number},
    battery: {type: Number},
    tracker: { type: Schema.Types.ObjectId, ref: 'Tracker', required: true }, // Verweis auf Tracker
  },
  { timestamps: true }
);

// Exportiere das Mongoose-Modell
module.exports = mongoose.model('Measurements', MeasurementSchema);
