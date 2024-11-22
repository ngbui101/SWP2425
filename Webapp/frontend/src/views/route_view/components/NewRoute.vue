<template>
    <div :class="['container', (user.settings?.template ?? 'default') === 'dark' ? 'dark-mode' : '']">
        <div class="mapview-container">
            <div v-if="trackers.length === 0" class="overlay">
                <p class="overlay-text">No trackers available. Please add a tracker.</p>
            </div>

            <!-- Start/Stop and Save Route controls -->
            <div class="route-controls">
                <button v-if="!isRouteActive" @click="startRoute" class="start-route-button">
                    {{ $t('NewRoute-StartRoute') }}
                </button>

                <div v-if="isRouteActive" class="timer-container">
                    <p>{{ formattedTime }}</p>
                    <button @click="stopRoute" class="stop-route-button">
                        {{ $t('NewRoute-StopRoute') }}
                    </button>
                </div>

                <button v-if="showSaveButton" @click="saveRoute" class="save-route-button">
                    {{ $t('NewRoute-SaveRoute') }}
                </button>
            </div>
        </div>

        <!-- Map Card -->
        <div v-if="trackers.length > 0" class="card">
            <div class="map-container">
                <div ref="mapElement" class="map"></div>
                <div v-if="(user.settings?.template ?? 'default') === 'dark'" class="map-overlay"></div>
            </div>
        </div>
    </div>
</template>

<script setup>
import '../../map_view/components/styles_currentmap.css';
import { ref, computed, onMounted, watch } from 'vue';
import axios from 'axios';
import { useAuthStore } from "@/stores/auth";

// Store references
const authStore = useAuthStore();
const user = computed(() => authStore.userDetail);

// Tracker-related state
const trackers = ref([]);
const selectedTracker = ref(null);

// Button state
const buttonLabel = ref("Start");
const showSaveButton = ref(false);
const timerVisible = ref(false);
const timer = ref(0);
let timerInterval = null;

// Fetch trackers for the user
const fetchTrackersForUser = async () => {
    try {
        const token = authStore.accessToken;
        const config = { headers: { Authorization: `Bearer ${token}` } };
        const response = await axios.get('http://localhost:3500/api/tracker/user/', config);
        trackers.value = response.data;

        // Automatically select the first tracker if available
        if (trackers.value.length > 0 && !selectedTracker.value) {
            selectedTracker.value = trackers.value[0]._id;
        }
    } catch (error) {
        console.error('Failed to fetch trackers:', error);
    }
};

// Start/Stop/Save button logic
const handleStartStop = () => {
    if (buttonLabel.value === "Start") {
        startRoute();
    } else if (buttonLabel.value === "Stop") {
        stopRoute();
    }
};

// Start route logic
const startRoute = () => {
    buttonLabel.value = "Stop";
    timerVisible.value = true;
    timer.value = 0;

    // Start the timer
    timerInterval = setInterval(() => {
        timer.value += 1;
    }, 1000);
};

// Stop route logic
const stopRoute = () => {
    buttonLabel.value = "Start";
    clearInterval(timerInterval);
    timerInterval = null;
    timerVisible.value = false;
    showSaveButton.value = true;
};

// Save route logic
const saveRoute = () => {
    // Implement saving logic here
    console.log("Route saved!");
    showSaveButton.value = false;
};

// Timer formatting
const formattedTime = computed(() => {
    const minutes = Math.floor(timer.value / 60);
    const seconds = timer.value % 60;
    return `${minutes.toString().padStart(2, "0")}:${seconds.toString().padStart(2, "0")}`;
});

// Fetch trackers on mount
onMounted(fetchTrackersForUser);

const filteredTrackers = computed(() => {
    return trackers.value;
});
</script>

<style scoped>
.tracker-info-card {
    background: #f8f9fa;
    padding: 20px;
    border: 1px solid #ddd;
    border-radius: 8px;
}

.dark-mode .tracker-info-card {
    background: #2e2e2e;
    border-color: #555;
    color: #bbb;
}

.dropdown-label {
    font-weight: bold;
    margin-bottom: 10px;
}

.tracker-dropdown {
    margin-left: 10px;
    padding: 5px;
    border-radius: 4px;
}

.filters-button {
    margin-left: 10px;
    padding: 5px 10px;
    background-color: #ff9800;
    color: white;
    border: none;
    border-radius: 4px;
    cursor: pointer;
}

.filters-button:hover {
    background-color: #e68a00;
}

.action-buttons {
    margin-top: 20px;
    display: flex;
    gap: 10px;
}

.action-button {
    padding: 10px 20px;
    font-size: 1rem;
    background-color: #28a745;
    color: white;
    border: none;
    border-radius: 4px;
    cursor: pointer;
}

.action-button:disabled {
    background-color: #ddd;
    cursor: not-allowed;
}

.save-button {
    padding: 10px 20px;
    font-size: 1rem;
    background-color: #007bff;
    color: white;
    border: none;
    border-radius: 4px;
    cursor: pointer;
}

.save-button:hover {
    background-color: #0056b3;
}

.timer {
    margin-top: 15px;
    font-size: 1.2rem;
    font-weight: bold;
}
</style>