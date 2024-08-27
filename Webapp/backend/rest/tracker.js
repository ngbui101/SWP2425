const express = require('express');
const router = express.Router();
const trackerController = require('../controller/trackerController');
const authenticate = require('../middleware/auth');

// Route to get all trackers
router.get('/', authenticate, trackerController.getAllTrackers);

// Route to get a single tracker by ID
router.get('/:id', authenticate, trackerController.getTrackerById);

// Route to create a new tracker
router.post('/', authenticate, trackerController.createTracker);

// Route to update a tracker by ID
router.put('/:id', authenticate, trackerController.updateTrackerName);

// Route to delete a tracker by ID
router.delete('/:id', authenticate, trackerController.deleteTracker);

// Route to set the mode of a tracker by ID
router.put('/:id/mode', authenticate, trackerController.setTrackerMode);

module.exports = router;
