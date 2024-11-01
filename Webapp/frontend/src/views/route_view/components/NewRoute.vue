<template>
    <div :class="['new-route-view', (user.settings?.template ?? 'default') === 'dark' ? 'dark-mode' : '']">
        <!-- Main card container -->
        <div class="card">
            <!-- Tracker selection dropdown -->
            <div class="tracker-info">
                <label for="tracker-select" class="dropdown-label">Select Tracker:</label>
                <select id="tracker-select" v-model="selectedTracker" @change="updateSelectedTracker">
                    <option v-for="tracker in trackers" :key="tracker._id" :value="tracker._id">
                        {{ tracker.name }}
                    </option>
                </select>
            </div>

            <!-- Start/Stop and Save Route controls -->
            <div class="route-controls">
                <button v-if="!isRouteActive" @click="startRoute" class="start-route-button">
                    Start Route
                </button>

                <div v-if="isRouteActive" class="timer-container">
                    <p>{{ formattedTime }}</p>
                    <button @click="stopRoute" class="stop-route-button">
                        Stop Route
                    </button>
                </div>

                <button v-if="showSaveButton" @click="saveRoute" class="save-route-button">
                    Save Route
                </button>
            </div>

            <!-- Map Container -->
            <div class="map-container">
                <div ref="mapElement" class="map"></div>
                <!-- Dark mode overlay -->
                <div v-if="(user.settings?.template ?? 'default') === 'dark'" class="map-overlay"></div>
            </div>
        </div>
    </div>
</template>

<script setup>
import { ref, computed, onMounted, onUnmounted } from 'vue';
import axios from 'axios';
import { useAuthStore } from "@/stores/auth";

const authStore = useAuthStore();
const user = computed(() => authStore.userDetail);

// Tracker data and controls
const trackers = ref([]);
const selectedTracker = ref(null);
const isRouteActive = ref(false);
const showSaveButton = ref(false);
const timer = ref(0);
let intervalId = null;

// Google Map setup
const mapElement = ref(null);
let map = null;

// Timer format
const formattedTime = computed(() => {
    const minutes = String(Math.floor(timer.value / 60)).padStart(2, '0');
    const seconds = String(timer.value % 60).padStart(2, '0');
    return `${minutes}:${seconds}`;
});

// Fetch tracker data from the backend
const fetchTrackers = async () => {
    try {
        const token = authStore.accessToken;
        const config = { headers: { Authorization: `Bearer ${token}` } };

        const response = await axios.get('http://localhost:3500/api/tracker/user/', config);
        trackers.value = response.data;
        selectedTracker.value = trackers.value.length > 0 ? trackers.value[0]._id : null;
    } catch (error) {
        console.error('Error fetching trackers:', error);
    }
};

// Start route with timer
const startRoute = () => {
    isRouteActive.value = true;
    showSaveButton.value = false;
    timer.value = 0;

    intervalId = setInterval(() => {
        timer.value += 1;
    }, 1000);
};

// Stop route and show save button
const stopRoute = () => {
    isRouteActive.value = false;
    clearInterval(intervalId);
    showSaveButton.value = true;
};

// Save route logic
const saveRoute = () => {
    console.log("Route saved for tracker:", selectedTracker.value);
    timer.value = 0;
    showSaveButton.value = false;
};

// Initialize Google Map
const initializeMap = () => {
    if (!mapElement.value) return;

    const position = { lat: 51.47686, lng: 7.20766 }; // Default center point
    map = new google.maps.Map(mapElement.value, {
        center: position,
        zoom: 12,
        streetViewControl: false,
    });
};

// Load Google Maps script dynamically
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

// Component lifecycle
onMounted(() => {
    fetchTrackers();
    loadGoogleMapsScript().then(() => {
        initializeMap();
    });
});

onUnmounted(() => {
    if (intervalId) clearInterval(intervalId);
});
</script>

<style scoped>
.new-route-view {
    display: flex;
    justify-content: center;
    align-items: center;
    background: linear-gradient(135deg, #f1e4cc 0%, #e6cc99 50%, #f1e4cc 100%);
    padding: 20px;
    min-height: 100vh;
}

.new-route-view.dark-mode {
    background: linear-gradient(135deg, #1e1e1e 0%, #141414 50%, #1e1e1e 100%);
}

.card {
    background-color: white;
    border-radius: 12px;
    box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1);
    padding: 20px;
    max-width: 600px;
    width: 100%;
}

.dark-mode .card {
    background-color: #2e2e2e;
    color: #ddd;
    box-shadow: 0 4px 10px rgba(255, 255, 255, 0.1);
}

.tracker-info {
    margin-bottom: 20px;
}

.dropdown-label {
    display: block;
    font-weight: bold;
    margin-bottom: 10px;
}

.route-controls {
    display: flex;
    flex-direction: column;
    align-items: center;
    margin-top: 20px;
}

.start-route-button,
.stop-route-button,
.save-route-button {
    padding: 10px 20px;
    font-size: 1rem;
    font-weight: bold;
    border: none;
    border-radius: 8px;
    margin: 10px;
    cursor: pointer;
}

.start-route-button {
    background-color: #5A976D;
    color: #fff;
}

.stop-route-button {
    background-color: #d9534f;
    color: #fff;
}

.save-route-button {
    background-color: #4285f4;
    color: #fff;
}

.timer-container p {
    font-size: 1.5rem;
    margin: 10px 0;
}

.map-container {
    margin-top: 20px;
    height: 300px;
    background: #f1f1f1;
    border: 1px solid #ddd;
    border-radius: 8px;
    overflow: hidden;
}

.map-overlay {
    position: absolute;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background: rgba(0, 0, 0, 0.3);
}
</style>
