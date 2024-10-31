<template>
  <div :class="['container', (user.template ?? 'default') === 'dark' ? 'dark-mode' : '']">
    <div class="mapview-container">
      <div v-if="trackers.length === 0" class="overlay">
        <p class="overlay-text">No trackers available. Please add a tracker.</p>
      </div>

      <!-- Greyish Overlay when there are no trackers (Top-level) -->
      <!-- Greyish Overlay when there are no trackers -->

      <!-- Tracker Info Card -->
      <div v-if="trackers.length > 0" class="tracker-info-card">
        <div class="card-body">
          <!-- Select Tracker with Filters Button -->
          <div class="tracker-container">
            <label for="tracker-dropdown" class="dropdown-label">
              Select Tracker:
              <select id="tracker-dropdown" class="tracker-dropdown" v-model="selectedTracker" @change="selectTracker">
                <option v-for="tracker in trackers" :key="tracker._id" :value="tracker._id">
                  {{ tracker.name }}
                </option>
              </select>
            </label>
            <button class="filters-button" @click="openTrackerFilters">Filters</button>
          </div>

          <!-- Select Timestamp with Filters Button -->
          <div class="timestamp-container">
            <label for="timestamp-dropdown" class="dropdown-label">
              Select Timestamp:
              <select id="timestamp-dropdown" class="tracker-dropdown" v-model="selectedTimestamp"
                @change="updateSelectedMeasurement">
                <option v-for="measurement in selectedTrackerMeasurements" :key="measurement._id"
                  :value="measurement._id">
                  {{ new Date(measurement.createdAt).toLocaleString() }}
                </option>
              </select>
            </label>
            <button class="filters-button" @click="openFilters">Filters</button>
          </div>

          <div class="grid-container">
            <!-- Mode Information -->
            <div class="grid-item-full mode-item">
              <strong>Mode:</strong>
              <span v-if="trackerMode === 'RT'">Real-Time Tracking</span>
              <span v-else>{{ trackerMode || 'N/A' }}</span>

              <!-- Mode Badges for GPS and LTE -->
              <span v-if="trackerMode === 'GPS'" class="mode-badge">GPS</span>
              <span v-if="trackerMode === 'LT'" class="mode-badge">LTE</span>

              <!-- Switch Mode Button -->
              <button class="switch-mode-button shimmering-button" @click="switchMode">Change Mode</button>
            </div>

            <!-- Battery Status -->
            <div class="grid-item-full">
              <div class="battery-status-wrapper">
                <strong>Battery Status:</strong>
                <div class="battery-wrapper">
                  <div class="battery-bar">
                    <div class="battery-fill"
                      :style="{ width: `${Math.round(selectedMeasurement.battery) || N / A}%` }">
                    </div>
                  </div>
                  <div class="battery-indicator">
                    <span>{{ Math.round(selectedMeasurement.battery) || 'N/A' }}%</span>
                  </div>
                </div>
              </div>
            </div>

            <!-- Latitude and Longitude -->
            <div class="grid-item">
              <strong>Latitude:&nbsp; </strong> {{ selectedMeasurement.latitude || 'N/A' }}
            </div>
            <div class="grid-item">
              <strong>Longitude:&nbsp;</strong> {{ selectedMeasurement.longitude || 'N/A' }}
            </div>

            <!-- Temperature and Humidity -->
            <div class="grid-item">
              <strong>Temperature:&nbsp;</strong> {{ selectedMeasurement.temperature || 'N/A' }} °C
            </div>
            <div class="grid-item">
              <strong>Humidity:&nbsp;</strong> {{ selectedMeasurement.humidity || 'N/A' }} %
            </div>

            <!-- Add/Remove Geofence button -->
            <div class="grid-item-full no-background">
              <button v-if="!geofenceActive" @click="addGeofence" class="geofence-button">Add Geofence</button>
              <button v-else @click="removeGeofence" class="remove-geofence-button">Remove Geofence</button>
              <button @click="addMotionSensor" class="geofence-button">Add Motion Sensor</button>
            </div>

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
    <div v-if="trackers.length > 0" class="card">
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
      <div class="map-container">
        <div ref="mapElement" class="map"></div>

        <!-- Grey Overlay for dark mode -->
        <div v-if="(user.template ?? 'default') === 'dark'" class="map-overlay"></div>
      </div>




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
import './styles_currentmap.css';
import { ref, computed, onMounted, watch } from 'vue';
import { useShepherd } from 'vue-shepherd'
import axios from 'axios';

import { useAuthStore } from "@/stores/auth";
const authStore = useAuthStore();
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
  // Access the auth store

  try {
    // Retrieve the access token from the auth store
    const token = authStore.accessToken;

    // Set up the configuration for Axios to include the Authorization header
    const config = {
      headers: {
        Authorization: `Bearer ${token}`
      }
    };

    // Fetch trackers for the authenticated user
    const response = await axios.get('http://localhost:3500/api/tracker/user/', config);
    trackers.value = response.data;

    // Fetch measurements for each tracker
    for (const tracker of trackers.value) {
      const measurementsResponse = await axios.get(`http://localhost:3500/api/position/tracker/${tracker._id}`, config);
      tracker.measurements = measurementsResponse.data;

      // Set the first tracker as the selected tracker, if not already set
      if (!selectedTracker.value) {
        selectedTracker.value = tracker._id;
        selectedTrackerName.value = tracker.name;
      }
    }

    // Update the selected tracker's measurements
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
    lat: Number(selectedMeasurement.value.latitude),
    lng: Number(selectedMeasurement.value.longitude)
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

      streetViewControl: false
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
    lat: Number(selectedMeasurement.value.latitude),
    lng: Number(selectedMeasurement.value.longitude)
  };

  console.log('Adding geofence at position:', position); // Log the geofence position for debugging

  // Always recreate the geofence circle when adding it, even after removing it
  if (map) {
    if (geofenceCircle) {
      geofenceCircle.setMap(null); // Remove the previous circle, if any
    }

    // Draw the geofence circle on the map with the current radius
    geofenceCircle = new google.maps.Circle({
      map,
      center: position,
      radius: geofenceRadius.value, // Ensure radius is set correctly
      fillColor: '#28a745',
      fillOpacity: 0.35,
      strokeColor: '#28a745',
      strokeOpacity: 0.8,
      strokeWeight: 2,
    });

    console.log('Geofence circle added with radius:', geofenceRadius.value); // Log circle radius
  } else {
    console.error('Map object is not available when adding geofence');
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
const user = computed(() => authStore.userDetail);
// On component mount, fetch trackers and measurements
onMounted(async () => {
  await authStore.getUser();
  await fetchTrackersForUser();

});

// Watch for changes in selected tracker and update measurements
watch(selectedTracker, updateSelectedTrackerMeasurements);
</script>

<style scoped></style>
