
const express = require('express');
const mongoose = require('mongoose');
const app = express();
const http = require('http');
const server = http.createServer(app);

const credentials = require('./middleware/credentials');
const errorHandlerMiddleware = require('./middleware/error_handler');
const authenticationMiddleware = require('./middleware/authentification');

mongoose.connect('mongodb+srv://sop:1234@softwarepraktikum.iporc.mongodb.net/SOP', {
    useUnifiedTopology: true,
    useNewUrlParser: true
  }).then(() => {
    console.log('Connected to MongoDB');
  }).catch((error) => {
    console.error('Error connecting to MongoDB:', error);
  })

// Routes
app.use('/api/position', require('../backend/rest/position'));
app.use('/api/tracker', require('../backend/rest/tracker'));
app.use('/api/users', require('../backend/rest/users')); 

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
    server.listen(9999, () => {
      console.log(`Listening on port 9999`);
    });
  });