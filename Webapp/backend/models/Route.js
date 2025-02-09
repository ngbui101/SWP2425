const mongoose = require('mongoose');

const routeSchema = new mongoose.Schema({
    userId: {
        type: mongoose.Schema.Types.ObjectId,
        ref: 'User',
        required: true,
    },
    name: {
        type: String,
        required: true,
    },
    positions: [
        {
            lat: { type: Number, required: true },
            lng: { type: Number, required: true },
        },
    ],
    totalTime: {
        type: Number,  // In seconds
        required: true,
    },
    steps: {
        type: Number,
        required: true,
    },
    avgSpeed: {
        type: Number,  // In m/s
        required: true,
    },
    maxSpeed: {
        type: Number,  // In m/s
        required: true,
    },
    caloriesBurned: {
        type: Number,
        default: 0,  // Optional for now, calculation can be added later
    },
    createdAt: {
        type: Date,
        default: Date.now,
    },
});

module.exports = mongoose.model('Route', routeSchema);
