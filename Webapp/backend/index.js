require('dotenv').config({ path: './.env' });
const express = require('express');
const cors = require('cors');
const cookieParser = require('cookie-parser');
const mongoose = require('mongoose');
const path = require('path');
const connectDB = require('./config/database');
const credentials = require('./middleware/credentials');
const axios = require('axios');
const mailController = require('./controller/mailController');
const errorHandlerMiddleware = require('./middleware/error_handler');
const authenticationMiddleware = require('./middleware/authentification');

const app = express();
const PORT = process.env.PORT || 3500;
connectDB()
// Allow Credentials
app.use(credentials);
//AWS
const device = require('./models/mqttDevice');

const corsOptions = {
  origin: "http://localhost:5173",
};

// CORS
app.use(cors({ credentials: true, origin: corsOptions }));

// application/x-www-form-urlencoded
app.use(express.urlencoded({ extended: false }));

// application/json response
app.use(express.json());

// Middleware for cookies
app.use(cookieParser());

app.use(authenticationMiddleware);

// Static files
app.use('/static', express.static(path.join(__dirname, 'public')));

// Default error handler
app.use(errorHandlerMiddleware);
app.get('/api/geocode', async (req, res) => {
  const { lat, lng } = req.query;

  if (!lat || !lng) {
    console.error("Latitude or Longitude missing from request");
    return res.status(400).json({ error: 'Latitude and longitude are required' });
  }

  const googleApiKey = process.env.GOOGLE_API_KEY; // Use API key from your .env file
  const geocodingUrl = `https://maps.googleapis.com/maps/api/geocode/json?latlng=${lat},${lng}&key=${googleApiKey}`;

  try {
    console.log(`Requesting geocode for lat: ${lat}, lng: ${lng}`);
    const response = await axios.get(geocodingUrl);
    console.log("Geocoding response received:", response.data);
    const { results } = response.data;

    if (results.length === 0) {
      console.error("No address found for the provided coordinates");
      return res.status(404).json({ error: 'No address found for the provided coordinates' });
    }

    // Return the formatted address
    res.json({
      address: results[0].formatted_address,
    });
  } catch (error) {
    console.error('Error fetching geocode data:', error.response ? error.response.data : error.message);
    res.status(500).json({ error: 'Failed to perform reverse geocoding' });
  }
});

// Routes
app.use('/api/geofence', require('../backend/rest/geofence'));
app.use('/api/event', require('../backend/rest/trainingevent'));
app.use('/api/history', require('../backend/rest/trackerhistory'));
app.use('/api/position', require('../backend/rest/measurement'));
app.use('/api/tracker', require('../backend/rest/tracker'));
app.use('/api/users', require('../backend/rest/users'));
app.use('/api/auth', require('../backend/rest/auth'));
app.use('/api/all-trackers', require('../backend/rest/alltrackers'));
app.use('/api/settings', require('../backend/rest/settings'));
app.use('/api/mail', require('./rest/mail'));
app.use('/api/mode', require('../backend/rest/mode'));

// 404 route handler
app.all('*', (req, res) => {
  res.status(404);

  if (req.accepts('json')) {
    res.json({ 'error': '404 Not Found' });
  } else {
    res.type('text').send('404 Not Found');
  }
});

mongoose.connection.once('open', () => {
  console.log('DB connected');
  app.listen(PORT, () => {
    console.log(`Listening on port ${PORT}`);
  });
});
