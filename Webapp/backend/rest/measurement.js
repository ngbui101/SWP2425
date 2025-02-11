// TOOK AUTHORIZATION OUT SO WE CAN TEST WITHOUT LOGIN
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

// Route to delete a measurement by ID
router.delete('/:id', authenticate, measurementController.deleteMeasurement);
router.get("/tracker/:trackerId/latest", measurementController.getLatestMeasurementByTrackerId);
// Route to create a new measurement
router.post('/', authenticate, measurementController.createMeasurement); // Add this line

module.exports = router;
