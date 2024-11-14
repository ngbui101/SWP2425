<template>
    <div class="card-view" :class="[(user?.settings?.template ?? 'default') === 'dark' ? 'dark-mode' : '']">
        <div class="tracker-card" v-for="tracker in trackers" :key="tracker._id">
            <!-- Info and Settings Icons at the top-right corner -->
            <div class="icon-wrapper">
                <div class="info-icon" @click="openInfoPopup(tracker)">
                    <i class="fas fa-info-circle"></i>
                </div>
                <div class="settings-icon" @click="openSettingsPopup(tracker)">
                    <i class="fas fa-cog"></i>
                </div>
            </div>

            <!-- Battery Bar above the card title -->
            <div class="battery-wrapper">
                <div class="battery-bar">
                    <div class="battery-fill"
                        :style="{ width: `${tracker.detailsWithTimestamps?.battery?.value.replace('%', '') || 0}%` }">
                    </div>
                </div>
                <div class="battery-indicator">
                    <span>
                        {{ tracker.detailsWithTimestamps?.battery?.value !== 'N/A' ?
                            tracker.detailsWithTimestamps?.battery?.value : 'N/A' }}
                    </span>
                </div>
            </div>

            <div class="card-header">
                <!-- Name Editing -->
                <h3 @dblclick="startEditingName(tracker)" @blur="saveTrackerName(tracker)"
                    @keydown.enter="saveTrackerName(tracker)">
                    <span v-if="!tracker.isEditingName">{{ tracker.name }}</span>
                    <input v-else type="text" v-model="tracker.editingName" class="name-input"
                        @blur="saveTrackerName(tracker)" maxlength="18" spellcheck="false" />
                </h3>
            </div>

            <div class="card-body">
                <!-- Display each key-value pair on its own line -->
                <p><strong>Mode:</strong> {{ tracker.modeLabel }}</p>
                <p><strong>Location:</strong> {{ tracker.location }}</p>
                <p><strong>Latitude:</strong> {{ tracker.detailsWithTimestamps?.latitude?.value }}</p>
                <p><strong>Longitude:</strong> {{ tracker.detailsWithTimestamps?.longitude?.value }}</p>
                <p><strong>Temp:</strong> {{ tracker.detailsWithTimestamps?.temperature?.value }}</p>
                <p><strong>Humidity:</strong> {{ tracker.detailsWithTimestamps?.humidity?.value }}</p>
                <p><strong>Device ID:</strong> {{ tracker.imei }}</p>
            </div>
        </div>

        <!-- Add Tracker as a card -->
        <div ref="tour3" class="tracker-card add-tracker-card" @click="openAddTrackerPopup"
            :class="{ 'scaling-effect': trackers.length === 0 }">
            <div class="card-body add-tracker-body">
                <i class="fas fa-plus"></i>&nbsp; Add Tracker
            </div>
        </div>

        <TrackerSettingsPopup v-if="showSettingsPopup" :selectedTrackerId="selectedTracker._id"
            :trackerNameInitial="selectedTracker.name" :trackerModeInitial="selectedTracker.mode"
            :sendingFrequencyInitial="selectedTracker.frequency" :template="user?.settings?.template"
            :closePopup="closeSettingsPopup" @updateTracker="handleUpdatedTracker" />

        <!-- Tracker Detail Popup -->
        <TrackerDetailPopup v-if="showInfoPopup" :tracker="selectedTracker" :template="user?.settings?.template"
            :closePopup="closeInfoPopup" />

        <!-- Add Tracker Popup -->
        <AddTrackerPopup v-if="showAddTrackerPopup" :template="user?.settings?.template"
            :closePopup="closeAddTrackerPopup" @tracker-added="handleTrackerAdded" />
    </div>
</template>

<script setup>
import { ref, onMounted, computed } from 'vue';
import { useAuthStore } from "@/stores/auth";
import { useApi, useApiPrivate } from "@/composables/useApi";
import TrackerSettingsPopup from './TrackerSettingsPopup.vue';
import TrackerDetailPopup from './TrackerDetailPopup.vue';
import AddTrackerPopup from './AddTrackerPopup.vue';
import { tour3 } from '@/routes/tourRefs.js';

const authStore = useAuthStore();
const user = computed(() => authStore.userDetail);
const trackers = ref([]);

const showSettingsPopup = ref(false);
const showInfoPopup = ref(false);
const showAddTrackerPopup = ref(false);
const selectedTracker = ref(null);
const handleUpdatedTracker = (updatedTracker) => {
    const trackerIndex = trackers.value.findIndex(tracker => tracker._id === updatedTracker.id);
    if (trackerIndex !== -1) {
        // Update the existing tracker with the new data
        trackers.value[trackerIndex] = {
            ...trackers.value[trackerIndex],
            ...updatedTracker,
            mode: updatedTracker.mode,
            modeLabel: updatedTracker.mode === 'RT' ? 'Real-Time-Tracking' : 'Long-Time-Tracking'
        };
    }
};

const fetchTrackersWithMeasurements = async () => {
    try {
        const api = useApiPrivate();
        const response = await api.get('http://localhost:3500/api/tracker/user/');
        const trackersWithDetails = response.data;

        for (const tracker of trackersWithDetails) {
            try {
                // Fetch mode data for the tracker
                const modeResponse = await api.get(`http://localhost:3500/api/mode/${tracker._id}`);
                tracker.modeDetails = modeResponse.data;

                tracker.modeLabel = tracker.modeDetails.GnssMode
                    ? 'Real-Time-Tracking'
                    : tracker.modeDetails.CellInfosMode
                        ? 'Long-Time-Tracking'
                        : 'Unknown Mode';

                // Fetch measurements for the tracker
                const measurementsResponse = await api.get(`http://localhost:3500/api/position/tracker/${tracker._id}`);
                const measurements = measurementsResponse.data;

                // Sort measurements by createdAt in descending order (newest to oldest)
                measurements.sort((a, b) => new Date(b.createdAt) - new Date(a.createdAt));

                // Log the sorted measurements array for verification
                console.log('Sorted Measurements array:', measurements);

                // Initialize details with no data and null timestamps
                tracker.detailsWithTimestamps = {
                    latitude: { value: 'N/A', timestamp: null },
                    longitude: { value: 'N/A', timestamp: null },
                    temperature: { value: 'N/A', timestamp: null },
                    humidity: { value: 'N/A', timestamp: null },
                    battery: { value: 'N/A', timestamp: null },
                };

                // Iterate through measurements to find the latest value for each type
                for (const measurement of measurements) {
                    const measurementDate = new Date(measurement.createdAt);

                    // Check and update latitude
                    if (measurement.latitude && !isNaN(measurement.latitude)) {
                        if (!tracker.detailsWithTimestamps.latitude.timestamp || measurementDate > new Date(tracker.detailsWithTimestamps.latitude.timestamp)) {
                            console.log(`Updating latitude: ${measurement.latitude} from ${measurementDate.toLocaleString()}`);
                            tracker.detailsWithTimestamps.latitude = {
                                value: measurement.latitude,
                                timestamp: measurementDate.toLocaleString(),
                            };
                        }
                    }

                    // Check and update longitude
                    if (measurement.longitude && !isNaN(measurement.longitude)) {
                        if (!tracker.detailsWithTimestamps.longitude.timestamp || measurementDate > new Date(tracker.detailsWithTimestamps.longitude.timestamp)) {
                            console.log(`Updating longitude: ${measurement.longitude} from ${measurementDate.toLocaleString()}`);
                            tracker.detailsWithTimestamps.longitude = {
                                value: measurement.longitude,
                                timestamp: measurementDate.toLocaleString(),
                            };
                        }
                    }

                    // Check and update temperature
                    if (measurement.temperature) {
                        if (!tracker.detailsWithTimestamps.temperature.timestamp || measurementDate > new Date(tracker.detailsWithTimestamps.temperature.timestamp)) {
                            console.log(`Updating temperature: ${measurement.temperature} from ${measurementDate.toLocaleString()}`);
                            tracker.detailsWithTimestamps.temperature = {
                                value: `${measurement.temperature}°C`,
                                timestamp: measurementDate.toLocaleString(),
                            };
                        }
                    }

                    // Check and update humidity
                    if (measurement.humidity) {
                        if (!tracker.detailsWithTimestamps.humidity.timestamp || measurementDate > new Date(tracker.detailsWithTimestamps.humidity.timestamp)) {
                            console.log(`Updating humidity: ${measurement.humidity} from ${measurementDate.toLocaleString()}`);
                            tracker.detailsWithTimestamps.humidity = {
                                value: `${measurement.humidity}%`,
                                timestamp: measurementDate.toLocaleString(),
                            };
                        }
                    }

                    // Check and update battery
                    if (measurement.battery) {
                        if (!tracker.detailsWithTimestamps.battery.timestamp || measurementDate > new Date(tracker.detailsWithTimestamps.battery.timestamp)) {
                            console.log(`Updating battery: ${measurement.battery} from ${measurementDate.toLocaleString()}`);
                            tracker.detailsWithTimestamps.battery = {
                                value: `${Math.round(measurement.battery)}%`,
                                timestamp: measurementDate.toLocaleString(),
                            };
                        }
                    }
                }

                // Get location based on the most recent latitude and longitude
                tracker.location =
                    tracker.detailsWithTimestamps.latitude.value !== 'N/A' && tracker.detailsWithTimestamps.longitude.value !== 'N/A'
                        ? await getReverseGeocodingAddress(
                            tracker.detailsWithTimestamps.latitude.value,
                            tracker.detailsWithTimestamps.longitude.value
                        )
                        : 'Unknown Location';
            } catch (error) {
                console.warn(`Error fetching data for tracker ${tracker._id}:`, error);
                tracker.detailsWithTimestamps = null;
                tracker.location = 'No data available';
                tracker.modeDetails = null;
                tracker.modeLabel = 'Unknown Mode';
            }
        }

        trackers.value = trackersWithDetails;
    } catch (error) {
        console.error('Failed to fetch trackers:', error);
    }
};


const getReverseGeocodingAddress = async (lat, lng) => {
    try {
        const response = await useApiPrivate().get(`http://localhost:3500/api/geocode?lat=${lat}&lng=${lng}`);
        return response.data.address || 'Unknown Location';
    } catch (error) {
        console.error(`Failed to get reverse geocoding address for coordinates [${lat}, ${lng}]:`, error);
        return 'Unknown Location';
    }
};

onMounted(async () => {
    await authStore.getUser();
    await fetchTrackersWithMeasurements(); // Use the updated function
});

const handleTrackerAdded = (newTracker) => {
    trackers.value.push(newTracker);
};

const openSettingsPopup = (tracker) => {
    selectedTracker.value = tracker;
    showSettingsPopup.value = true;
};

const closeSettingsPopup = () => {
    showSettingsPopup.value = false;
    selectedTracker.value = null;


};

const openInfoPopup = (tracker) => {
    selectedTracker.value = tracker;
    showInfoPopup.value = true;
};

const closeInfoPopup = () => {
    showInfoPopup.value = false;
    selectedTracker.value = null;
};

const openAddTrackerPopup = () => {
    showAddTrackerPopup.value = true;
};

const closeAddTrackerPopup = async () => {
    showAddTrackerPopup.value = false;

    try {
        // Call authStore.getUser() to refresh user data
        await authStore.getUser();
        await fetchTrackersWithMeasurements(); // Refresh the trackers list
    } catch (error) {
        console.error('Failed to refresh user data or fetch trackers:', error);
        alert('There was an issue refreshing the data. Please reload the page.');
    }
};

const startEditingName = (tracker) => {
    tracker.isEditingName = true;
    tracker.editingName = tracker.name;
};

const saveTrackerName = async (tracker) => {
    tracker.isEditingName = false;
    if (tracker.editingName && tracker.editingName !== tracker.name) {
        try {
            await useApiPrivate().put(`http://localhost:3500/api/tracker/${tracker._id}`, { name: tracker.editingName });
            tracker.name = tracker.editingName;
        } catch (error) {
            console.error('Failed to update the tracker name:', error);
        }
    }
};
</script>

<style scoped>
.icon-wrapper {
    display: flex;
    justify-content: space-between;
    position: absolute;
    top: 10px;
    left: 10px;
    right: 10px;
    z-index: 10;
    /* Ensure it stays above other content */
}

.info-icon,
.settings-icon {
    display: flex;
    /* Ensures the icon size matches the container */
    align-items: center;
    /* Centers the icon vertically */
    justify-content: center;
    /* Centers the icon horizontally */
    font-size: 18px;
    cursor: pointer;
    transition: transform 0.3s;
    padding: 2px;
    /* Adds a small padding to increase hover area */
}


.info-icon:hover,
.settings-icon:hover {
    transform: scale(1.05);
}

.card-view.dark-mode .info-icon {
    color: #E69543;
}

.card-view.dark-mode .settings-icon {
    color: #E69543;
}

/* Name input */
.name-input {
    background-color: transparent;
    border: none;
    font-size: 1.5rem;
    text-align: center;
    width: 100%;
    outline: none;
    color: inherit;
}


.name-input:focus {
    border-bottom: 1px solid #555;
}

/* Default light mode styles */
.card-view {
    display: flex;
    flex-wrap: wrap;
    gap: 20px;
    justify-content: center;
}

.tracker-card {
    background: rgb(255, 255, 255);
    padding: 20px;
    border: 1px solid #ddd;
    border-radius: 12px;
    width: 260px;
    box-shadow: 0 5px 10px rgba(0, 0, 0, 0.4);
    text-align: center;
    transition: transform 0.3s, box-shadow 0.3s;
    position: relative;
}

.tracker-card:hover {
    transform: scale(1.05);
    transition: transform 0.3s ease;
    z-index: 15;
}

/* Dark mode styles */
.dark-mode .tracker-card {
    background-color: #2e2e2e;
    border-color: #555;
    color: #bbb;
    box-shadow: 0 4px 8px rgba(255, 255, 255, 0.1);
}

.card-view.dark-mode .tracker-card:hover {
    box-shadow: 0 6px 12px rgba(255, 255, 255, 0.2);
}

/* Settings Icon */
.settings-icon {
    position: absolute;
    top: 10px;
    right: 10px;
    font-size: 18px;
    color: #851515;
    cursor: pointer;
}

@keyframes rotate360 {
    0% {
        transform: rotate(0deg);
    }

    100% {
        transform: rotate(360deg);
    }
}

.settings-icon:hover {
    transform: scale(1.3);
    animation: rotate360 2s linear infinite;
}

.card-view.dark-mode .settings-icon {
    color: #E69543;
}

/* Battery Bar Wrapper */
.battery-wrapper {
    display: flex;
    justify-content: center;
    align-items: center;
    margin-bottom: 10px;
    position: relative;
}

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

.battery-fill {
    background-color: #28a745;
    height: 100%;
    transition: width 0.3s;
}

.battery-indicator {
    font-size: 0.8rem;
    font-weight: bold;
    color: #333;
}

.card-view.dark-mode .battery-bar {
    background-color: #444;
    border-color: #777;
}

.card-view.dark-mode .battery-indicator {
    color: #bbb;
}

.dark-mode .card-header {
    margin-top: 10px;
    color: #5A976D;
}

.card-body {
    margin-top: 10px;
}

/* Aligning key-value pairs */
.card-body p {
    margin: 5px 0;
    font-size: 0.9rem;
    color: #333;
    text-align: left;
}

.card-view.dark-mode .card-body p {
    color: #bbb;
}

/* Add Tracker as a card */
.add-tracker-card {
    background: rgba(255, 255, 255, 1);
    border: 1px dashed #ddd;
    padding: 20px;
    border-radius: 12px;
    width: 260px;
    display: flex;
    align-items: center;
    justify-content: center;
    cursor: pointer;
    box-shadow: 0 5px 10px rgba(0, 0, 0, 0.2);
    transition: transform 0.3s, box-shadow 0.3s;
}

.scaling-effect {
    animation: scaleEffect 4s ease-in-out infinite;
}

/* Add tracker hover effect */
.add-tracker-card:hover {
    transform: scale(1.05);
    animation: none;
    /* Stop animation when hovering */
    box-shadow: 0 6px 12px rgba(0, 0, 0, 0.2);
}

/* Keyframe animation for scaling */
@keyframes scaleEffect {
    0% {
        transform: scale(1.0);
    }

    50% {
        transform: scale(1.4);
    }

    100% {
        transform: scale(1.0);
    }
}

/* Dark mode for Add Tracker */
.card-view.dark-mode .add-tracker-card {
    background-color: #2e2e2e;
    border-color: #555;
    box-shadow: 0 4px 8px rgba(255, 255, 255, 0.1);
}

.card-view.dark-mode .add-tracker-card:hover {
    box-shadow: 0 6px 12px rgba(255, 255, 255, 0.2);
}

.dark-mode .name-input {
    border: none;
    border-bottom: 2px solid #f1e4cc;
    /* Brighter border/underline */
    color: #f1e4cc;
    /* Brighter text color if needed */
    outline: none;
    /* Remove the default outline */
    background: transparent;
    /* Keep the background transparent */
}

.add-tracker-body {
    font-size: 1.2rem;
    color: #851515;
    text-align: center;
}

.card-view.dark-mode .add-tracker-body {
    color: #E69543;
}
</style>
