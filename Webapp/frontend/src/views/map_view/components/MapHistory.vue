<template>
    <div :class="['container', (user.settings?.template ?? 'default') === 'dark' ? 'dark-mode' : '']">
        <div class="mapview-container">
            <!-- Tracker and Timestamp Range Selection Card -->
            <div v-if="trackers.length > 0" class="tracker-info-card">
                <div class="card-body">
                    <!-- Select Tracker Dropdown -->
                    <label for="tracker-dropdown" class="dropdown-label">
                        Select Tracker:
                        <select id="tracker-dropdown" class="tracker-dropdown" v-model="selectedTracker"
                            @change="updateSelectedTrackerMeasurements">
                            <option v-for="tracker in trackers" :key="tracker._id" :value="tracker._id">
                                {{ tracker.name }}
                            </option>
                        </select>
                    </label>

                    <!-- From Timestamp Dropdown with error highlighting -->
                    <label for="from-timestamp-dropdown" class="dropdown-label">
                        From Timestamp:
                        <select id="from-timestamp-dropdown"
                            :class="['tracker-dropdown', !fromTimestamp && errorMessage ? 'error-dropdown' : '']"
                            v-model="fromTimestamp" @change="updateTimestampRange">
                            <option v-for="measurement in selectedTrackerMeasurements" :key="measurement._id"
                                :value="measurement.createdAt">
                                {{ new Date(measurement.createdAt).toLocaleString() }}
                            </option>
                        </select>
                    </label>

                    <!-- To Timestamp Dropdown with error highlighting -->
                    <label for="to-timestamp-dropdown" class="dropdown-label">
                        To Timestamp:
                        <select id="to-timestamp-dropdown"
                            :class="['tracker-dropdown', !toTimestamp && errorMessage ? 'error-dropdown' : '']"
                            v-model="toTimestamp" @change="updateTimestampRange">
                            <option v-for="measurement in selectedTrackerMeasurements" :key="measurement._id"
                                :value="measurement.createdAt">
                                {{ new Date(measurement.createdAt).toLocaleString() }}
                            </option>
                        </select>
                    </label>

                    <!-- Error message display -->
                    <p v-if="errorMessage" class="error-message">{{ errorMessage }}</p>

                    <!-- Use Pin for Every Measurement Checkbox -->
                    <label class="checkbox-label">
                        <input type="checkbox" v-model="usePinForEveryMeasurement" />
                        Use Pin for every Measurement
                    </label>

                    <!-- Build History Button -->
                    <button class="build-history-button" @click="buildHistory">Build History</button>
                </div>
            </div>
        </div>

        <!-- Map Card -->
        <div v-if="trackers.length > 0" class="card">
            <div class="card-header">
                <p class="map-title">{{ mapHistoryTitle }}</p>
            </div>

            <!-- Map Container -->
            <div class="map-container">
                <div ref="mapElement" class="map"></div>
                <div v-if="showViewDetailsButton" class="center-button-container">
                    <button class="view-details-button" @click="viewDetails">View Details</button>
                </div>
                <!-- Grey Overlay for dark mode -->
                <div v-if="(user.settings?.template ?? 'default') === 'dark'" class="map-overlay"></div>
            </div>
        </div>

    </div>
</template>

<script setup>

import { ref, computed, onMounted, watch } from 'vue';
import axios from 'axios';
import { useAuthStore } from "@/stores/auth";

const authStore = useAuthStore();
const user = computed(() => authStore.userDetail);

const trackers = ref([]);
const selectedTracker = ref(null);
const fromTimestamp = ref(null);
const toTimestamp = ref(null);
const selectedTrackerMeasurements = ref([]);
const usePinForEveryMeasurement = ref(false);
const errorMessage = ref(''); // New ref for error message
const mapElement = ref(null);
const showViewDetailsButton = ref(false);
let map = null;
let markers = [];
let path = null;
let circles = [];
const viewDetails = () => {
    console.log("View Details button clicked");
    // Add your logic here for viewing details, if needed.
};

// Computed property for the map history title
const mapHistoryTitle = computed(() => {
    if (fromTimestamp.value && toTimestamp.value) {
        return `Map history from ${new Date(fromTimestamp.value).toLocaleString()} to ${new Date(toTimestamp.value).toLocaleString()}`;
    } else if (fromTimestamp.value) {
        return `Map history from ${new Date(fromTimestamp.value).toLocaleString()}`;
    } else {
        return "Map history";
    }
});

// Fetch trackers and their measurements
const fetchTrackersForUser = async () => {
    try {
        const token = authStore.accessToken;
        const config = { headers: { Authorization: `Bearer ${token}` } };

        const response = await axios.get('http://localhost:3500/api/tracker/user/', config);
        trackers.value = response.data;

        for (const tracker of trackers.value) {
            const measurementsResponse = await axios.get(`http://localhost:3500/api/position/tracker/${tracker._id}`, config);
            tracker.measurements = measurementsResponse.data;

            if (!selectedTracker.value) {
                selectedTracker.value = tracker._id;
            }
        }

        updateSelectedTrackerMeasurements();
    } catch (error) {
        console.error('Failed to fetch trackers or measurements:', error);
    }
};

// Update measurements for selected tracker and filter out invalid coordinates
const updateSelectedTrackerMeasurements = () => {
    const tracker = trackers.value.find(t => t._id === selectedTracker.value);
    if (tracker) {
        selectedTrackerMeasurements.value = tracker.measurements
            .filter(m => {
                const latitude = parseFloat(m.latitude);
                const longitude = parseFloat(m.longitude);
                return !isNaN(latitude) && !isNaN(longitude);
            })
            .map(m => ({
                ...m,
                latitude: parseFloat(m.latitude),
                longitude: parseFloat(m.longitude),
            }))
            .sort((a, b) => new Date(a.createdAt) - new Date(b.createdAt));
    }
};

// Function to draw numbered pins for each measurement
const drawPinsWithGroupedInfoWindow = (measurements) => {
    markers.forEach(marker => marker.setMap(null)); // Clear previous markers
    markers = []; // Reset markers array

    // Step 1: Group measurements by location (latitude and longitude)
    const locationGroups = new Map();

    measurements.forEach((m, index) => {
        const key = `${m.latitude},${m.longitude}`;
        if (!locationGroups.has(key)) {
            locationGroups.set(key, []);
        }
        locationGroups.get(key).push({ ...m, index: index + 1 });
    });

    // Step 2: Create markers and info windows for each unique location
    locationGroups.forEach((groupedMeasurements, key) => {
        const { latitude, longitude } = groupedMeasurements[0];

        // Create a single marker for the location
        const marker = new google.maps.Marker({
            position: { lat: latitude, lng: longitude },
            map,
            label: `${groupedMeasurements[0].index}`, // Show the first index as label
        });

        // Create combined content for the info window
        const infoContent = groupedMeasurements
            .map(m => `Measurement ${m.index}: ${new Date(m.createdAt).toLocaleString()}`)
            .join("<br>");

        const infoWindow = new google.maps.InfoWindow({
            content: infoContent
        });

        // Show info window on hover
        marker.addListener("mouseover", () => {
            infoWindow.open(map, marker);
        });
        marker.addListener("mouseout", () => {
            infoWindow.close();
        });

        markers.push(marker);
    });
};


// Function to draw circles and specific pins based on checkbox state
const drawCirclesAndPins = (measurements) => {
    markers.forEach(marker => marker.setMap(null)); // Clear previous markers
    circles.forEach(circle => circle.setMap(null)); // Clear previous circles
    markers = [];
    circles = [];

    measurements.forEach((m, index) => {
        if (index === 0 || index === measurements.length - 1) {
            // Draw numbered pins for the first and last measurements
            const marker = new google.maps.Marker({
                position: { lat: m.latitude, lng: m.longitude },
                map,
                label: `${index + 1}`,
                title: `Point ${index + 1}`
            });
            markers.push(marker);
        } else {
            // Draw circles for intermediate measurements
            const circle = new google.maps.Circle({
                strokeColor: "#000000",
                strokeOpacity: 1.0,
                strokeWeight: 0,
                fillColor: "#000000",
                fillOpacity: 1.0,
                map,
                center: { lat: m.latitude, lng: m.longitude },
                radius: 5
            });
            circles.push(circle);
        }
    });
};

// Build history action with validation and error handling
const buildHistory = () => {
    const tracker = trackers.value.find(t => t._id === selectedTracker.value);

    // Validate that both timestamps are selected
    if (!fromTimestamp.value || !toTimestamp.value) {
        errorMessage.value = "You have to select a start- and endpoint first";
        return;
    }

    // Validate that the start date is not later than the end date
    if (new Date(fromTimestamp.value) > new Date(toTimestamp.value)) {
        errorMessage.value = "The starting point of the history cannot be later than the endpoint";
        return;
    }

    errorMessage.value = ''; // Clear error if all validations pass

    const measurements = tracker.measurements
        .filter(m =>
            new Date(m.createdAt) >= new Date(fromTimestamp.value) &&
            new Date(m.createdAt) <= new Date(toTimestamp.value)
        )
        .map(m => ({
            ...m,
            latitude: parseFloat(m.latitude),
            longitude: parseFloat(m.longitude),
        }))
        .filter(m => !isNaN(m.latitude) && !isNaN(m.longitude)) // Filter out NaN values
        .sort((a, b) => new Date(a.createdAt) - new Date(b.createdAt));

    if (measurements.length < 2) {
        console.warn("Not enough valid data points to build history.");
        return;
    }

    markers.forEach(marker => marker.setMap(null));
    circles.forEach(circle => circle.setMap(null));
    if (path) path.setMap(null);

    // Create an array of path coordinates for the Polyline
    const pathCoordinates = measurements.map(m => ({ lat: m.latitude, lng: m.longitude }));

    // Debug: log path coordinates to check for gaps or unexpected values
    console.log("Filtered Path coordinates:", pathCoordinates);

    path = new google.maps.Polyline({
        path: pathCoordinates,
        geodesic: true,
        strokeColor: "#FF0000",
        strokeOpacity: 1.0,
        strokeWeight: 2,
    });
    path.setMap(map);
    showViewDetailsButton.value = true;

    if (usePinForEveryMeasurement.value) {
        drawPinsWithGroupedInfoWindow(measurements);
    } else {
        drawCirclesAndPins(measurements);
    }

    const bounds = new google.maps.LatLngBounds();
    pathCoordinates.forEach(coord => bounds.extend(coord));
    map.fitBounds(bounds);
};

// Initialize the map
const initializeMap = () => {
    if (!mapElement.value) return;

    const position = { lat: 51.47686, lng: 7.20766 };

    if (!map) {
        map = new google.maps.Map(mapElement.value, {
            center: position,
            zoom: 12,
            streetViewControl: false
        });
    } else {
        map.setCenter(position);
    }
};

// Load Google Maps API script dynamically if needed
const loadGoogleMapsScript = () => {
    return new Promise((resolve, reject) => {
        if (window.google && window.google.maps) {
            resolve();
        } else if (!document.querySelector("script[src*='maps.googleapis.com']")) {
            const script = document.createElement('script');
            script.src = `https://maps.googleapis.com/maps/api/js?key=AIzaSyDzh7DicT6KmawobOi6iir27IFEQBsRhRo&libraries=geometry`;
            script.async = true;
            script.defer = true;
            script.onload = resolve;
            script.onerror = reject;
            document.head.appendChild(script);
        }
    });
};

onMounted(async () => {
    await authStore.getUser();
    await fetchTrackersForUser();
    loadGoogleMapsScript().then(() => {
        initializeMap();
    });
});

watch(selectedTracker, updateSelectedTrackerMeasurements);
</script>

<style scoped>
.map-title {
    font-size: 1.2rem;
    font-weight: bold;
    color: #1f1f1f;
    text-align: center;
    margin: 0;
}

.dark-mode .map-title {
    color: #5A976D;
}

.checkbox-label {
    display: flex;
    align-items: center;
    font-size: 1rem;
    margin-top: 10px;
    accent-color: #00543D;
}

.dark-mode .checkbox-label {
    accent-color: #E69543;
    color: #ddd;
}

/* Styling for Build History button */
.build-history-button {
    width: 100%;
    padding: 10px;
    margin-top: 15px;
    background-color: #851515;
    color: #fff;
    border: none;
    border-radius: 18px;
    cursor: pointer;
    font-size: 1rem;
    box-shadow: inset 0 0 10px 3px rgba(0, 84, 61, 0.2);
}

.dark-mode .build-history-button {
    background-color: #E69543;
    box-shadow: 0 4px 8px rgba(255, 255, 255, 0.1);
    color: #1f1f1f;
}

.build-history-button:hover {
    transform: scale(1.05);
}

/* Error highlighting */
.error-dropdown {
    border: 2px solid #ff4d4d;
}

.error-message {
    color: #ff4d4d;
    font-size: 0.9rem;
    margin-top: 10px;
    text-align: center;
}

.center-button-container {
    display: flex;
    justify-content: center;
    margin-top: 20px;
}

/* View Details button styled similar to Build History button */
.view-details-button {
    width: 200px;
    padding: 10px;
    background-color: #851515;
    color: #fff;
    border: none;
    border-radius: 18px;
    cursor: pointer;
    font-size: 1rem;
    box-shadow: inset 0 0 10px 3px rgba(0, 84, 61, 0.2);
}

.dark-mode .view-details-button {
    background-color: #E69543;
    box-shadow: 0 4px 8px rgba(255, 255, 255, 0.1);
    color: #1f1f1f;
}

.view-details-button:hover {
    transform: scale(1.05);
}
</style>
