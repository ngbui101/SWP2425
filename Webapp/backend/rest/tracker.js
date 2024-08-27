const express = require('express');
const router = express.Router();
const trackerController = require('../controller/trackerController');
const authenticate = require('../middleware/auth');

// Route to get all trackers
router.get('/',  trackerController.getAllTrackers);

// Route to get a single tracker by ID
router.get('/:id',  trackerController.getTrackerById);

// Route to create a new tracker
router.post('/', trackerController.createTracker);

// Route to update a tracker by ID
router.put('/:id',  trackerController.updateTrackerName);

// Route to delete a tracker by ID
router.delete('/:id',  trackerController.deleteTracker);

// Route to set the mode of a tracker by ID
router.put('/:id/mode',  trackerController.setTrackerMode);

module.exports = router;
