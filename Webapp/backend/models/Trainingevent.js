const mongoose = require('mongoose');
const Schema = mongoose.Schema;

const TrainingEventSchema = new Schema({
    startTime: { type: Date, required: true }, // Startzeit des Trainings
    endTime: { type: Date, required: true },   // Endzeit des Trainings
    trackerHistory: { type: Schema.Types.ObjectId, ref: 'TrackerHistory', required: true } // Referenz auf ein einzelnes TrackerHistory
});

module.exports = mongoose.model('TrainingEvent', TrainingEventSchema);
