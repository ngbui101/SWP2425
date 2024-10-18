const TrainingEvent = require('../models/Trainingevent');
const TrackerHistory = require('../models/Trackerhistory');

// Get all training events
async function getAllTrainingEvents(req, res) {
    try {
        const trainingEvents = await TrainingEvent.find({}).populate('trackerHistory');
        res.status(200).json(trainingEvents);
    } catch (error) {
        res.status(500).json({ message: 'Server error', error });
    }
}

// Get a training event by ID
async function getTrainingEventById(req, res) {
    const { id } = req.params;

    try {
        const trainingEvent = await TrainingEvent.findById(id).populate('trackerHistory');
        if (!trainingEvent) {
            return res.status(404).json({ message: 'Training event not found' });
        }
        res.status(200).json(trainingEvent);
    } catch (error) {
        res.status(500).json({ message: 'Server error', error });
    }
}

// Füge dies zu deinem Controller hinzu
async function getTrainingEventsByTrackerId(req, res) {
    const { trackerId } = req.params;

    try {
        // Suche nach Training Events, die auf die TrackerHistory mit der angegebenen Tracker-ID verweisen
        const trainingEvents = await TrainingEvent.find({ trackerHistory: trackerId }).populate('trackerHistory');
        
        if (trainingEvents.length === 0) {
            return res.status(404).json({ message: 'No training events found for this tracker' });
        }

        res.status(200).json(trainingEvents);
    } catch (error) {
        res.status(500).json({ message: 'Server error', error });
    }
}

// Create a new training event
async function createTrainingEvent(req, res) {
    const { startTime, endTime, trackerId } = req.body;

    // Validierung des Inputs
    if (!startTime || !endTime || !trackerId) {
        return res.status(400).json({ message: 'Start time, end time, and tracker ID are required' });
    }

    // Überprüfen, ob die TrackerHistory mit der trackerId existiert
    try {
        const history = await TrackerHistory.findOne({ tracker: trackerId });
        if (!history) {
            return res.status(404).json({ message: 'TrackerHistory not found for the given tracker ID' });
        }

        const trainingEvent = new TrainingEvent({
            startTime,
            endTime,
            trackerHistory: history._id // Speichere die TrackerHistory-ID
        });

        await trainingEvent.save();
        res.status(201).json({ message: 'Training event created successfully', trainingEvent });
    } catch (error) {
        res.status(500).json({ message: 'Server error', error });
    }
}

// Update a training event by ID
async function updateTrainingEvent(req, res) {
    const { id } = req.params;
    const { startTime, endTime, trackerId } = req.body;

    try {
        // Überprüfen, ob die TrackerHistory mit der trackerId existiert, wenn sie bereitgestellt wird
        if (trackerId) {
            const history = await TrackerHistory.findOne({ tracker: trackerId });
            if (!history) {
                return res.status(404).json({ message: 'TrackerHistory not found for the given tracker ID' });
            }
        }

        const trainingEvent = await TrainingEvent.findByIdAndUpdate(id, {
            startTime,
            endTime,
            trackerHistory: trackerId ? (await TrackerHistory.findOne({ tracker: trackerId }))._id : undefined // Update nur, wenn trackerId bereitgestellt wird
        }, { new: true });

        if (!trainingEvent) {
            return res.status(404).json({ message: 'Training event not found' });
        }

        res.status(200).json({ message: 'Training event updated successfully', trainingEvent });
    } catch (error) {
        res.status(500).json({ message: 'Server error', error });
    }
}

// Delete a training event by ID
async function deleteTrainingEvent(req, res) {
    const { id } = req.params;

    try {
        const trainingEvent = await TrainingEvent.findByIdAndDelete(id);
        if (!trainingEvent) {
            return res.status(404).json({ message: 'Training event not found' });
        }

        res.status(200).json({ message: 'Training event deleted successfully' });
    } catch (error) {
        res.status(500).json({ message: 'Server error', error });
    }
}

// Exportiere die Methode
module.exports = {
    getAllTrainingEvents,
    getTrainingEventById,
    createTrainingEvent,
    updateTrainingEvent,
    deleteTrainingEvent,
    getTrainingEventsByTrackerId // Stelle sicher, dass diese exportiert wird
};
