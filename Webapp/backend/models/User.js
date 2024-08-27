const mongoose = require('mongoose')
// This line gets the Schema constructor from Mongoose, which is used to define the structure of documents in a MongoDB collection.
const Schema = mongoose.Schema

// The UserSchema defines the structure of a user document with the following fields:
const UserSchema = Schema(
  {
    username: {
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
          tracker: {type: Schema.Types.ObjectId, ref: 'Tracker'}
      }
  ]
  },
  /* The second argument to the Schema constructor is an options object, which includes:

virtuals: An object defining virtual properties. These are properties not stored in the database but derived from existing fields.
full_name: A virtual property that concatenates first_name and last_name.
id: A virtual property that returns the document's _id.
timestamps: An option to automatically manage created_at and updated_at fields. */
  {
    virtuals: {
      

      id: {
        get() {
          return this._id
        }
      }
    }
    
  },

)

module.exports = mongoose.model('User', UserSchema)