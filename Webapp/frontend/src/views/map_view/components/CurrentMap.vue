<template>
  <div :class="['container', (user.settings?.template ?? 'default') === 'dark' ? 'dark-mode' : '']">
    <div class="mapview-container">
      <div v-if="trackers.length === 0" class="overlay">
        <p class="overlay-text">{{ $t('CurrentMap-no_trackers') }}</p>
      </div>

      <!-- Greyish Overlay when there are no trackers (Top-level) -->
      <!-- Greyish Overlay when there are no trackers -->

      <!-- Tracker Info Card -->
      <div v-if="trackers.length > 0" class="tracker-info-card">
        <div class="card-body">
          <!-- Select Tracker with Filters Button -->
          <div class="tracker-container">
            <label for="tracker-dropdown" class="dropdown-label">
              {{ $t('CurrentMap-select_tracker') }}:
              <select id="tracker-dropdown" class="tracker-dropdown" v-model="selectedTracker" @change="selectTracker">
                <option v-for="tracker in filteredTrackers" :key="tracker._id" :value="tracker._id">
                  {{ tracker.name }}
                </option>
              </select>
            </label>
            <button class="filters-button" @click="openTrackerFilters">Filters</button>
          </div>

          <!-- Select Timestamp with Filters Button -->
          <div class="timestamp-container">
            <label for="timestamp-dropdown" class="dropdown-label">
              {{ $t('CurrentMap-select_timestamp') }}:
              <select id="timestamp-dropdown" class="tracker-dropdown" v-model="selectedTimestamp"
                @change="updateSelectedMeasurement">
                <option v-for="(measurement, index) in filteredMeasurements" :key="measurement._id"
                  :value="measurement._id" :class="{ 'newest-timestamp': index === 0 }">
                  {{ new Date(measurement.createdAt).toLocaleString() }}
                </option>
              </select>
            </label>
            <button class="filters-button" @click="openTimestampFilters">Filters</button>
          </div>

          <div class="grid-container">
            <!-- Mode Information -->
            <div class="grid-item-full mode-item">
              <strong>{{ $t('TrackerCard-mode') }}:</strong> {{ trackerModeLabel }}


              <!-- Switch Mode Button -->
              <button class="switch-mode-button shimmering-button" @click="openChangeModePopup">{{
                $t('CurrentMap-change_mode') }}</button>
              <ChangeModePopup v-if="isChangeModePopupOpen" :template="user.settings?.template"
                :selectedTrackerId="selectedTracker" :closePopup="closeChangeModePopup" />
            </div>

            <!-- Battery Status -->
            <div v-if="selectedMeasurement.battery !== undefined" class="grid-item-full">
              <div class="battery-status-wrapper">
                <strong>Battery Status:</strong>
                <div class="battery-wrapper">
                  <div class="battery-bar">
                    <div class="battery-fill" :style="{ width: `${Math.round(selectedMeasurement.battery)}%` }"></div>
                  </div>
                  <div class="battery-indicator">
                    <span>{{ Math.round(selectedMeasurement.battery) }}%</span>
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

            <!-- Temperature -->
            <div v-if="selectedMeasurement.temperature !== undefined" class="grid-item">
              <strong>{{ $t('TrackerList-temperature') }}:&nbsp;</strong> {{ selectedMeasurement.temperature }} °C
            </div>
            <!-- Humidity -->
            <div v-if="selectedMeasurement.humidity !== undefined" class="grid-item">
              <strong>{{ $t('TrackerList-humidity') }}:&nbsp;</strong> {{ selectedMeasurement.humidity }} %
            </div>

            <!-- Add/Remove Geofence button -->
            <!-- Add/Remove Geofence button -->
            <div class="grid-item-full no-background button-row">
              <button v-if="!geofenceActive" @click="addGeofence" class="geofence-button">{{
                $t('CurrentMap-add_geofence') }}</button>
              <button v-else @click="removeGeofence" class="remove-geofence-button">{{
                $t('CurrentMap-remove_geofence') }}</button>
              <!-- <button @click="addMotionSensor" class="geofence-button">{{ $t('CurrentMap-add_motion_sensor') }}</button> -->
            </div>

          </div>
        </div>

        <!-- Slider for geofence radius, shown only when geofence is active -->
        <div v-if="showGeofenceSlider" class="slider-container">
          <label for="radius-slider" class="radius-label">Geofence Radius: {{ geofenceRadius }} meters</label>
          <input id="radius-slider" type="range" min="100" max="5000" step="100" v-model="geofenceRadius"
            class="radius-slider" />
          <button @click="applyGeofenceRadius" class="apply-radius-button">{{ $t('CurrentMap-apply') }}</button>
        </div>
      </div>
    </div>

    <!-- Map Card -->
    <div v-if="trackers.length > 0" class="card">
      <div class="card-header">
        <div class="timestamp">
          <strong>Position Timestamp: </strong>
          <span :class="[(user.settings?.template ?? 'default') === 'dark' ? 'date-dark' : 'date-normal']">
            {{ selectedMeasurement.createdAt ? new Date(selectedMeasurement.createdAt).toLocaleString() : 'N/A' }}
          </span>
        </div>
        <div class="location">
          <p class="location-text">{{ selectedTrackerLocation }}</p>
        </div>
        <div class="tracker-mode">
          {{ $t('TrackerCard-mode') }}: {{ trackerModeLabel }}

        </div>
      </div>

      <!-- Map Container -->
      <div class="map-container">
        <div ref="mapElement" class="map"></div>

        <!-- Grey Overlay for dark mode -->
        <div v-if="(user.settings?.template ?? 'default') === 'dark'" class="map-overlay"></div>
      </div>
      <div class="legend">
        <span :style="{ color: modeColors.green }" class="legend-item">
          <i class="fas fa-map-pin"></i> {{ $t('CurrentMap-green') }}: {{ modeAccuracy.green }}&nbsp;&nbsp;
        </span>
        <span :style="{ color: modeColors.yellow }" class="legend-item">
          <i class="fas fa-map-pin"></i> {{ $t('CurrentMap-yellow') }}: {{ modeAccuracy.yellow }}&nbsp;&nbsp;
        </span>
        <span :style="{ color: modeColors.red }" class="legend-item">
          <i class="fas fa-map-pin"></i> {{ $t('CurrentMap-red') }}: {{ modeAccuracy.red }}&nbsp;&nbsp;
        </span>
        <span :style="{ color: (user.settings?.template ?? 'default') === 'dark' ? '#E69543' : '#851515' }"
          class="legend-item">
          {{ $t('CurrentMap-current_accuracy') }}:
          <strong>
            {{
              selectedMeasurement.accuracy
                ? Math.round(selectedMeasurement.accuracy * 10) / 10 + 'm'
                : 'N/A'
            }}
          </strong>
        </span>
      </div>









    </div>

    <!-- Conditionally render the Tracker Filter Popup -->
    <TrackerFilterPopup v-if="isTrackerFilterPopupOpen" :template="user.settings?.template"
      :filters="user.settings.trackerFilters" :closePopup="closeTrackerFilters" :applyFilters="applyTrackerFilters" />

    <!-- Conditionally render the Timestamp Filter Popup -->
    <TimestampFilterPopup v-if="isTimestampFilterPopupOpen" :template="user.settings?.template"
      :filters="user.settings.timestampFilters" :closePopup="closeTimestampFilters"
      :applyFilters="applyTimestampFilters" />




  </div>
</template>

<script setup>
import './styles_currentmap.css';
import { ref, computed, onMounted, watch } from 'vue';
import { useShepherd } from 'vue-shepherd'
import axios from 'axios';
import TrackerFilterPopup from './TrackerFilterPopup.vue';
import TimestampFilterPopup from './TimestampFilterPopup.vue';
import { useAuthStore } from "@/stores/auth";
import ChangeModePopup from './ChangeModePopup.vue';

const isTrackerFilterPopupOpen = ref(false);
const isTimestampFilterPopupOpen = ref(false);

const isChangeModePopupOpen = ref(false);
const openChangeModePopup = () => {
  isChangeModePopupOpen.value = true;
};

const closeChangeModePopup = () => {
  isChangeModePopupOpen.value = false;
};
// Method to open the Tracker Filter popup
const openTrackerFilters = () => {
  isTrackerFilterPopupOpen.value = true;
};

// Method to close the Tracker Filter popup
const closeTrackerFilters = () => {
  isTrackerFilterPopupOpen.value = false;
};

// Method to open the Timestamp Filter popup
const openTimestampFilters = () => {
  isTimestampFilterPopupOpen.value = true;
};

// Method to close the Timestamp Filter popup
const closeTimestampFilters = () => {
  isTimestampFilterPopupOpen.value = false;
};

// Method to apply Tracker filters
const applyTrackerFilters = (filters) => {

  // Handle the filtering logic here
};

// Method to apply Timestamp filters
const applyTimestampFilters = (filters) => {
  // Update the timestamp filters in the parent component
  user.value.settings.timestampFilters = filters;
};


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

const modeAccuracy = computed(() => {
  if (selectedMeasurement.value?.mode === "GPS") {
    return {
      green: "0-25m",
      yellow: "26-50m",
      red: ">50m",
    };
  } else if (selectedMeasurement.value?.mode === "LTE") {
    return {
      green: "0-100m",
      yellow: "101-500m",
      red: ">500m",
    };
  }
  return {
    green: "N/A",
    yellow: "N/A",
    red: "N/A",
  };
});

const modeColors = computed(() => ({
  green: "#228B22", // Green
  yellow: "#FFD700", // Yellow
  red: "#FF0000", // Red
}));


const trackerModeLabel = computed(() => {
  if (selectedMeasurement.value && selectedMeasurement.value.mode) {
    return selectedMeasurement.value.mode === "GPS" ? "Real-Time Tracking" : "Long-Time Tracking";
  }
  return "N/A"; // Default if no measurement or mode is set
});


// Geofence-related state
const geofenceActive = ref(false); // Geofence initially inactive
const showGeofenceSlider = ref(false);
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

      const modeResponse = await axios.get(`http://localhost:3500/api/mode/${tracker._id}`, config);
      tracker.modeDetails = modeResponse.data; // Store the mode details

      const measurementsResponse = await axios.get(
        `http://localhost:3500/api/position/tracker/${tracker._id}`,
        config
      );
      // Filter measurements to include only LTE or GPS modes
      tracker.measurements = measurementsResponse.data.filter(
        (measurement) => measurement.mode === 'LTE' || measurement.mode === 'GPS'
      );


      const geofenceResponse = await axios.get(`http://localhost:3500/api/geofence/${tracker.geofence}`, config);
      tracker.geofenceDetails = geofenceResponse.data; // Store geofence details
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

const applyGeofenceRadius = async () => {
  if (!selectedMeasurement.value) {
    console.error('No selected measurement to base the geofence on.');
    return;
  }

  const tracker = trackers.value.find(t => t._id === selectedTracker.value);
  const geofenceId = tracker?.geofence;

  if (!geofenceId) {
    console.error('No geofence ID found for the selected tracker.');
    return;
  }

  const data = {
    active: true,
    radius: geofenceRadius.value,
    latitude: selectedMeasurement.value.latitude,
    longitude: selectedMeasurement.value.longitude
  };

  const token = authStore.accessToken;
  const config = { headers: { Authorization: `Bearer ${token}` } };

  try {
    await axios.put(`http://localhost:3500/api/geofence/${geofenceId}`, data, config);


    // Hide slider and "Apply" button but keep geofence active
    showGeofenceSlider.value = false;
    geofenceActive.value = true;
  } catch (error) {
    console.error('Failed to save geofence settings to server:', error);
  }
};


const updateGeofenceState = () => {
  const tracker = trackers.value.find(t => t._id === selectedTracker.value);
  if (tracker && tracker.geofenceDetails) {
    // Set geofence active state and radius from database values
    geofenceActive.value = tracker.geofenceDetails.active;
    geofenceRadius.value = tracker.geofenceDetails.radius;

    // Draw the geofence circle if it's active
    if (tracker.geofenceDetails.active) {
      const latitude = Number(tracker.geofenceDetails.latitude);
      const longitude = Number(tracker.geofenceDetails.longitude);
      const radius = Number(tracker.geofenceDetails.radius);
      drawGeofenceCircle(latitude, longitude, radius);
    }
  } else {
    // Default values if no active geofence
    geofenceActive.value = false;
    geofenceRadius.value = 2000; // Default radius
  }
};

const updateSelectedTrackerMeasurements = (initialLoad = false) => {
  const tracker = trackers.value.find(t => t._id === selectedTracker.value);
  if (tracker && tracker.measurements.length > 0) {
    selectedTrackerName.value = tracker.name;

    // Set `selectedTrackerMeasurements` first
    selectedTrackerMeasurements.value = tracker.measurements.sort(
      (a, b) => new Date(b.createdAt) - new Date(a.createdAt)
    );

    // Use `filteredMeasurements` to find the most recent filtered measurement
    const mostRecentMeasurement = filteredMeasurements.value[0];
    if (mostRecentMeasurement) {
      selectedTimestamp.value = mostRecentMeasurement._id;
      updateSelectedMeasurement(initialLoad);
    } else {
      selectedTimestamp.value = null;
      selectedMeasurement.value = {}; // Reset if no valid measurement
    }
  } else {
    // Handle case where no measurements exist
    selectedTrackerMeasurements.value = [];
    selectedTimestamp.value = null;
    selectedMeasurement.value = {};
  }
};


// Update selected measurement
const updateSelectedMeasurement = (initialLoad = false) => {
  const tracker = trackers.value.find(t => t._id === selectedTracker.value);
  if (tracker) {
    const measurement = tracker.measurements.find(m => m._id === selectedTimestamp.value);
    if (measurement) {
      selectedMeasurement.value = measurement;
      getReverseGeocodingAddress(measurement.latitude, measurement.longitude, measurement.accuracy);

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

let accuracyCircle = null; // Declare a global variable for the accuracy circle

const initializeMap = () => {
  if (!selectedMeasurement.value || !mapElement.value) return;

  const position = {
    lat: Number(selectedMeasurement.value.latitude),
    lng: Number(selectedMeasurement.value.longitude),
  };
  const accuracy = Number(selectedMeasurement.value.accuracy);

  // Determine the pin color based on accuracy and mode
  let backgroundColor;
  if (selectedMeasurement.value.mode === "GPS") {
    backgroundColor = accuracy <= 25
      ? modeColors.value.green
      : accuracy <= 50
        ? modeColors.value.yellow
        : modeColors.value.red;
  } else if (selectedMeasurement.value.mode === "LTE") {
    backgroundColor = accuracy <= 100
      ? modeColors.value.green
      : accuracy <= 500
        ? modeColors.value.yellow
        : modeColors.value.red;
  } else {
    backgroundColor = "#000000"; // Default for unknown mode
  }

  const customPin = new google.maps.marker.PinElement({
    background: backgroundColor,
    borderColor: "#000000",
    glyph: "●",
    glyphColor: "#ddd",
  });

  if (map) {
    map.setCenter(position);

    if (marker) {
      marker.position = position;
      marker.content = customPin.element;
    } else {
      marker = new google.maps.marker.AdvancedMarkerElement({
        position,
        map,
        content: customPin.element,
        title: "Tracker Location",
      });
    }

    // Create or update the accuracy circle
    if (accuracyCircle) {
      accuracyCircle.setCenter(position);
      accuracyCircle.setRadius(accuracy); // Update radius based on accuracy
    } else {
      accuracyCircle = new google.maps.Circle({
        map,
        center: position,
        radius: accuracy, // Set radius based on accuracy
        fillColor: "#0000FF", // Blue color for accuracy circle
        fillOpacity: 0.2,
        strokeColor: "#0000FF",
        strokeOpacity: 0.5,
        strokeWeight: 1,
      });
    }
  } else {
    // Initialize the map if not already created
    map = new google.maps.Map(mapElement.value, {
      center: position,
      zoom: 12,
      streetViewControl: false,
      mapId: "6c54d0c5731b3526",
    });

    marker = new google.maps.marker.AdvancedMarkerElement({
      position,
      map,
      content: customPin.element,
      title: "Tracker Location",
    });

    // Create the accuracy circle
    accuracyCircle = new google.maps.Circle({
      map,
      center: position,
      radius: accuracy,
      fillColor: "#0000FF", // Blue color for accuracy circle
      fillOpacity: 0.2,
      strokeColor: "#0000FF",
      strokeOpacity: 0.5,
      strokeWeight: 1,
    });
  }
};


const drawGeofenceCircle = (latitude, longitude, radius) => {
  if (!isGoogleMapsLoaded.value) {
    console.warn("Google Maps API is not loaded yet.");
    return;
  }

  if (geofenceCircle) {
    geofenceCircle.setMap(null); // Remove the existing circle, if any
  }

  geofenceCircle = new google.maps.Circle({
    map,
    center: { lat: Number(latitude), lng: Number(longitude) },
    radius: Number(radius),
    fillColor: '#28a745',
    fillOpacity: 0.35,
    strokeColor: '#28a745',
    strokeOpacity: 0.8,
    strokeWeight: 2,
  });

};

// Add geofence when the button is clicked
const addGeofence = () => {
  geofenceActive.value = true;
  showGeofenceSlider.value = true;
  const position = {
    lat: parseFloat(selectedMeasurement.value.latitude),
    lng: parseFloat(selectedMeasurement.value.longitude)
  };
  geofenceRadius.value = 2000; // Default radius
  drawGeofenceCircle(position.lat, position.lng, geofenceRadius.value);
};

// Remove geofence when the button is clicked
const removeGeofence = async () => {
  const tracker = trackers.value.find(t => t._id === selectedTracker.value);
  const geofenceId = tracker?.geofence;

  if (!geofenceId) {
    console.error('No geofence ID found for the selected tracker.');
    return;
  }

  const data = { active: false, radius: null, latitude: null, longitude: null };

  const token = authStore.accessToken;
  const config = { headers: { Authorization: `Bearer ${token}` } };

  try {
    await axios.put(`http://localhost:3500/api/geofence/${geofenceId}`, data, config);


    geofenceActive.value = false;
    showGeofenceSlider.value = false; // Hide slider and apply button
    if (geofenceCircle) {
      geofenceCircle.setMap(null); // Remove the geofence circle from the map
      geofenceCircle = null;
    }
  } catch (error) {
    console.error('Failed to remove geofence:', error);
  }
};

watch(selectedTracker, updateGeofenceState);
watch(geofenceRadius, (newRadius) => {
  if (geofenceCircle) {
    geofenceCircle.setRadius(parseFloat(newRadius));

  }
});


const getReverseGeocodingAddress = async (lat, lng, accuracy) => {
  const geocodingUrl = `http://localhost:3500/api/geocode?lat=${lat}&lng=${lng}`;

  try {
    const response = await axios.get(geocodingUrl);
    console.log("Geocoding API response:", response.data); // Log the response for debugging

    if (response.data?.address) {
      const fullAddress = response.data.address;

      // Extract zip and city from the address
      const addressParts = fullAddress.split(','); // Split the address into parts
      const lastPart = addressParts[addressParts.length - 1]?.trim(); // e.g., 'Germany'
      const secondLastPart = addressParts[addressParts.length - 2]?.trim(); // e.g., '44793 Bochum'
      const zipAndCity = secondLastPart?.match(/(\d{5})\s(.+)/); // Regex to match 'zip city'

      const zip = zipAndCity ? zipAndCity[1] : "Unknown Zip";
      const city = zipAndCity ? zipAndCity[2] : "Unknown City";

      if (accuracy <= 25) {
        // Full address for high accuracy
        selectedTrackerLocation.value = fullAddress;
      } else {
        // Only zip and city for lower accuracy
        selectedTrackerLocation.value = `${zip}, ${city}`;
      }
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
      script.src = `https://maps.googleapis.com/maps/api/js?key=AIzaSyDzh7DicT6KmawobOi6iir27IFEQBsRhRo&libraries=marker,geometry`;

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
  await loadGoogleMapsScript();  // Wait for Google Maps to load
  await fetchTrackersForUser();
  updateGeofenceState(); // Now safe to call since Google Maps is loaded
});

// Watch for changes in selected tracker and update measurements
watch(selectedTracker, updateSelectedTrackerMeasurements);

const filteredTrackers = computed(() => {
  // Filter based on user's settings for tracker mode
  const modeFilter = user.value.settings.trackerFilters?.mode || [];
  return trackers.value.filter(tracker => {
    // If no mode filter is set, include all trackers
    if (modeFilter.length === 0) return true;

    // Filter by mode if a specific mode is set
    return modeFilter.includes(tracker.mode);
  });
});
const filteredMeasurements = computed(() => {
  const validPositionFilter = user.value.settings.timestampFilters?.validPosition ?? false;
  const modeFilters = user.value.settings.timestampFilters?.mode || [];

  return selectedTrackerMeasurements.value.filter((measurement) => {
    // Filter by valid position
    if (validPositionFilter && (isNaN(measurement.latitude) || isNaN(measurement.longitude))) {
      return false;
    }

    // Filter by mode
    const measurementMode = measurement.mode === 'LTE' ? 'LT' : measurement.mode === 'GPS' ? 'RT' : null;
    return modeFilters.length === 0 || modeFilters.includes(measurementMode);
  });
});



</script>



<style scoped>
.newest-timestamp {
  background-color: #d4edda;
  /* Light green background */
  color: #155724;
  /* Dark green text color for better contrast */
}
</style>
