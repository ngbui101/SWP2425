<template>
    <div :class="['container', (user.settings?.template ?? 'default') === 'dark' ? 'dark-mode' : '']">
        <div class="mapview-container">
            <!-- Left Info Container -->
            <div class="tracker-info-card">
                <div class="card-body">
                    <h2 class="card-title-route">{{ $t('RouteDetails') }}</h2>

                    <div class="controls-container">
                        <!-- Dropdown for Tracker Selection -->
                        <div class="tracker-dropdown">
                            <label for="tracker-select">{{ $t('SelectTracker') }}:</label>
                            <select id="tracker-select" v-model="selectedTracker">
                                <option v-for="tracker in trackers" :key="tracker._id" :value="tracker._id">
                                    {{ tracker.name }}
                                </option>
                            </select>
                        </div>

                        <!-- Start/Stop Buttons -->
                        <div v-if="!isRouteActive" class="start-container">
                            <button class="start-button" @click="startRoute" :disabled="!selectedTracker">{{
                                $t('Start')
                                }}</button>
                        </div>
                        <div v-else class="stop-container">
                            <p class="timer-text">{{ formattedTime }}</p>
                            <button class="stop-button" @click="stopRoute">{{ $t('Stop') }}</button>
                        </div>
                    </div>

                    <!-- Statistics -->
                    <div class="stats-grid">
                        <div class="stat-item">
                            <strong>{{ $t('Steps') }}</strong>
                            <p>{{ stepsWalked }}</p>
                        </div>
                        <div class="stat-item">
                            <strong>{{ $t('AvgSpeed') }}</strong>
                            <p>{{ avgSpeed }} km/h</p>
                        </div>
                        <div class="stat-item">
                            <strong>{{ $t('MaxSpeed') }}</strong>
                            <p>{{ maxSpeed }} km/h</p>
                        </div>
                        <div class="stat-item">
                            <strong>{{ $t('TotalDistance') }}</strong>
                            <p>{{ totalDistance }} km</p>
                        </div>
                    </div>
                </div>
            </div>

            <!-- Right Map Container -->
            <div class="card map-card">
                <div class="map-container">
                    <div ref="mapElement" class="map"></div>
                </div>
                <!-- Save Route Button -->
                <!-- Save Route Container -->
                <div v-if="showSaveButton" class="save-route-container">
                    <input v-model="routeName" placeholder="Enter route name" class="route-name-input" />
                    <button class="save-route-button" @click="saveRoute">{{ $t('SaveRoute') }}</button>
                </div>

            </div>
        </div>
    </div>
</template>

<script setup>
import { ref, computed, onMounted, watch } from "vue";
import axios from "axios";
import { useAuthStore } from "@/stores/auth";
import { useApiPrivate } from "@/composables/useApi";
const authStore = useAuthStore();
const user = computed(() => authStore.userDetail);

const smoothedPosition = ref(null);  // To store the last smoothed position
const routeName = ref('');

const api = useApiPrivate();
// Trackers and selected tracker
const trackers = ref([]);
const selectedTracker = ref(null);
const selectedMeasurement = ref({});
const lastMeasurementTime = ref(null); // Tracks the latest measurement timestamp
const mapElement = ref(null);

// Timer and route state
const isRouteActive = ref(false);
const timerVisible = ref(false);
const timer = ref(0);
let liveUpdateInterval = null;
let simulatedMeasurementInterval = null; // Interval for sending simulated measurements
let timerInterval = null; // Interval for the timer

// Statistics data
const stepsWalked = ref(0);
const avgSpeed = ref(0);
const maxSpeed = ref(0);
const totalDistance = ref(0);

// Map variables
let map = null;
let marker = null;
let polyline = null; // For drawing the route
const routePath = ref([]); // Stores the path coordinates for the route
const startPointMarker = ref(null); // Green dot for the start point
const endPointMarker = ref(null); // Blue dot for the endpoint
const startLabel = ref(null); // Label for start point
const endLabel = ref(null); // Label for endpoint

// Array to store new fetched measurement IDs
const measurementIds = ref([]); // Store fetched measurement IDs

// Save old tracker settings
const oldTrackerSettings = ref({});

const generateTestRoute = () => {
    const startLat = 48.8566;  // Start at a specific latitude (e.g., Paris coordinates)
    const startLng = 2.3522;   // Start at a specific longitude
    const positions = [];

    for (let i = 0; i < 15; i++) {
        // Add random small increments to the latitude and longitude for test positions
        const newPosition = {
            lat: startLat + i * 0.001,
            lng: startLng + i * 0.001,
        };
        positions.push(newPosition);
    }

    return positions;
};

// Timer formatting
const formattedTime = computed(() => {
    const minutes = Math.floor(timer.value / 60);
    const seconds = timer.value % 60;
    return `${minutes.toString().padStart(2, "0")}:${seconds.toString().padStart(2, "0")}`;
});


const showSaveButton = ref(false);  // Track if the save button is displayed

/* const saveRoute = async () => {
    try {
        // Send route data to backend (replace with your API logic)
        const routeData = {
            name: routeName.value || `Route - ${new Date().toLocaleString()}`,
            positions: routePath.value,
            totalTime: timer.value,
            steps: stepsWalked.value,
            avgSpeed: parseFloat(avgSpeed.value) / 3.6,
            maxSpeed: parseFloat(maxSpeed.value) / 3.6,
            caloriesBurned: 0,
        };

        const token = authStore.accessToken;
        const config = { headers: { Authorization: `Bearer ${token}` } };

        await axios.post('http://localhost:3500/api/routes', routeData, config);

        alert('Route saved successfully');

        // Reset fields and hide the save container
        routeName.value = '';
        showSaveButton.value = false;
    } catch (error) {
        console.error('Failed to save route:', error);
        alert('Error saving route');
    }
}; */

const saveRoute = async () => {
    try {
        // Generate a test route with dummy positions
        const testPositions = generateTestRoute();

        // Send route data to backend
        const routeData = {
            name: routeName.value || `Test Route - ${new Date().toLocaleString()}`,
            positions: testPositions,  // Use generated test positions
            totalTime: 900,            // Assume 15 minutes (900 seconds)
            steps: 1500,               // Assume 1500 steps
            avgSpeed: 4.5,             // Assume average speed of 4.5 km/h
            maxSpeed: 6.0,             // Assume max speed of 6.0 km/h
            caloriesBurned: 120,       // Assume 120 calories burned
        };

        const token = authStore.accessToken;
        const config = { headers: { Authorization: `Bearer ${token}` } };

        await axios.post('http://localhost:3500/api/routes', routeData, config);

        alert('Test route saved successfully');

        // Reset fields and hide the save container
        routeName.value = '';
        showSaveButton.value = false;
    } catch (error) {
        console.error('Failed to save route:', error);
        alert('Error saving route');
    }
};

// Fetch trackers and their latest measurements
const fetchTrackersForUser = async () => {
    try {
        const token = authStore.accessToken;
        const config = { headers: { Authorization: `Bearer ${token}` } };

        const response = await axios.get("http://localhost:3500/api/tracker/user/", config);
        trackers.value = response.data;

        for (const tracker of trackers.value) {
            const measurementsResponse = await axios.get(
                `http://localhost:3500/api/position/tracker/${tracker._id}`,
                config
            );
            tracker.measurements = measurementsResponse.data.filter((m) => m.latitude && m.longitude);
        }

        if (trackers.value.length > 0 && !selectedTracker.value) {
            selectedTracker.value = trackers.value[0]._id;
        }

        if (selectedTracker.value) {
            updateSelectedMeasurement();
        }
    } catch (error) {
        console.error("Failed to fetch trackers:", error);
    }
};

// Update the selected tracker's latest measurement
const updateSelectedMeasurement = async () => {
    if (!selectedTracker.value) return;

    const tracker = trackers.value.find((t) => t._id === selectedTracker.value);
    if (!tracker) return;

    const token = authStore.accessToken;
    const config = { headers: { Authorization: `Bearer ${token}` } };

    try {
        const measurementsResponse = await axios.get(
            `http://localhost:3500/api/position/tracker/${selectedTracker.value}`,
            config
        );

        const validMeasurement = measurementsResponse.data.find(
            (m) => m.latitude !== undefined && m.longitude !== undefined && !isNaN(m.latitude) && !isNaN(m.longitude)
        );

        if (validMeasurement) {
            selectedMeasurement.value = validMeasurement;

            const position = {
                lat: Number(validMeasurement.latitude),
                lng: Number(validMeasurement.longitude),
            };

            // Ensure the map is initialized
            await initializeMap();

            // Update or create the marker
            if (marker) {
                marker.setPosition(position);  // Update the marker position
            } else {
                marker = new google.maps.Marker({
                    position,
                    map,
                    title: "Tracker Position",
                });
            }

            // Center the map on the new position
            map.setCenter(position);
        } else {
            console.warn("No valid measurements found for the selected tracker.");
            selectedMeasurement.value = null;
        }
    } catch (error) {
        console.error("Failed to fetch the selected tracker's measurements:", error);
    }
};

const waitForGoogleMaps = async () => {
    return new Promise((resolve) => {
        const checkGoogle = () => {
            if (window.google && window.google.maps) {
                resolve();
            } else {
                setTimeout(checkGoogle, 100);  // Retry every 100ms until Google Maps API is ready
            }
        };
        checkGoogle();
    });
};


// Initialize or update the map with the latest position
const initializeMap = async () => {
    if (map) return;  // Only initialize if map is null

    if (!selectedMeasurement.value || !mapElement.value) return;

    await waitForGoogleMaps();  // Ensure Google Maps API is loaded

    const position = {
        lat: Number(selectedMeasurement.value.latitude),
        lng: Number(selectedMeasurement.value.longitude),
    };

    // Initialize the map
    map = new google.maps.Map(mapElement.value, {
        center: position,
        zoom: 30,
    });

    // Initialize polyline for the route
    polyline = new google.maps.Polyline({
        path: routePath.value,
        geodesic: true,
        strokeColor: "#FF0000",
        strokeOpacity: 1.0,
        strokeWeight: 2,
    });
    polyline.setMap(map);
};

const updateMapElements = (position) => {
    if (!map) return;

    // Update or create marker
    if (marker) {
        marker.setPosition(position);  // Update the marker position
    } else {
        marker = new google.maps.Marker({
            position,
            map,
            title: "Tracker Position",
        });
    }

    // Update map center
    map.setCenter(position);

    // Update polyline path if route is active
    if (isRouteActive.value && routePath.value.length > 0) {
        polyline.setPath(routePath.value);
    }
};

// Start route logic with live updates
const startRoute = async () => {
    if (!selectedTracker.value) return;

    lastMeasurementTime.value = new Date();  // Reset timestamp
    isRouteActive.value = true;
    timerVisible.value = true;
    timer.value = 0;
    stepsWalked.value = 0;
    avgSpeed.value = 0;
    maxSpeed.value = 0;
    totalDistance.value = 0;
    routePath.value = [];  // Reset route path
    measurementIds.value = [];

    // Fetch and save old tracker mode settings
    try {
        const { data } = await api.get(`http://localhost:3500/api/mode/${selectedTracker.value}`);
        oldTrackerSettings.value = data;  // Save the full mode object
        console.log("Old tracker mode fetched and saved:", oldTrackerSettings.value);
    } catch (error) {
        console.error("Failed to fetch tracker mode:", error);
        return;  // Stop execution if fetching mode fails
    }

    try {
        // Update tracker mode to real-time mode
        await authStore.updateTrackerMode(
            selectedTracker.value,
            true,          // GnssMode (real-time mode)
            5,             // Frequency in seconds (5000ms)
            undefined,     // No hourly frequency needed
            false,         // BatteryMode
            false          // TemperatureMode
        );

        console.log("Tracker mode updated to real-time mode.");
    } catch (error) {
        console.error("Failed to set tracker mode:", error);
        return;  // Stop execution if tracker mode fails to update
    }

    // Initialize the map
    await initializeMap();

    // Start fetching measurements every 3 seconds
    liveUpdateInterval = setInterval(async () => {
        await fetchMeasurements();
    }, 3000);

    // Start the timer
    timerInterval = setInterval(() => {
        timer.value += 1;
    }, 1000);
};





const addStartMarker = (measurement) => {
    const position = {
        lat: Number(measurement.latitude),
        lng: Number(measurement.longitude),
    };

    if (startPointMarker.value) {
        startPointMarker.value.setMap(null);  // Remove any existing start marker
    }

    // Add a new marker with the "Start" label
    startPointMarker.value = new google.maps.Marker({
        position,
        map,
        icon: {
            path: google.maps.SymbolPath.CIRCLE,
            scale: 8,  // Size of the circle marker
            fillColor: "green",
            fillOpacity: 1,
            strokeColor: "green",
            strokeWeight: 2,
            labelOrigin: new google.maps.Point(0, -2),
        },
        label: {
            text: "Start",
            color: "green",
            fontSize: "12px",
            fontWeight: "bold",
        },
    });
};

// Fetch all measurements and process only new ones
const fetchMeasurements = async () => {
    const tracker = trackers.value.find((t) => t._id === selectedTracker.value);
    if (!tracker) return;

    const token = authStore.accessToken;
    const config = { headers: { Authorization: `Bearer ${token}` } };

    try {
        const response = await axios.get(
            `http://localhost:3500/api/position/tracker/${selectedTracker.value}`,
            config
        );

        const newMeasurements = response.data.filter(
            (m) =>
                m.latitude &&
                m.longitude &&
                new Date(m.createdAt) > lastMeasurementTime.value &&
                m.speed !== undefined &&
                m.step !== undefined
        );

        console.log("New measurements fetched:", newMeasurements);

        for (const measurement of newMeasurements) {
            // Add the start marker on the first new measurement
            if (routePath.value.length === 0 && !startPointMarker.value) {
                addStartMarker(measurement);
            }

            updateMapWithNewPosition(measurement);
            measurementIds.value.push(measurement._id);  // Save measurement IDs
        }
    } catch (error) {
        console.error("Failed to fetch measurements:", error);
    }
};
let lastStepCount = 0;
// Update the map with the new position
function applyLowPassFilter(newPosition, previousPosition, alpha = 0.2) {
    return {
        lat: alpha * newPosition.lat + (1 - alpha) * (previousPosition?.lat ?? newPosition.lat),
        lng: alpha * newPosition.lng + (1 - alpha) * (previousPosition?.lng ?? newPosition.lng),
    };
}

let lastPosition = null;

const updateMapWithNewPosition = (measurement) => {
    console.log("Updating map with new measurement:", measurement);

    if (measurement.step > lastStepCount || (routePath.value.length === 0 && measurement.step > 0)) {
        const newPosition = {
            lat: Number(measurement.latitude),
            lng: Number(measurement.longitude),
        };

        // Apply low-pass filter
        smoothedPosition.value = applyLowPassFilter(newPosition, smoothedPosition.value);

        // Add the smoothed position to the route path
        routePath.value.push(smoothedPosition.value);

        // Calculate distance from the last position
        if (lastPosition) {
            const distance = calculateHaversineDistance(lastPosition, smoothedPosition.value);
            totalDistance.value = (Number(totalDistance.value) + distance / 1000).toFixed(2);  // Convert to km
        }
        lastPosition = smoothedPosition.value;

        // Update the polyline to connect all points
        polyline.setPath(routePath.value);

        // Update the main marker to the smoothed position
        if (marker) {
            marker.setPosition(smoothedPosition.value);
        }

        map.setCenter(smoothedPosition.value);

        // Update statistics
        stepsWalked.value = measurement.step;
        avgSpeed.value = (measurement.speed * 3.6).toFixed(2);  // Convert speed to km/h
        maxSpeed.value = Math.max(maxSpeed.value, measurement.speed * 3.6).toFixed(2);

        lastStepCount = measurement.step;
        lastMeasurementTime.value = new Date(measurement.createdAt);
    } else {
        console.log("Skipped measurement due to no increase in step count.");
    }
};

function calculateHaversineDistance(coord1, coord2) {
    const R = 6371e3;  // Earth's radius in meters
    const lat1 = (coord1.lat * Math.PI) / 180;
    const lat2 = (coord2.lat * Math.PI) / 180;
    const deltaLat = lat2 - lat1;
    const deltaLng = ((coord2.lng - coord1.lng) * Math.PI) / 180;

    const a =
        Math.sin(deltaLat / 2) * Math.sin(deltaLat / 2) +
        Math.cos(lat1) * Math.cos(lat2) * Math.sin(deltaLng / 2) * Math.sin(deltaLng / 2);
    const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));

    return R * c;  // Distance in meters
}


// Stop route logic
const stopRoute = async () => {
    isRouteActive.value = false;
    clearInterval(liveUpdateInterval);
    clearInterval(timerInterval);
    liveUpdateInterval = null;
    timerInterval = null;

    // Mark the last point as the endpoint
    if (routePath.value.length > 0) {
        const lastPoint = routePath.value[routePath.value.length - 1];

        if (endPointMarker.value) {
            endPointMarker.value.setMap(null);  // Remove old endpoint marker if it exists
        }

        endPointMarker.value = new google.maps.Marker({
            position: lastPoint,
            map,
            icon: {
                path: google.maps.SymbolPath.CIRCLE,
                scale: 8,
                fillColor: "blue",
                fillOpacity: 1,
                strokeColor: "blue",
                strokeWeight: 2,
                labelOrigin: new google.maps.Point(0, -2),
            },
            label: {
                text: "End",
                color: "blue",
                fontSize: "12px",
                fontWeight: "bold",
            },
        });
    }

    if (marker) {
        marker.setMap(null);
    }

    // Delete fetched measurements
    const token = authStore.accessToken;
    const config = { headers: { Authorization: `Bearer ${token}` } };

    try {
        for (const id of measurementIds.value) {
            await axios.delete(`http://localhost:3500/api/position/${id}`, config);
            console.log(`Measurement ${id} deleted.`);
        }
        measurementIds.value = [];
    } catch (error) {
        console.error("Failed to delete measurements:", error);
    }

    // Restore old tracker settings
    try {
        if (!oldTrackerSettings.value?.frequenz) {
            const { data } = await api.get(`http://localhost:3500/api/mode/${selectedTracker.value}`);
            oldTrackerSettings.value = data;
            console.log("Fetched tracker mode details for restoration:", oldTrackerSettings.value);
        }

        await authStore.updateTrackerMode(
            selectedTracker.value,
            oldTrackerSettings.value.GnssMode ?? false,
            oldTrackerSettings.value.frequenz ? oldTrackerSettings.value.frequenz / 1000 : 3600,
            undefined,
            oldTrackerSettings.value.BatteryMode ?? true,
            oldTrackerSettings.value.TemperatureMode ?? true
        );
        console.log("Tracker mode restored to previous settings.");
    } catch (error) {
        console.error("Failed to restore tracker settings:", error);
    }

    // Show the save button
    showSaveButton.value = true;
};


const loadGoogleMapsScript = () => {
    return new Promise((resolve, reject) => {
        if (window.google && window.google.maps) {
            resolve(); // Already loaded
        } else if (!document.querySelector("script[src*='maps.googleapis.com']")) {
            const script = document.createElement("script");
            script.src = `https://maps.googleapis.com/maps/api/js?key=AIzaSyDzh7DicT6KmawobOi6iir27IFEQBsRhRo&libraries=geometry`;
            script.async = true;
            script.defer = true;
            script.onload = resolve;
            script.onerror = reject;
            document.head.appendChild(script);
        } else {
            // Wait for the existing script to load
            script.onload = resolve;
        }
    });
};

// Watch for tracker selection changes
watch(selectedTracker, updateSelectedMeasurement);
watch(selectedTracker, async () => {
    // Clear only the route path and polyline, but let the marker be updated in `updateSelectedMeasurement`
    routePath.value = [];
    if (polyline) polyline.setPath([]);

    // Call the function to update measurements and marker position
    await updateSelectedMeasurement();
});
// On component mount
onMounted(async () => {
    await authStore.getUser();

    if (!window.google || !window.google.maps) {
        await loadGoogleMapsScript(); // Ensure Google Maps is loaded
    }

    await fetchTrackersForUser(); // Fetch trackers after maps is loaded
});
</script>
<style scoped>
.card-title-route {
    font-size: 1.5rem;
    font-weight: bold;
    color: #ffae00;
    text-align: center;
}

.container {
    display: flex;
    flex-direction: column;
    gap: 20px;
    padding: 20px;
}

.mapview-container {
    display: flex;
    flex-wrap: wrap;
    gap: 20px;
    justify-content: space-between;
}

/* Info Card */
.tracker-info-card {
    flex: 1;
    min-width: 300px;
    max-width: 400px;
    background: #f8f9fa;
    padding: 20px;
    border-radius: 8px;
    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
}

.dark-mode .tracker-info-card {
    background: #2e2e2e;
    color: #ddd;
}

.card-body {
    display: flex;
    flex-direction: column;
    align-items: center;
}

.controls-container {
    display: flex;
    flex-direction: column;
    align-items: center;
    margin-bottom: 20px;
}

.timer-text {
    font-size: 2rem;
    font-weight: bold;
    margin: 10px 0;
}

.time-display p {
    font-size: 1.5rem;
    font-weight: bold;
}

.start-button,
.stop-button {
    font-size: 1.2rem;
    padding: 10px 20px;
    border-radius: 5px;
    border: none;
    cursor: pointer;
    transition: background-color 0.3s ease;
}

.start-button {
    background-color: #28a745;
    color: #fff;
}

.start-button:hover {
    background-color: #218838;
}

.stop-button {
    background-color: #dc3545;
    color: #fff;
}

.stop-button:hover {
    background-color: #c82333;
}

/* Stats Grid */
.stats-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
    /* Items adapt to available space */
    gap: 20px;
    width: 100%;
}

.stat-item {
    white-space: normal;
    /* Allow text to wrap */
    word-break: break-word;
    /* Break long words if necessary */
}

.dark-mode .stat-item {
    background: #444;
    color: #ddd;
}

/* Map Card */
.map-card {
    flex: 2;
    min-width: 600px;
    background: #f8f9fa;
    padding: 20px;
    border-radius: 8px;
    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
}

.dark-mode .map-card {
    background: #2e2e2e;
    color: #ddd;
}

.map-container {
    width: 100%;
    height: 500px;
    border-radius: 8px;
}

.map {
    width: 100%;
    height: 100%;
}

.dark-mode .map-container {
    border: 1px solid #555;
}

.tracker-dropdown {
    margin-bottom: 20px;
    text-align: center;
}

.tracker-dropdown select {
    padding: 10px;
    font-size: 1rem;
    border-radius: 5px;
    border: 1px solid #ccc;
    width: 100%;
}

.tracker-dropdown label {
    display: block;
    margin-bottom: 5px;
    font-weight: bold;
}

.save-route-container {
    display: flex;
    justify-content: center;
    margin-top: 20px;
}

.save-route-button {
    font-size: 1.2rem;
    padding: 10px 20px;
    background-color: #007bff;
    color: #fff;
    border: none;
    border-radius: 8px;
    cursor: pointer;
    transition: background-color 0.3s ease;
}

.save-route-button:hover {
    background-color: #0056b3;
}

.route-name-input {
    margin-right: 10px;
    padding: 10px;
    font-size: 1rem;
    border: 1px solid #ccc;
    border-radius: 8px;
    width: 50%;
}


/* Responsive Design */
@media screen and (max-width: 768px) {
    .mapview-container {
        flex-direction: column;
        align-items: center;
    }

    .tracker-info-card,
    .map-card {
        width: 100%;
    }

    .stats-grid {
        grid-template-columns: 1fr;
    }
}
</style>