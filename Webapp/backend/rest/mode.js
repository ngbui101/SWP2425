const express = require('express');
const router = express.Router();
const modeController = require('../controller/modeController');
const authenticate = require('../middleware/auth');

// Update mode configuration for a specific tracker (authenticated user)
router.put('/:trackerId', authenticate, modeController.updateMode);

// Get mode configuration for a specific tracker (authenticated user)
router.get('/:trackerId', authenticate, modeController.getMode);

module.exports = router;
