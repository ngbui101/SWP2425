const bcrypt = require('bcryptjs');

const User = require('../models/User')
const Tracker = require('../models/Tracker');

async function addTrackerToUser(req, res) {
  const {trackerId } = req.params;
  const userId = req.user.id;
  try {
    // Prüfe, ob der Benutzer existiert
    const user = await User.findById(userId);
    if (!user) {
      return res.status(404).json({ message: 'User not found' });
    }

    // Prüfe, ob der Tracker existiert
    const tracker = await Tracker.findById(trackerId);
    if (!tracker) {
      return res.status(404).json({ message: 'Tracker not found' });
    }

    // Prüfe, ob der Tracker bereits dem Benutzer hinzugefügt wurde
    if (user.tracker.includes(trackerId)) {
      return res.status(400).json({ message: 'Tracker already added to user' });
    }

    // Füge den Tracker dem Benutzer hinzu
    user.tracker.push(trackerId);
    await user.save();

    res.status(200).json({ message: 'Tracker added to user successfully', user });
  } catch (error) {
    console.error('Error adding tracker to user:', error);
    res.status(500).json({ message: 'Failed to add tracker to user', error });
  }
}
// Get all users
async function getAllUsers(req, res) {
  try {
    const users = await User.find({}, '-password'); // Exclude the password field
    res.status(200).json(users);
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

// Get a user by ID
async function getUserById(req, res) {
  const { id } = req.params;

  try {
    const user = await User.findById(id, '-password').populate('settings'); // Populate settings
    if (!user) {
      return res.status(404).json({ message: 'User not found' });
    }
    res.status(200).json(user);
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

// Create a new user
async function createUser(req, res) {
  const { username, email, password} = req.body;

  try {
    const user = new User({
      username,
      email,
      password, // Ensure password is hashed in the User model before saving
      tracker: [],
    });

    await user.save();
    res.status(201).json({ message: 'User created successfully', user });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

async function updateUser(req, res) {
  const { id } = req.params;
  const { email, language, currentPassword, newPassword, confirmPassword, number, template } = req.body;
  
  try {
    const user = await User.findById(id);
    if (!user) {
      return res.status(404).json({ message: 'User not found' });
    }

    // Update email if provided
    if (email) {
      user.email = email;
    }

    // Update language if provided
    if (language) {
      user.language = language;
    }

    // Handle phone number update
    if (number) {
      user.number = number;
    }
    if (template) {
      user.template = template;
    }

    // Handle password change
    if (currentPassword && newPassword && confirmPassword) {
      const isMatch = await bcrypt.compare(currentPassword, user.password);
      if (!isMatch) {
        return res.status(400).json({ message: 'Current password is incorrect' });
      }

      if (newPassword !== confirmPassword) {
        return res.status(400).json({ message: 'New passwords do not match' });
      }

      const hashedPassword = await bcrypt.hash(newPassword, 10);
      user.password = hashedPassword;
    }

    await user.save();
    res.status(200).json({ message: 'User updated successfully', user });

  } catch (error) {
    console.error('Error updating user:', error);
    res.status(500).json({ message: 'Failed to update user', error });
  }
}



// Delete a user by ID
async function deleteUser(req, res) {
  const { id } = req.params;

  try {
    const user = await User.findByIdAndDelete(id);
    if (!user) {
      return res.status(404).json({ message: 'User not found' });
    }

    res.status(200).json({ message: 'User deleted successfully' });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
}

module.exports = {
  getAllUsers,
  getUserById,
  createUser,
  updateUser,
  deleteUser,
  addTrackerToUser
};
