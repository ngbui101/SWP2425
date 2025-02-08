const express = require('express');
const router = express.Router();
const { saveRoute, getSavedRoutes } = require('../controller/routeController');

// Define routes
router.post('/', saveRoute);            // POST route to save a new route
router.get('/', getSavedRoutes);        // GET route to fetch saved routes

module.exports = router;
