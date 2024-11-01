const User = require('../models/User')
const jwt = require('jsonwebtoken')
const bcrypt = require('bcryptjs')
const mailController = require('../controller/mailController');
const Settings = require('../models/Settings');
/* Purpose: Register a new user

1.Extract user details from the request body.
2.Check for missing fields and mismatched passwords, returning 422 status if invalid.
3.Check if a user with the same email already exists, returning 409 status if true.
4.Hash the password using bcrypt.
5.Create a new user in the database.
6.Return 201 status on success or 400 on error.
*/

async function register(req, res) {
  const { email, password, password_confirm } = req.body;

  // Check for missing fields
  if (!email || !password || !password_confirm) {
    return res.status(422).json({ message: 'Invalid fields' });
  }

  // Check if passwords match
  if (password !== password_confirm) {
    return res.status(422).json({ message: 'Passwords do not match' });
  }

  // Check if a user with the same email already exists
  const userExists = await User.exists({ email }).exec();
  if (userExists) {
    return res.sendStatus(409); // Conflict: User with the same email already exists
  }

  try {
    // Hash the password
    const hashedPassword = await bcrypt.hash(password, 10);

    // Step 1: Create a new Settings document with default values
    const newSettings = await Settings.create({});

    // Step 2: Create the new User document with a reference to the Settings document
    const newUser = await User.create({
      email,
      password: hashedPassword,
      tracker: [], // Empty tracker array by default
      settings: newSettings._id // Link the settings document to the user
    });

    return res.sendStatus(201); // Successfully created
  } catch (error) {
    console.error('Error during user registration:', error);
    return res.status(400).json({ message: 'Could not register' });
  }
}

module.exports = { register };
/* Purpose: Authenticate a user and issue JWT Tokens
1. Extract email and password from the request body.
2. Check for missing fields, returning 422 status if invalid.
3. Find the user by email, returning 401 status if not found.
4. Compare the provided password with the stored hashed password, returning 401 status if they do not match.
5. Create an access token and a refresh token using JWT.
6. Store the refresh token in the user document.
7. Set a secure HTTP-only cookie with the refresh token and return the access token in the response.
*/
async function login(req, res) {
  const { email, password } = req.body

  if (!email || !password) return res.status(422).json({ 'message': 'Invalid fields' })

  const user = await User.findOne({ email }).exec()

  if (!user) return res.status(401).json({ message: "Email or password is incorrect" })

  const match = await bcrypt.compare(password, user.password)

  if (!match) return res.status(401).json({ message: "Email or password is incorrect" })

  const accessToken = jwt.sign(
    {
      id: user.id
    },
    process.env.ACCESS_TOKEN_SECRET,
    {
      expiresIn: '1800s'
    }
  )

  const refreshToken = jwt.sign(
    {
      id: user.id
    },
    process.env.REFRESH_TOKEN_SECRET,
    {
      expiresIn: '1d'
    }
  )

  user.refresh_token = refreshToken
  await user.save()

  res.cookie('refresh_token', refreshToken, { httpOnly: true, sameSite: 'None', secure: true, maxAge: 24 * 60 * 60 * 1000 })
  res.json({ access_token: accessToken })
}

/* Purpose: Log out a user by invalidating the refresh token.
1. Check for the presence of a refresh token in cookies, returning 204 status if not found.
2. Find the user by the refresh token.
3. If the user is not found, clear the refresh token cookie and return 204 status.
4. Set the user's refresh token to null and save the user document.
5. Clear the refresh token cookie and return 204 status. */
async function logout(req, res) {
  const cookies = req.cookies

  if (!cookies.refresh_token) return res.sendStatus(204)

  const refreshToken = cookies.refresh_token
  const user = await User.findOne({ refresh_token: refreshToken }).exec()

  if (!user) {
    res.clearCookie('refresh_token', { httpOnly: true, sameSite: 'None', secure: true })
    return res.sendStatus(204)
  }

  user.refresh_token = null
  await user.save()

  res.clearCookie('refresh_token', { httpOnly: true, sameSite: 'None', secure: true })
  res.sendStatus(204)
}
/* Purpose: Issue a new access token using a valid refresh token.
1. Check for the presence of a refresh token in cookies, returning 401 status if not found.
2. Find the user by the refresh token.
3. If the user is not found, return 403 status.
4. Verify the refresh token using JWT. If invalid or mismatched user ID, return 403 status.
5. Create a new access token and return it in the response. */
async function refresh(req, res) {
  const cookies = req.cookies
  if (!cookies.refresh_token) return res.sendStatus(401)

  const refreshToken = cookies.refresh_token

  const user = await User.findOne({ refresh_token: refreshToken }).exec()

  if (!user) return res.sendStatus(403)

  jwt.verify(
    refreshToken,
    process.env.REFRESH_TOKEN_SECRET,
    (err, decoded) => {
      if (err || user.id !== decoded.id) return res.sendStatus(403)

      const accessToken = jwt.sign(
        { id: decoded.id },
        process.env.ACCESS_TOKEN_SECRET,
        { expiresIn: '1800s' }
      )

      res.json({ access_token: accessToken })
    }
  )
}

/* Purpose: Return the currently authenticated users details
1. Extract the user details from the request (assumes user is already authenticated).
2. Return the user details with 200 status. */

async function user(req, res) {
  try {
    // Find the user by ID and populate the settings field
    const user = await User.findById(req.user.id).populate('settings');
    
    if (!user) {
      return res.status(404).json({ message: 'User not found' });
    }

    return res.status(200).json(user);
  } catch (error) {
    console.error('Error fetching authenticated user:', error);
    return res.status(500).json({ message: 'Server error', error });
  }
}

// Utility function to generate a random password
function generateRandomPassword(length) {
  const chars = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789';
  let password = '';
  for (let i = 0; i < length; i++) {
    const randomIndex = Math.floor(Math.random() * chars.length);
    password += chars[randomIndex];
  }
  return password;
}

// Reset password function
async function resetPassword(req, res) {
  const { email } = req.body;

  if (!email) return res.status(422).json({ message: 'Email is required' });

  try {
    // Find user by email
    const user = await User.findOne({ email }).exec();

    if (!user) {
      return res.status(404).json({ success: false, message: 'No account found with that email.' });
    }

    // Generate a new random password
    const newPassword = generateRandomPassword(8);

    // Hash the new password
    const hashedPassword = await bcrypt.hash(newPassword, 10);

    // Update user's password in the database
    user.password = hashedPassword;
    await user.save();

    // Send the new password to the user's email
    const subject = 'Your New Password for BOTracker';
    const text = `Hi, here is your new password: ${newPassword}. Please change it after logging in.`;

    await mailController.sendEmail(user.email, subject, text);

    return res.status(200).json({ success: true, message: 'A new password has been sent to your email.' });
  } catch (error) {
    console.error('Error resetting password:', error);
    return res.status(500).json({ success: false, message: 'Server error' });
  }
}

module.exports = { register, login, logout, refresh, user, resetPassword }