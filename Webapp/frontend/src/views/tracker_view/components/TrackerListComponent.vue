<template>
    <div :class="(user.settings?.template ?? 'default') === 'dark' ? 'dark-mode' : ''">
        <table v-if="trackers.length > 0" class="tracker-table">
            <thead>
                <tr>
                    <th>Name</th>
                    <th>Mode</th>
                    <th>Location</th>
                    <th>Latitude</th>
                    <th>Longitude</th>
                    <th>Temperature</th>
                    <th>Humidity</th>
                    <th>Battery</th>
                    <th>Device ID</th>
                    <th>Actions</th>
                </tr>
            </thead>
            <tbody>
                <tr v-for="tracker in trackers" :key="tracker._id">
                    <td @dblclick="startEditingName(tracker)">
                        <span v-if="!tracker.isEditingName">{{ tracker.name }}</span>
                        <input v-else type="text" v-model="tracker.editingName" class="name-input"
                            @blur="saveTrackerName(tracker)" @keydown.enter="saveTrackerName(tracker)" maxlength="18"
                            spellcheck="false" />
                    </td>
                    <td>{{ tracker.modeLabel }}</td>
                    <td>{{ tracker.location }}</td>
                    <td>{{ tracker.detailsWithTimestamps?.latitude?.value || 'N/A' }}</td>
                    <td>{{ tracker.detailsWithTimestamps?.longitude?.value || 'N/A' }}</td>
                    <td>{{ tracker.detailsWithTimestamps?.temperature?.value || 'N/A' }}</td>
                    <td>{{ tracker.detailsWithTimestamps?.humidity?.value || 'N/A' }}</td>
                    <td>{{ tracker.detailsWithTimestamps?.battery?.value || 'N/A' }}</td>
                    <td>{{ tracker.imei || 'N/A' }}</td>
                    <td>
                        <i class="fas fa-info-circle info-icon" @click="openInfoPopup(tracker)"></i>
                        <i class="fas fa-cog settings-icon" @click="openSettingsPopup(tracker)"></i>
                    </td>
                </tr>
            </tbody>
        </table>

        <!-- Add Tracker Button below the table -->
        <div class="add-tracker-wrapper">
            <button class="add-tracker-btn" @click="openAddTrackerPopup"
                :class="{ 'scaling-effect': trackers.length === 0 }">
                <i class="fas fa-plus"></i>&nbsp; Add Tracker
            </button>
        </div>

        <!-- Tracker Settings Popup -->
        <TrackerSettingsPopup v-if="showSettingsPopup" :selectedTrackerId="selectedTracker?._id"
            :trackerNameInitial="selectedTracker?.name" :trackerModeInitial="selectedTracker?.mode"
            :sendingFrequencyInitial="selectedTracker?.frequency" :template="user.settings?.template"
            :closePopup="closePopup" @updateTracker="handleUpdatedTracker" />

        <!-- Tracker Detail Popup -->
        <TrackerDetailPopup v-if="showInfoPopup" :tracker="selectedTracker" :template="user.settings?.template"
            :closePopup="closeInfoPopup" />

        <!-- Add Tracker Popup -->
        <AddTrackerPopup v-if="showAddTrackerPopup" :template="user.settings?.template"
            :closePopup="closeAddTrackerPopup" @tracker-added="handleTrackerAdded" />
    </div>
</template>

<script setup>
import { ref, onMounted, computed } from 'vue';
import { useAuthStore } from "@/stores/auth";
import { useApiPrivate } from "@/composables/useApi";
import TrackerSettingsPopup from './TrackerSettingsPopup.vue';
import TrackerDetailPopup from './TrackerDetailPopup.vue';
import AddTrackerPopup from './AddTrackerPopup.vue';

const authStore = useAuthStore();
const user = computed(() => authStore.userDetail);
const trackers = ref([]);

const showSettingsPopup = ref(false);
const showInfoPopup = ref(false);
const selectedTracker = ref(null);
const showAddTrackerPopup = ref(false);

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
                const modeResponse = await api.get(`http://localhost:3500/api/mode/${tracker._id}`);
                tracker.modeDetails = modeResponse.data;

                tracker.modeLabel = tracker.modeDetails.GnssMode
                    ? 'Real-Time-Tracking'
                    : tracker.modeDetails.CellInfosMode
                        ? 'Long-Time-Tracking'
                        : 'Unknown Mode';

                const measurementsResponse = await api.get(`http://localhost:3500/api/position/tracker/${tracker._id}`);
                const measurements = measurementsResponse.data;
                measurements.sort((a, b) => new Date(b.createdAt) - new Date(a.createdAt));

                tracker.detailsWithTimestamps = {
                    latitude: { value: 'N/A', timestamp: null },
                    longitude: { value: 'N/A', timestamp: null },
                    temperature: { value: 'N/A', timestamp: null },
                    humidity: { value: 'N/A', timestamp: null },
                    battery: { value: 'N/A', timestamp: null },
                };

                for (const measurement of measurements) {
                    const measurementDate = new Date(measurement.createdAt);
                    if (measurement.latitude && !isNaN(measurement.latitude)) {
                        if (!tracker.detailsWithTimestamps.latitude.timestamp || measurementDate > new Date(tracker.detailsWithTimestamps.latitude.timestamp)) {
                            tracker.detailsWithTimestamps.latitude = {
                                value: measurement.latitude,
                                timestamp: measurementDate.toLocaleString(),
                            };
                        }
                    }
                    if (measurement.longitude && !isNaN(measurement.longitude)) {
                        if (!tracker.detailsWithTimestamps.longitude.timestamp || measurementDate > new Date(tracker.detailsWithTimestamps.longitude.timestamp)) {
                            tracker.detailsWithTimestamps.longitude = {
                                value: measurement.longitude,
                                timestamp: measurementDate.toLocaleString(),
                            };
                        }
                    }
                    if (measurement.temperature) {
                        if (!tracker.detailsWithTimestamps.temperature.timestamp || measurementDate > new Date(tracker.detailsWithTimestamps.temperature.timestamp)) {
                            tracker.detailsWithTimestamps.temperature = {
                                value: `${measurement.temperature}°C`,
                                timestamp: measurementDate.toLocaleString(),
                            };
                        }
                    }
                    if (measurement.humidity) {
                        if (!tracker.detailsWithTimestamps.humidity.timestamp || measurementDate > new Date(tracker.detailsWithTimestamps.humidity.timestamp)) {
                            tracker.detailsWithTimestamps.humidity = {
                                value: `${measurement.humidity}%`,
                                timestamp: measurementDate.toLocaleString(),
                            };
                        }
                    }
                    if (measurement.battery) {
                        if (!tracker.detailsWithTimestamps.battery.timestamp || measurementDate > new Date(tracker.detailsWithTimestamps.battery.timestamp)) {
                            tracker.detailsWithTimestamps.battery = {
                                value: `${Math.round(measurement.battery)}%`,
                                timestamp: measurementDate.toLocaleString(),
                            };
                        }
                    }
                }

                tracker.location =
                    tracker.detailsWithTimestamps.latitude.value !== 'N/A' && tracker.detailsWithTimestamps.longitude.value !== 'N/A'
                        ? await getReverseGeocodingAddress(tracker.detailsWithTimestamps.latitude.value, tracker.detailsWithTimestamps.longitude.value)
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

const openSettingsPopup = (tracker) => {
    selectedTracker.value = tracker;
    showSettingsPopup.value = true;
};

const closePopup = () => {
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
        await authStore.getUser(); // Refresh user data
        await fetchTrackersWithMeasurements(); // Refresh the trackers list
    } catch (error) {
        console.error('Failed to refresh data:', error);
        alert('There was an issue refreshing the data. Please reload the page.');
    }
};

const startEditingName = (tracker) => {
    tracker.isEditingName = true;
    tracker.editingName = tracker.name; // Pre-populate the input with the current name
};

const saveTrackerName = async (tracker) => {
    tracker.isEditingName = false; // Disable the editing mode
    if (tracker.editingName && tracker.editingName !== tracker.name) {
        try {
            const api = useApiPrivate();
            await api.put(`http://localhost:3500/api/tracker/${tracker._id}`, { name: tracker.editingName });
            tracker.name = tracker.editingName; // Update the tracker name in the frontend
        } catch (error) {
            console.error('Failed to update the tracker name:', error);
        }
    }
};

onMounted(() => {
    authStore.getUser(); // Ensure user data is loaded
    fetchTrackersWithMeasurements(); // Load trackers with measurements when the component is mounted
});
</script>

<style scoped>
/* Styles for the tracker table and components */
.tracker-table {
    width: 100%;
    border-collapse: collapse;
    background-color: #f1e4cc;
}

.tracker-table th,
.tracker-table td {
    padding: 15px 20px;
    border-bottom: 1px solid #ddd;
    text-align: center;
    font-size: 1rem;
    border: 1px solid #00543D;
}

.tracker-table th {
    background-color: #f1e4cc;
    color: #000;
    font-weight: bold;
    text-transform: uppercase;
}

.tracker-table tr:nth-child(even) {
    background-color: #f5f5f5;
}

.tracker-table tr:hover {
    background-color: #e6cc99;
}

.settings-icon,
.info-icon {
    font-size: 1.2rem;
    cursor: pointer;
    margin-left: 8px;
}

.info-icon {
    color: #0062cc;
}

.settings-icon:hover,
.info-icon:hover {
    transform: scale(1.3);
}

.add-tracker-wrapper {
    display: flex;
    justify-content: center;
    margin-top: 20px;
}

.add-tracker-btn {
    background-color: #851515;
    color: #ddd;
    padding: 10px 20px;
    border: none;
    border-radius: 5px;
    cursor: pointer;
    font-size: 1rem;
    transition: transform 0.3s, box-shadow 0.3s;
}

.add-tracker-btn:hover {
    transform: scale(1.1);
}

/* Dark mode styles */
.dark-mode .tracker-table {
    background-color: #2e2e2e;
}

.dark-mode .tracker-table th {
    background-color: #333;
    color: #ddd;
    border-color: #555;
}

.dark-mode .tracker-table td {
    background-color: #3a3a3a;
    color: #ccc;
    border-color: #555;
}

.dark-mode .tracker-table tr:nth-child(even) {
    background-color: #444;
}

.dark-mode .tracker-table tr:hover {
    background-color: #555;
}

.dark-mode .settings-icon,
.dark-mode .info-icon {
    color: #E69543;
}

.dark-mode .add-tracker-btn {
    background-color: #E69543;
    color: #1f1f1f;
}
</style>
