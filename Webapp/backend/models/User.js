const mongoose = require('mongoose');
const Schema = mongoose.Schema;

const UserSchema = new Schema(
  {
    username: {
      type: String,
      required: false
    },
    number: {
      type: String,
      required: false
    },
    email: {
      type: String,
      required: true,
      lowercase: true,
      trim: true,
      unique: true,
      validate: [
        (val) => /^\w+([\.-]?\w+)*@\w+([\.-]?\w+)*(\.\w{2,3})+$/.test(val),
      ]
    },
    password: {
      type: String,
      required: true,
      min: 6
    },
    refresh_token: String,
    tracker: [
      {
        type: Schema.Types.ObjectId,
        ref: 'Tracker'
      }
    ],
    // Reference the settings object for each user
    settings: {
      type: Schema.Types.ObjectId,
      ref: 'Settings',
      required: false
    }
  },
  {
    virtuals: {
      id: {
        get() {
          return this._id;
        }
      }
    }
  }
);

module.exports = mongoose.model('User', UserSchema);
