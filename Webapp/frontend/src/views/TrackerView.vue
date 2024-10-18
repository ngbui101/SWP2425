<template>
    <div :class="['tracker-view', (user.template ?? 'default') === 'dark' ? 'dark-mode' : '']">
        <!-- Toolbar with icons for List/Card view -->
        <div class="toolbar">
            <div class="view-switcher">
                <!-- List View Icon with Tooltip -->
                <div class="icon-container">
                    <i class="fas fa-list" @click="setView('list')" :class="{ active: currentView === 'list' }"></i>
                    <span class="tooltip">List View</span>
                </div>

                <!-- Card View Icon with Tooltip -->
                <div class="icon-container">
                    <i class="fas fa-th-large" @click="setView('card')" :class="{ active: currentView === 'card' }"></i>
                    <span class="tooltip">Card View</span>
                </div>
            </div>
        </div>

        <!-- Conditional rendering based on selected view -->
        <TrackerListComponent v-if="currentView === 'list'" :trackers="trackers" :user="user"
            @add-tracker="addTracker" />
        <TrackerCardComponent v-if="currentView === 'card'" :trackers="trackers" :user="user"
            @add-tracker="addTracker" />
    </div>
</template>



<script setup>
import { ref, onMounted, computed } from 'vue';
import axios from 'axios';
import TrackerListComponent from '../components/trackerview/TrackerListComponent.vue';
import TrackerCardComponent from '../components/trackerview/TrackerCardComponent.vue';
import { useAuthStore } from "@/stores/auth";

const trackers = ref([]);
const currentView = ref('card'); // Default view is now card view

// Fetch the user object from the auth store
const authStore = useAuthStore();
const user = computed(() => authStore.userDetail);

// Method to fetch trackers and their latest measurements
const fetchTrackersForUser = async () => {
    try {
        const token = authStore.accessToken;
        const config = { headers: { Authorization: `Bearer ${token}` } };

        // Fetch trackers for the authenticated user
        const response = await axios.get('http://localhost:3500/api/tracker/user/', config);
        const trackersWithLatestMeasurements = response.data;

        // Fetch measurements for each tracker
        for (const tracker of trackersWithLatestMeasurements) {
            const measurementsResponse = await axios.get(`http://localhost:3500/api/position/tracker/${tracker._id}`, config);
            const measurements = measurementsResponse.data;

            // Filter and sort measurements
            const validMeasurements = measurements.filter(measurement =>
                measurement.latitude && measurement.longitude && !isNaN(measurement.latitude) && !isNaN(measurement.longitude)
            );
            validMeasurements.sort((a, b) => new Date(b.createdAt) - new Date(a.createdAt));

            tracker.latestMeasurement = validMeasurements.length > 0 ? validMeasurements[0] : null;
            tracker.location = tracker.latestMeasurement ?
                await getReverseGeocodingAddress(tracker.latestMeasurement.latitude, tracker.latestMeasurement.longitude) :
                'Unknown Location';
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

// Change view (list or card)
const setView = (view) => {
    currentView.value = view;
};

// Add Tracker button action
const addTracker = () => {
    console.log('Add Tracker clicked');
};

// Fetch trackers on component mount
onMounted(async () => {
    await authStore.getUser();
    fetchTrackersForUser();
});
</script>

<style scoped>
.tracker-view {
    background-color: #f1e4cc;
    padding-bottom: 20px;
    min-height: 600px;
    padding: 13px;
}

.tracker-view.dark-mode {
    background-color: #1e1e1e;
}

/* Toolbar styling */
.toolbar {
    display: flex;
    align-items: center;
    margin-bottom: 10px;
}

/* View switcher icon container */
.view-switcher {
    display: flex;
    gap: 20px;
}

.view-switcher i {
    font-size: 24px;
    cursor: pointer;
    color: #555;
    transition: color 0.3s;
}

.view-switcher.dark-mode i {
    color: #bbb;
}

.view-switcher i.active {
    color: #333;
}

.view-switcher.dark-mode i.active {
    color: #fff;
}

.view-switcher i:hover {
    color: #000;
}

.view-switcher.dark-mode i:hover {
    color: #fff;
}

/* Tooltip styles */
.tooltip {
    visibility: hidden;
    background-color: #333;
    color: #fff;
    text-align: center;
    border-radius: 4px;
    padding: 5px;
    position: absolute;
    bottom: -25px;
    font-size: 12px;
    white-space: nowrap;
    opacity: 0;
    transition: opacity 0.3s;
}

.icon-container:hover .tooltip {
    visibility: visible;
    opacity: 1;
}
</style>
