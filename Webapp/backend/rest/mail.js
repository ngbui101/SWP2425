// rest/mail.js
const express = require('express');
const router = express.Router();
const mailController = require('../controller/mailController');

// Route to manually send a test email
router.post('/send', async (req, res) => {
    console.log('Hit the /send route');  // Log to confirm route is hit

    const { to, subject, text } = req.body;
    const result = await mailController.sendEmail(to, subject, text);

    if (result.success) {
        res.status(200).json({ message: 'Email sent successfully', info: result.info });
    } else {
        res.status(500).json({ message: 'Failed to send email', error: result.error });
    }
});

module.exports = router;
