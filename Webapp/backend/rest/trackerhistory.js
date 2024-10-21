const express = require('express');
const router = express.Router();
const trackerHistoryController = require('../controller/trackerhistoryController');
const authenticate = require('../middleware/auth');

// Route to get all tracker histories
router.get('/', authenticate, trackerHistoryController.getAllTrackerHistories);

// Route to get a single tracker history by ID
router.get('/:id', authenticate, trackerHistoryController.getTrackerHistoryById);

// Route to get all tracker histories for a specific tracker
router.get('/tracker/:trackerId', authenticate, trackerHistoryController.getTrackerHistoriesByTrackerId);

// Route to create a new tracker history
router.post('/', authenticate, trackerHistoryController.createTrackerHistory);

// Route to update a tracker history by ID (e.g., add new positions)
router.put('/:id', authenticate, trackerHistoryController.updateTrackerHistory);

// Route to delete a tracker history by ID
router.delete('/:id', authenticate, trackerHistoryController.deleteTrackerHistory);

module.exports = router;
