const express = require('express')
const router = express.Router()
const authControllers = require('../controller/authController')
const authMiddleware = require('../middleware/auth')


router.post('/register', authControllers.register)

router.post('/login', authControllers.login)

router.post('/logout', authControllers.logout)

router.post('/refresh', authControllers.refresh)
router.post('/forgot-password', authControllers.resetPassword);
router.get('/user', authMiddleware, authControllers.user)
module.exports = router