const mongoose = require('mongoose');

const allTrackersSchema = new mongoose.Schema({
    imei: {
        type: String,
        required: true,
        unique: true
    },
    pin: {
        type: String,
        required: true
    },
    isRegistered: {
        type: Boolean,
        default: false
    }
});

module.exports = mongoose.model('AllTracker', allTrackersSchema);
