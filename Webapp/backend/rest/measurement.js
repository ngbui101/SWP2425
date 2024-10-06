//TOOK AUTHORIZATION OUT SO WE CAN TEST WITHOUT LOGIN
const express = require('express');
const router = express.Router();
const measurementController = require('../controller/measurementController');
const authenticate = require('../middleware/auth');

// Route to get all measurements
router.get('/', authenticate, measurementController.getAllMeasurements);

// Route to get a single measurement by ID
router.get('/:id', authenticate, measurementController.getMeasurementById);

// Route to get all measurements for a specific tracker, with optional type filter
router.get('/tracker/:trackerId', authenticate, measurementController.getMeasurementsByTrackerId);

// Route to create a new position measurement
router.post('/position', authenticate, measurementController.measurePosition);

// Route to create a new temperature measurement
router.post('/temperature', authenticate, measurementController.measureTemperature);

// Route to create a new humidity measurement
router.post('/humidity', authenticate, measurementController.measureHumidity);

// Route to delete a measurement by ID
router.delete('/:id', authenticate, measurementController.deleteMeasurement);

// Route to set the mode of a measurement by ID
router.put('/:id/mode', authenticate, measurementController.setPositionMode);

module.exports = router;
