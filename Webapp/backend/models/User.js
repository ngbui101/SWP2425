const mongoose = require('mongoose');
const Schema = mongoose.Schema;

// The UserSchema defines the structure of a user document with the following fields:
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
    language: {
      type: String,
      enum: ['EN', 'DE'],
      default: 'EN',
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
    // Add the template field to manage the color scheme
    template: {
      type: String,
      default: 'default'  // Default theme will be 'default'
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
