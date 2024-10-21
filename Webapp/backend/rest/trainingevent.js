const express = require('express');
const router = express.Router();
const trainingEventController = require('../controller/trainingeventController');
const authenticate = require('../middleware/auth');

// Route to get all training events
router.get('/', authenticate, trainingEventController.getAllTrainingEvents);

// Route to get a single training event by ID
router.get('/:id', authenticate, trainingEventController.getTrainingEventById);

// Route to get all training events for a specific tracker
router.get('/tracker/:trackerId', authenticate, trainingEventController.getTrainingEventsByTrackerId); // Hinzugefügt

// Route to create a new training event
router.post('/', authenticate, trainingEventController.createTrainingEvent);

// Route to update a training event by ID
router.put('/:id', authenticate, trainingEventController.updateTrainingEvent);

// Route to delete a training event by ID
router.delete('/:id', authenticate, trainingEventController.deleteTrainingEvent);

module.exports = router;
