const mongoose = require('mongoose');
const Schema = mongoose.Schema;

const TrackerHistorySchema = new Schema ( {
    tracker: { type: Schema.Types.ObjectId, ref: 'Tracker', required: true, unique: true }, // Eindeutige Tracker-ID
    positions: [
        {
            position: {type: Schema.Types.ObjectId, ref: 'Measurements'}
        }
    ]
  })

module.exports = mongoose.model('TrackerHistory', TrackerHistorySchema);