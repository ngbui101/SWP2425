// controller/mailController.js
const transporter = require('../config/mailer'); // Load transporter from mailer config

// Function to send an email
const sendEmail = async (to, subject, text) => {
    const mailOptions = {
        from: process.env.GMAIL_USER,
        to,
        subject,
        text
    };

    try {
        const info = await transporter.sendMail(mailOptions);
        console.log('Email sent:', info);
        return { success: true, info };
    } catch (error) {
        console.error('Error sending email:', error);
        return { success: false, error };
    }
};

module.exports = { sendEmail };  // Export the sendEmail function
