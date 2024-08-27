//TOOK AUTHORIZATION OUT SO WE CAN TEST WITHOUT LOGIN
const express = require('express');
const router = express.Router();
const positionController = require('../controller/positionController');
const authenticate = require('../middleware/auth');

// Route to get all positions
router.get('/', authenticate, positionController.getAllPositions);
// Route to get a single position by ID
router.get('/:id', authenticate, positionController.getPositionById);
// Route to create a new position
router.post('/',authenticate,  positionController.createPosition);
// Route to delete a position by ID
router.delete('/:id', authenticate, positionController.deletePosition);
// Route to set the mode of a position by ID
router.put('/:id/mode', authenticate, positionController.setPositionMode);

module.exports = router;
