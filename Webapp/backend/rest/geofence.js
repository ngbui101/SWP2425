const express = require('express');
const router = express.Router();
const geofenceController = require('../controller/geofenceController');
const authenticate = require('../middleware/auth');

// Route to create a new geofence
router.post('/', authenticate, geofenceController.createGeofence);

// Route to get a geofence by ID
router.get('/:id', authenticate, geofenceController.getGeofenceById);

// Route to update a geofence by ID
router.put('/:id', authenticate, geofenceController.updateGeofence);

// Route to delete a geofence by ID
router.delete('/:id', authenticate, geofenceController.deleteGeofence);

module.exports = router;
