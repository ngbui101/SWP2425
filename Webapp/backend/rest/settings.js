const express = require('express');
const router = express.Router();
const settingsController = require('../controller/settingsController');
const authenticate = require('../middleware/auth');

// Update settings for the authenticated user
router.put('/', authenticate, settingsController.updateSettings);

// Get settings for the authenticated user
router.get('/', authenticate, settingsController.getSettings);

module.exports = router;
