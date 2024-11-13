const express = require('express');
const router = express.Router();
const allTrackersController = require('../controller/allTrackersController');


// Route to create a new tracker
router.post('/', allTrackersController.createAllTracker);

// Route to update an existing tracker by IMEI
router.put('/:imei', allTrackersController.updateAllTracker);

// Route to delete a tracker by IMEI
router.delete('/:imei', allTrackersController.deleteAllTracker);

// Route to get all trackers
router.get('/', allTrackersController.getAllTrackers);

module.exports = router;
