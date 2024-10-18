// config/mailer.js
const nodemailer = require('nodemailer');
require('dotenv').config(); // Ensure environment variables are loaded

// Create transporter object using the default SMTP transport
const transporter = nodemailer.createTransport({
    service: 'gmail',  // Use Gmail service
    auth: {
        user: process.env.GMAIL_USER,  // Your Gmail email
        pass: process.env.GMAIL_PASS   // Your Gmail app password
    }
});

module.exports = transporter;  // Export the transporter
