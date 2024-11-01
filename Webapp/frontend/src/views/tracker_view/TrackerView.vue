<template>
    <div :class="['tracker-view', (user.settings?.template ?? 'default') === 'dark' ? 'dark-mode' : '']">
        <!-- Toolbar with Toggle Bar for List/Card view -->
        <div class="toolbar">
            <div class="toggle-container" @mousedown="startDragging($event)" @mouseup="stopDragging"
                @mouseleave="stopDragging" @mousemove="handleDragging($event)">

                <!-- Sliding background div -->
                <div class="slider"
                    :class="{ 'slide-left': currentView === 'card', 'slide-right': currentView === 'list' }"></div>

                <!-- Card View Toggle (Left) -->
                <label @click="setView('card')" :class="{ active: currentView === 'card' }">
                    Card View
                </label>

                <!-- List View Toggle (Right) -->
                <label @click="setView('list')" :class="{ active: currentView === 'list' }">
                    List View
                </label>
            </div>
        </div>

        <!-- Conditional rendering based on selected view -->
        <TrackerCardComponent v-if="currentView === 'card'" :trackers="trackers" :user="user"
            @add-tracker="addTracker" />
        <TrackerListComponent v-if="currentView === 'list'" :trackers="trackers" :user="user"
            @add-tracker="addTracker" />
    </div>
</template>

<script setup>
import { ref, onMounted, computed } from 'vue';
import axios from 'axios';
import TrackerListComponent from './components/TrackerListComponent.vue';
import TrackerCardComponent from './/components/TrackerCardComponent.vue';
import { useAuthStore } from "@/stores/auth";

const trackers = ref([]);
const currentView = ref('card'); // Default view is now card
let isDragging = ref(false); // Indicates if the slider is being dragged
let sliderPosition = ref(100); // Default slider starts at Card View (rightmost)
let draggingDirection = ref(''); // Tracks the drag direction

// Fetch user from the auth store
const authStore = useAuthStore();
const user = computed(() => authStore.userDetail);

// Method to change the view
const setView = (view) => {
    currentView.value = view;
    sliderPosition.value = view === 'card' ? 0 : 100; // Card View on left (0), List View on right (100)
};

// Start dragging the slider
const startDragging = (event) => {
    isDragging.value = true;
    updateSliderPosition(event.clientX);
};

// Stop dragging the slider
const stopDragging = () => {
    if (!isDragging.value) return;
    isDragging.value = false;

    // Determine which view is closer to the slider
    if (sliderPosition.value > 50) {
        setView('list'); // Snap to List View (right side)
    } else {
        setView('card'); // Snap to Card View (left side)
    }

    // Reset dragging direction after stop
    draggingDirection.value = '';
};

// Handle the dragging movement
const handleDragging = (event) => {
    if (!isDragging.value) return;

    const oldSliderPosition = sliderPosition.value;
    updateSliderPosition(event.clientX);

    // Set the dragging direction based on the new position
    draggingDirection.value = sliderPosition.value > oldSliderPosition ? 'right' : 'left';
};

// Update the slider's position based on the mouse X position
const updateSliderPosition = (clientX) => {
    const container = document.querySelector('.toggle-container');
    const rect = container.getBoundingClientRect();
    const percentage = ((clientX - rect.left) / rect.width) * 100;

    // Boundaries to prevent dragging outside
    sliderPosition.value = Math.min(100, Math.max(0, percentage));
};

// Fetch trackers and latest measurements
const fetchTrackersForUser = async () => {
    try {
        const token = authStore.accessToken;
        const config = { headers: { Authorization: `Bearer ${token}` } };

        const response = await axios.get('http://localhost:3500/api/tracker/user/', config);
        const trackersWithLatestMeasurements = response.data;

        for (const tracker of trackersWithLatestMeasurements) {
            try {
                // Attempt to fetch measurements
                const measurementsResponse = await axios.get(`http://localhost:3500/api/position/tracker/${tracker._id}`, config);
                const measurements = measurementsResponse.data;

                const validMeasurements = measurements.filter(measurement =>
                    measurement.latitude && measurement.longitude && !isNaN(measurement.latitude) && !isNaN(measurement.longitude)
                );
                validMeasurements.sort((a, b) => new Date(b.createdAt) - new Date(a.createdAt));

                tracker.latestMeasurement = validMeasurements.length > 0 ? validMeasurements[0] : null;
                tracker.location = tracker.latestMeasurement
                    ? await getReverseGeocodingAddress(tracker.latestMeasurement.latitude, tracker.latestMeasurement.longitude)
                    : 'Unknown Location';
            } catch (measurementError) {
                // If no measurements, set default values
                console.warn(`No measurements found for tracker ${tracker._id}`);
                tracker.latestMeasurement = null;
                tracker.location = 'No data available';
            }
        }

        trackers.value = trackersWithLatestMeasurements;
    } catch (error) {
        console.error('Failed to fetch trackers or measurements:', error);
    }
};

// Perform reverse geocoding using your backend API
const getReverseGeocodingAddress = async (lat, lng) => {
    try {
        const response = await axios.get(`http://localhost:3500/api/geocode?lat=${lat}&lng=${lng}`);
        return response.data?.address || 'Unknown Location';
    } catch (error) {
        console.error('Failed to perform reverse geocoding:', error);
        return 'Unknown Location';
    }
};

// Fetch trackers on component mount
onMounted(async () => {
    await authStore.getUser();
    fetchTrackersForUser();
});
</script>


<style scoped>
.tracker-view {
    background: linear-gradient(135deg, #f1e4cc 0%, #e6cc99 50%, #f1e4cc 100%);
    padding-bottom: 20px;
    min-height: 100vh;
    padding-top: 15px;
}

.tracker-view.dark-mode {
    background: linear-gradient(135deg, #1e1e1e 0%, #141414 50%, #1e1e1e 100%);
}

/* Toolbar styling */
.toolbar {
    display: flex;
    justify-content: center;
    align-items: center;
    margin-top: 10px;
    margin-bottom: 20px;
}

/* Toggle container */
.toggle-container {
    position: relative;
    width: 250px;
    height: 50px;
    border-radius: 30px;
    border: 1px solid #000;
    display: flex;
    align-items: center;
    justify-content: space-between;
    background-color: white;
    overflow: hidden;
    user-select: none;
}

/* Sliding background */
.slider {
    position: absolute;
    top: 0;
    bottom: 0;
    width: 50%;
    background-color: #C19A6B;
    border-radius: 30px 0 0 30px;
    transition: transform 0s ease;
    z-index: 0;
    transform: translateX(calc(var(--slider-pos, 0) * 1%));
}

.slide-left {
    transform: translateX(0);
}

.slide-right {
    border-radius: 0 30px 30px 0;
    transform: translateX(100%);
}

/* Toggle labels */
.toggle-container label {
    width: 50%;
    text-align: center;
    font-weight: bold;
    padding: 10px 0;
    cursor: pointer;
    z-index: 1;
    /* Ensures text is on top of slider */
}

.toggle-container .active {
    color: rgb(0, 0, 0);
    /* Color of active label */
    border-right: none;
    /* Remove right border when active */
}

/* Remove left border on Card View when active */
.toggle-container label:last-of-type.active {
    border-left: none;
}

.toggle-container label:not(.active) {
    color: #1f1f1f;
    border-left: none;
}

/* Dark mode styles */
.dark-mode .toggle-container {
    background-color: #333;
    border-color: #555;
}

.dark-mode .slider {
    background-color: #5A976D;
}

.dark-mode .toggle-container label {
    color: white;
}

/* Add other styles back */
.dark-mode .toggle-container .active {
    background-color: #5A976D;
    color: #333;
}

.dark-mode .toggle-container label:not(.active) {
    color: #aaa;
}
</style>