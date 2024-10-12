const mongoose = require('mongoose');
const Schema = mongoose.Schema;

const MeasurementSchema = new Schema(
  {
    imei: {type: String},
    mode: {
      type: String,
      enum: ['GPS', 'GSM', 'LTE M', 'NBIOT'],
    },
    latitude: {type: String},
    longitude: {type: String},
    temperature: {type: String},
    cellinfo: {type: String},
    humidity: { type: String},
    battery: {type: String},
    tracker: { type: Schema.Types.ObjectId, ref: 'Tracker', required: true }, // Verweis auf Tracker
  },
  { timestamps: true }
);

// Exportiere das Mongoose-Modell
module.exports = mongoose.model('Measurements', MeasurementSchema);
