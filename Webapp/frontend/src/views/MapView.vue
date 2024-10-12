<template>
  <div class="container">
    <!-- Tracker Info Card -->
    <div class="tracker-info-card">
      <div class="card-body">
        <label for="tracker-dropdown" class="dropdown-label">
          Select Tracker:
          <select id="tracker-dropdown" class="tracker-dropdown" v-model="selectedTracker" @change="selectTracker">
            <option v-for="tracker in trackers" :key="tracker._id" :value="tracker._id">
              {{ tracker.name }}
            </option>
          </select>
        </label>

        <label for="timestamp-dropdown" class="dropdown-label">
          Select Timestamp:
          <select id="timestamp-dropdown" v-model="selectedTimestamp" @change="updateSelectedMeasurement">
            <option v-for="measurement in selectedTrackerMeasurements" :key="measurement._id" :value="measurement._id">
              {{ new Date(measurement.createdAt).toLocaleString() }}
            </option>
          </select>
        </label>

        <div class="grid-container">
          <!-- Mode Information -->
          <div class="grid-item-full mode-item">
            <strong>Mode:</strong>
            <span v-if="trackerMode === 'RT'">Real-Time</span>
            <span v-else>{{ trackerMode || 'N/A' }}</span>

            <!-- Mode Badges for GPS and LTE -->
            <span v-if="trackerMode === 'RT'" class="mode-badge">GPS</span>
            <span v-if="trackerMode === 'LT'" class="mode-badge">LTE</span>

            <!-- Switch Mode Button -->
            <button class="switch-mode-button shimmering-button" @click="switchMode">Switch Mode</button>
          </div>

          <!-- Battery Status -->
          <div class="grid-item-full">
            <div class="battery-status-wrapper">
              <strong>Battery Status:</strong>
              <div class="battery-wrapper">
                <div class="battery-bar">
                  <div class="battery-fill" :style="{ width: `${selectedMeasurement.battery || 89}%` }"></div>
                </div>
                <div class="battery-indicator">
                  <span>{{ selectedMeasurement.battery || '89' }}%</span>
                </div>
              </div>
            </div>
          </div>

          <!-- Latitude and Longitude -->
          <div class="grid-item">
            <strong>Latitude: </strong> {{ selectedMeasurement.latitude || 'N/A' }}
          </div>
          <div class="grid-item">
            <strong>Longitude:</strong> {{ selectedMeasurement.longitude || 'N/A' }}
          </div>

          <!-- Temperature and Humidity -->
          <div class="grid-item">
            <strong>Temperature:</strong> -
          </div>
          <div class="grid-item">
            <strong>Humidity:</strong> -
          </div>

          <!-- Add/Remove Geofence button -->
          <div class="grid-item no-border">
            <button v-if="!geofenceActive" @click="addGeofence" class="geofence-button">Add Geofence</button>
            <button v-else @click="removeGeofence" class="remove-geofence-button">Remove Geofence</button>
          </div>
        </div>

        <!-- Slider for geofence radius, shown only when geofence is active -->
        <div v-if="geofenceActive" class="slider-container">
          <label for="radius-slider" class="radius-label">Geofence Radius: {{ geofenceRadius }} meters</label>
          <input id="radius-slider" type="range" min="100" max="5000" step="100" v-model="geofenceRadius"
            class="radius-slider" />
        </div>
      </div>
    </div>

    <!-- Map Card -->
    <div class="card">
      <div class="card-header">
        <div class="timestamp">
          <strong>Position Timestamp:</strong> {{ selectedMeasurement.createdAt ? new
            Date(selectedMeasurement.createdAt).toLocaleString() : 'N/A' }}
        </div>
        <div class="tracker-mode">
          Mode: {{ trackerMode || 'N/A' }}
          <span v-if="trackerMode === 'RT'" class="mode-badge">GPS</span>
          <span v-if="trackerMode === 'LT'" class="mode-badge">LTE</span>
        </div>
      </div>

      <!-- Map Container -->
      <div ref="mapElement" class="map"></div>

      <div class="location-display">
        <p>{{ selectedTrackerLocation }}</p>
        <div class="location-accuracy">
          Estimated Accuracy: 5m
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, computed, onMounted, watch } from 'vue';
import { useShepherd } from 'vue-shepherd'
import axios from 'axios';

const el = ref(null);
const tour = useShepherd({
  useModalOverlay: true
});

const trackers = ref([]);
const selectedTracker = ref(null);
const selectedTimestamp = ref(null);
const selectedMeasurement = ref({});
const selectedTrackerMeasurements = ref([]);
const selectedTrackerLocation = ref('Unknown Location');
const selectedTrackerName = ref('Tracker Information');
const mapElement = ref(null);
let map = null;
let marker = null;
let geofenceCircle = null; // Declare geofence circle
let isGoogleMapsLoaded = ref(false);

// Geofence-related state
const geofenceActive = ref(false); // Geofence initially inactive
const geofenceRadius = ref(2000); // Default radius value in meters (set to 2000)

// Compute the mode of the selected tracker
const trackerMode = computed(() => {
  const tracker = trackers.value.find(t => t._id === selectedTracker.value);
  return tracker ? tracker.mode : null;
});

// Fetch trackers and measurements for the user
const fetchTrackersForUser = async () => {
  const userId = '66ce5ccf63640a4596a9b45b';  // Replace with actual userId
  try {
    const response = await axios.get(`http://localhost:3500/api/tracker/users/${userId}/trackers`);
    trackers.value = response.data;

    for (const tracker of trackers.value) {
      const measurementsResponse = await axios.get(`http://localhost:3500/api/position/tracker/${tracker._id}`);
      tracker.measurements = measurementsResponse.data;

      if (!selectedTracker.value) {
        selectedTracker.value = tracker._id;
        selectedTrackerName.value = tracker.name;
      }
    }

    updateSelectedTrackerMeasurements(true);
  } catch (error) {
    console.error('Failed to fetch trackers or measurements:', error);
  }
};

// Update selected tracker measurements
const updateSelectedTrackerMeasurements = (initialLoad = false) => {
  const tracker = trackers.value.find(t => t._id === selectedTracker.value);
  if (tracker && tracker.measurements.length > 0) {
    selectedTrackerName.value = tracker.name;
    selectedTrackerMeasurements.value = tracker.measurements.sort((a, b) => new Date(b.createdAt) - new Date(a.createdAt));

    const mostRecentMeasurement = selectedTrackerMeasurements.value[0];
    selectedTimestamp.value = mostRecentMeasurement._id;
    updateSelectedMeasurement(initialLoad);
  }
};

// Update selected measurement
const updateSelectedMeasurement = (initialLoad = false) => {
  const tracker = trackers.value.find(t => t._id === selectedTracker.value);
  if (tracker) {
    const measurement = tracker.measurements.find(m => m._id === selectedTimestamp.value);
    if (measurement) {
      selectedMeasurement.value = measurement;
      getReverseGeocodingAddress(measurement.latitude, measurement.longitude);

      if (initialLoad) {
        if (isGoogleMapsLoaded.value) {
          initializeMap();
        } else {
          loadGoogleMapsScript().then(() => {
            initializeMap();
          });
        }
      } else {
        initializeMap();
      }
    }
  }
};

// Initialize the map without the geofence initially
const initializeMap = () => {
  if (!selectedMeasurement.value || !mapElement.value) return;

  const position = {
    lat: selectedMeasurement.value.latitude,
    lng: selectedMeasurement.value.longitude
  };

  if (map) {
    map.setCenter(position);
    if (marker) {
      marker.setPosition(position);
    } else {
      marker = new google.maps.Marker({ position, map, title: 'Tracker Location' });
    }
  } else {
    map = new google.maps.Map(mapElement.value, {
      center: position,
      zoom: 12,
    });
    marker = new google.maps.Marker({
      position,
      map,
      title: 'Tracker Location',
    });
  }
};

// Add geofence when the button is clicked
const addGeofence = () => {
  geofenceActive.value = true;

  const position = {
    lat: selectedMeasurement.value.latitude,
    lng: selectedMeasurement.value.longitude
  };

  // Always recreate the geofence circle when adding it, even after removing it
  if (map) {
    if (geofenceCircle) {
      geofenceCircle.setMap(null); // Remove the previous circle, if any
    }

    // Draw the geofence circle on the map with the current radius
    geofenceCircle = new google.maps.Circle({
      map,
      center: position,
      radius: 2000, // Default to 2000 meters (even before slider change)
      fillColor: '#28a745',
      fillOpacity: 0.35,
      strokeColor: '##28a745',
      strokeOpacity: 0.8,
      strokeWeight: 2,
    });

    // Make sure the slider reflects the default 2000 meters
    geofenceRadius.value = 2000;

    console.log('Geofence added with radius:', geofenceRadius.value);
  }
};

// Remove geofence when the button is clicked
const removeGeofence = () => {
  geofenceActive.value = false;
  if (geofenceCircle) {
    geofenceCircle.setMap(null); // Remove the geofence circle from the map
    geofenceCircle = null;
  }
  console.log('Geofence removed');
};

// Watch geofenceRadius for changes and update the circle's radius dynamically
watch(geofenceRadius, (newRadius) => {
  const radius = parseFloat(newRadius); // Ensure the radius is a number
  console.log('Slider moved, new radius:', radius); // Log slider changes for debugging
  if (geofenceCircle) {
    console.log('Updating geofence radius to:', radius); // Log the radius update process
    geofenceCircle.setRadius(radius); // Update the radius of the geofence circle when the slider changes
  } else {
    console.log('Geofence circle is not initialized yet');
  }
});

// Perform reverse geocoding using your backend API
const getReverseGeocodingAddress = async (lat, lng) => {
  const geocodingUrl = `http://localhost:3500/api/geocode?lat=${lat}&lng=${lng}`;

  try {
    const response = await axios.get(geocodingUrl);
    if (response.data && response.data.address) {
      selectedTrackerLocation.value = response.data.address;
    } else {
      selectedTrackerLocation.value = 'Unknown Location';
    }
  } catch (error) {
    console.error('Failed to perform reverse geocoding:', error);
    selectedTrackerLocation.value = 'Unknown Location';
  }
};

// Load Google Maps API script dynamically
const loadGoogleMapsScript = () => {
  return new Promise((resolve, reject) => {
    if (window.google && window.google.maps) {
      isGoogleMapsLoaded.value = true;
      resolve();
    } else if (!document.querySelector("script[src*='maps.googleapis.com']")) {
      const script = document.createElement('script');
      script.src = `https://maps.googleapis.com/maps/api/js?key=AIzaSyDzh7DicT6KmawobOi6iir27IFEQBsRhRo&libraries=geometry`;
      script.async = true;
      script.defer = true;
      script.onload = () => {
        isGoogleMapsLoaded.value = true;
        resolve();
      };
      script.onerror = reject;
      document.head.appendChild(script);
    }
  });
};

// On component mount, fetch trackers and measurements
onMounted(async () => {
  await fetchTrackersForUser();

});

// Watch for changes in selected tracker and update measurements
watch(selectedTracker, updateSelectedTrackerMeasurements);
</script>

<style scoped>
/* Global box-sizing for better control over padding and width */
*,
*::before,
*::after {
  box-sizing: border-box;
}

/* Prevent horizontal scrolling and ensure body and html take up full width */
html,
body {
  margin: 0;
  padding: 0;
  width: 100%;
  overflow-x: hidden;
}

.mode-badge {
  font-weight: bold;
  color: red;
  margin-left: 8px;
}

.container {
  display: flex;
  flex-direction: row;
  width: 100%;
  padding-left: 10px;
  padding-right: 10px;
  margin: 0;
  gap: 0;
  background-color: #f1e4cc;
}

.card {
  width: 100%;
  height: 600px;
  padding: 20px;
  box-shadow: 0px 4px 8px rgba(0, 0, 0, 0.0);
  flex: 1;
}

.card-header {
  display: flex;
  justify-content: center;
  align-items: center;
  margin-bottom: 20px;
}

.timestamp {
  flex: 1;
  text-align: left;
}

.dropdown-label {
  display: flex;
  flex-direction: column;
  font-size: 1.1rem;
  color: #333;
  width: 100%;
}

.grid-container {
  display: grid;
  grid-template-columns: repeat(2, 1fr);
  gap: 20px;
  margin-top: 20px;
}

.grid-item {
  background-color: #f1e4cc;
  padding: 10px;
  text-align: center;
  border-radius: 8px;
  font-size: 1.2rem;
  font-weight: 500;
  border: 1px solid #00543D;
  color: #011812;
  min-height: auto;
  display: flex;
  align-items: center;
  justify-content: center;
}

/* Ensures full-width for Mode and Battery grid items */
.grid-item-full {
  grid-column: span 2;
  background-color: #f1e4cc;
  padding: 15px;
  text-align: center;
  border-radius: 8px;
  font-size: 1.2rem;
  font-weight: 500;
  color: #011812;
  width: 100%;
  border: 1px solid #00543D;
}

/* Mode Item - relative positioning for Switch Mode button */
.mode-item {
  position: relative;
  padding: 10px;
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.grid-item strong {
  color: #011812;
}

.switch-mode-button {
  position: absolute;
  bottom: 0;
  /* Touches bottom of the grid item */
  right: 0;
  /* Touches right side of the grid item */
  margin: 5px;
  /* Space from the border */
  padding: 4px 8px;
  /* Smaller padding */
  background: linear-gradient(45deg, #7a7a7a, #9a9a9a);
  color: white;
  border: none;
  border-radius: 5px;
  cursor: pointer;
  font-size: 0.8rem;
  /* Smaller font */
  font-weight: bold;
  text-transform: uppercase;
  overflow: hidden;
  position: relative;
  z-index: 1;
  border: 1px solid #000000;
}

.switch-mode-button:before {
  content: '';
  position: absolute;
  top: 0;
  left: -100%;
  width: 200%;
  height: 100%;
  background: linear-gradient(to right,
      rgba(255, 255, 255, 0) 0%,
      rgba(255, 255, 255, 0.5) 50%,
      rgba(255, 255, 255, 0) 100%);
  z-index: -1;
  transition: all 0.5s;
  transform: skewX(-20deg);
}

.switch-mode-button:hover:before {
  left: 100%;
  transition: all 0.5s ease-in-out;
}

.switch-mode-button:focus {
  outline: none;
}

.switch-mode-button:active {
  background: linear-gradient(45deg, #b0b0b0, #d0d0d0);
}

/* Optional shimmer animation for continuous effect */
@keyframes shimmer {
  0% {
    background-position: -200%;
  }

  100% {
    background-position: 200%;
  }
}

.shimmering-button {
  background: linear-gradient(90deg, #6a6a6a 25%, #8a8a8a 50%, #6a6a6a 75%);
  background-size: 400%;
  animation: shimmer 5s infinite;
}

.switch-mode-button:hover {
  background-color: #0056b3;
}

.card-title {
  font-size: 1.5rem;
  font-weight: bold;
  color: #4B2E18;
  text-align: center;
}

.tracker-dropdown-container {
  flex: 1;
  text-align: center;
}

.tracker-info-card {
  margin-top: 20px;
}

.tracker-mode {
  flex: 1;
  text-align: right;
}

.tracker-dropdown {
  padding: 8px;
  border-radius: 4px;
  border: 1px solid #00543D;
  font-size: 16px;
  width: 100%;
}

#timestamp-dropdown {
  padding: 8px;
  border-radius: 4px;
  border: 1px solid #00543D;
  font-size: 16px;
  width: 100%;
}

.map {
  width: 100%;
  height: 470px;
  border-radius: 8px;
  overflow: hidden;
  border: 1px solid #00543D;
}

.location-display {
  display: flex;
  justify-content: space-between;
  /* Align location to center and accuracy to the right */
  align-items: center;

  width: 100%;
  /* Ensure full width */
  color: #011812;
}

.location-display p {
  flex-grow: 1;
  text-align: center;
  font-weight: bold;
}

.location-accuracy {
  font-weight: normal;
  margin-left: auto;
  text-align: right;
  color: #011812;
}

/* Battery Status Wrapper */
.battery-status-wrapper {
  display: inline-flex;
  align-items: center;
  gap: 10px;
  white-space: nowrap;
}

/* Battery Bar Wrapper */
.battery-wrapper {
  display: flex;
  justify-content: center;
  align-items: center;
  position: relative;
}

/* Battery Bar (Phone-like) */
.battery-bar {
  width: 70px;
  height: 20px;
  background-color: #ddd;
  border-radius: 4px;
  position: relative;
  margin-right: 5px;
  display: flex;
  align-items: center;
  overflow: hidden;
  border: 2px solid #555;
}

/* Battery Fill (Green) */
.battery-fill {
  background-color: #28a745;
  height: 100%;
  transition: width 0.3s;
}

/* Battery Indicator Text */
.battery-indicator {
  font-size: 0.8rem;
  font-weight: bold;
  color: #333;
}

/* Remove border specifically for the grid item containing the geofence button */
.no-border {
  border: none;
}

/* Styling for the "Add Geofence" and "Remove Geofence" button */
.geofence-button,
.remove-geofence-button {
  margin-left: auto;
  margin-right: auto;
  margin-bottom: 20px;
  padding: 10px 20px;
  width: 100%;
  background-color: #28a745;
  color: #fff;
  border: none;
  border-radius: 5px;
  cursor: pointer;
  font-size: 1rem;
  border: 1px solid #00543D;
}

.remove-geofence-button {
  background-color: #d9534f;
}

.geofence-button:hover,
.remove-geofence-button:hover {
  background-color: #00402E;
}

/* Styling for the slider */
.radius-slider {
  margin-left: 20px;
  width: 150px;
  cursor: pointer;
  margin-top: 40px;
}

.radius-label {
  font-weight: bold;
  margin-right: 10px;
}

/* Mobile view: Stack cards on top of each other */
@media (max-width: 768px) {
  .container {
    flex-direction: column;
    width: 100%;
  }

  .card {
    max-width: 100%;
    width: 100%;
    padding: 10px;
  }

  .tracker-info-card {
    width: 100%;

  }
}
</style>
