const mongoose = require('mongoose');
const Schema = mongoose.Schema;

const MeasurementSchema = new Schema(
  {
    type: {
      type: String,
      enum: ['Position', 'Temperature', 'Humidity', 'None'],
      default: 'None',
    },
    latitude: {
      type: Number,
      required: function () {
        return this.type === 'Position';
      },
    },
    longitude: {
      type: Number,
      required: function () {
        return this.type === 'Position';
      },
    },
    temperature: {
      type: Number,
      required: function () {
        return this.type === 'Temperature';
      },
    },
    humidity: {
      type: Number,
      required: function () {
        return this.type === 'Humidity';
      },
    },
    mode: {
      type: String,
      enum: ['GPS', 'LTE', 'IOT'],
      required: function () {
        return this.type === 'Position'; // mode ist nur erforderlich, wenn der Typ 'Position' ist
      },
    },
    tracker: { type: Schema.Types.ObjectId, ref: 'Tracker', required: true }, // Verweis auf Tracker
  },
  { timestamps: true }
);

// Exportiere das Mongoose-Modell
module.exports = mongoose.model('Measurements', MeasurementSchema);
