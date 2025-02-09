const Route = require('../models/Route');

// Save a new route
const saveRoute = async (req, res) => {
    try {
        const { name, positions, totalTime, steps, avgSpeed, maxSpeed, caloriesBurned } = req.body;

        // Validate required fields
        if (!name || !positions || !totalTime || !steps || !avgSpeed || !maxSpeed) {
            return res.status(400).json({ message: 'Missing required fields' });
        }

        const newRoute = new Route({
            userId: req.user._id,  // Assuming user is authenticated and available in req.user
            name,
            positions,
            totalTime,
            steps,
            avgSpeed,
            maxSpeed,
            caloriesBurned: caloriesBurned || 0,
        });

        await newRoute.save();
        res.status(201).json(newRoute);
    } catch (error) {
        console.error('Error saving route:', error);
        res.status(500).json({ message: 'Server error' });
    }
};

// Fetch all saved routes for a user
const getSavedRoutes = async (req, res) => {
    try {
        const routes = await Route.find({ userId: req.user._id });
        res.status(200).json(routes);
    } catch (error) {
        console.error('Error fetching routes:', error);
        res.status(500).json({ message: 'Server error' });
    }
};

module.exports = {
    saveRoute,
    getSavedRoutes,
};
