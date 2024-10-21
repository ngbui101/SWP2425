<template>
    <div class="card-view" :class="[(user.template ?? 'default') === 'dark' ? 'dark-mode' : '']">
        <div class="tracker-card" v-for="tracker in trackers" :key="tracker._id">
            <!-- Settings Icon at the top-right corner -->
            <div class="settings-icon" @click="openSettingsPopup(tracker)">
                <i class="fas fa-cog"></i>
            </div>

            <!-- Battery Bar above the card title -->
            <div class="battery-wrapper">
                <div class="battery-bar">
                    <div class="battery-fill"
                        :style="{ width: `${tracker.latestMeasurement ? tracker.latestMeasurement.battery || 89 : 89}%` }">
                    </div>
                </div>
                <div class="battery-indicator">
                    <span>{{ tracker.latestMeasurement ? Math.round(tracker.latestMeasurement.battery) || 'N/A' : 'N/A'
                        }}%</span>
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
                <p><strong>Mode:</strong> {{ tracker.mode }}</p>
                <p><strong>Location:</strong> {{ tracker.location }}</p>
                <p><strong>Latitude:</strong> {{ tracker.latestMeasurement ? tracker.latestMeasurement.latitude || 'N/A'
                    : 'N/A' }}</p>
                <p><strong>Longitude:</strong> {{ tracker.latestMeasurement ? tracker.latestMeasurement.longitude ||
                    'N/A' : 'N/A' }}</p>
                <p><strong>Temp:</strong> {{ tracker.latestMeasurement ? tracker.latestMeasurement.temperature || 'N/A'
                    : 'N/A' }}°C</p>
                <p><strong>Humidity:</strong> {{ tracker.latestMeasurement ? tracker.latestMeasurement.humidity || 'N/A'
                    : 'N/A' }}%</p>
                <p><strong>Device ID:</strong> {{ tracker.imei }}</p>
            </div>
        </div>


        <!-- Add Tracker as a card -->
        <div class="tracker-card add-tracker-card" @click="openAddTrackerPopup"
            :class="{ 'scaling-effect': trackers.length === 0 }">
            <div class="card-body add-tracker-body">
                <i class="fas fa-plus"></i>&nbsp; Add Tracker
            </div>
        </div>
        <!-- Tracker Settings Popup -->
        <TrackerSettingsPopup v-if="showSettingsPopup" :trackerNameInitial="selectedTracker.name"
            :trackerModeInitial="selectedTracker.mode" :template="user.template" :closePopup="closePopup" />
        <!-- Add Tracker Popup -->
        <AddTrackerPopup v-if="showAddTrackerPopup" :template="user.template" :closePopup="closeAddTrackerPopup" />
    </div>
</template>

<script setup>
import { ref } from 'vue';
import { useAuthStore } from "@/stores/auth";
import { useApi, useApiPrivate } from "@/composables/useApi";
import TrackerSettingsPopup from './TrackerSettingsPopup.vue';
import AddTrackerPopup from './AddTrackerPopup.vue';

// Define props received from parent component
defineProps({
    trackers: Array, // Array of trackers passed from the parent
    addTracker: Function, // Function to add a new tracker
    user: Object, // The user object passed from the parent
});
const showSettingsPopup = ref(false);
const showAddTrackerPopup = ref(false);
const selectedTracker = ref(null);
const openSettingsPopup = (tracker) => {
    selectedTracker.value = tracker;
    showSettingsPopup.value = true;
};

// Function to close the settings popup
const closePopup = () => {
    showSettingsPopup.value = false;
    selectedTracker.value = null;
};
// Function to open the Add Tracker Popup
const openAddTrackerPopup = () => {
    showAddTrackerPopup.value = true;
};

// Function to close the Add Tracker Popup
const closeAddTrackerPopup = () => {
    showAddTrackerPopup.value = false;
};

// Start editing the tracker's name
const startEditingName = (tracker) => {
    tracker.isEditingName = true;
    tracker.editingName = tracker.name; // Pre-populate the input with the current name
};

// Save the new tracker name and update it in the backend
const saveTrackerName = async (tracker) => {
    tracker.isEditingName = false; // Disable the editing mode

    // Only proceed with the API call if the name has changed
    if (tracker.editingName && tracker.editingName !== tracker.name) {
        try {
            const { data } = await updateTrackerName(tracker._id, tracker.editingName);
            tracker.name = tracker.editingName; // Update the tracker name in the frontend
        } catch (error) {
            console.error("Failed to update the tracker name:", error);
        }
    }
};

// Update the tracker name in the backend
const updateTrackerName = async (trackerId, newName) => {
    try {
        const { data } = await useApiPrivate().put(`http://localhost:3500/api/tracker/${trackerId}`, {
            name: newName
        });
        return data;
    } catch (error) {
        throw new Error(`Failed to update tracker name: ${error.message}`);
    }
};
</script>

<style scoped>
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
    background: rgba(224, 224, 224, 0.9);
    padding: 20px;
    border: 1px solid #ddd;
    border-radius: 12px;
    width: 260px;
    box-shadow: 0 5px 10px rgba(0, 0, 0, 1);
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
    background: rgba(224, 224, 224, 0.9);
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
