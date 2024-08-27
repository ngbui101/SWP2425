const express = require('express');
const router = express.Router();
const userController = require('backend/controller/userController');
const authenticate = require('../../middleware/auth');

// Route to get all users
router.get('/', authenticate,  userController.getAllUsers);
// Route to get a single user by ID
router.get('/:id', authenticate,  userController.getUserById);
// Route to create a new user
router.post('/', authenticate,  userController.createUser);
// Route to update a user by ID
router.put('/:id', authenticate, userController.updateUser);
// Route to delete a user by ID
router.delete('/:id', authenticate, userController.deleteUser);


module.exports = router;
