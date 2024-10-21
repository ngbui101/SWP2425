<template>
    <div :class="(user.template ?? 'default') === 'dark' ? 'dark-mode' : ''">
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
                    <th>Settings</th>
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
                    <td>{{ tracker.mode }}</td>
                    <td>{{ tracker.location }}</td>
                    <td>{{ tracker.latestMeasurement ? tracker.latestMeasurement.latitude || 'N/A' : 'N/A' }}</td>
                    <td>{{ tracker.latestMeasurement ? tracker.latestMeasurement.longitude || 'N/A' : 'N/A' }}</td>
                    <td>{{ tracker.latestMeasurement ? tracker.latestMeasurement.temperature || 'N/A' : 'N/A' }}°C</td>
                    <td>{{ tracker.latestMeasurement ? tracker.latestMeasurement.humidity || 'N/A' : 'N/A' }}%</td>
                    <td>{{ tracker.latestMeasurement ? Math.round(tracker.latestMeasurement.battery) || 'N/A' : 'N/A'
                        }}%
                    </td>
                    <td>{{ tracker.imei || 'N/A' }}</td>
                    <td>
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
        <TrackerSettingsPopup v-if="showSettingsPopup" :trackerNameInitial="selectedTracker.name"
            :trackerModeInitial="selectedTracker.mode" :template="user.template" :closePopup="closePopup" />

        <!-- Add Tracker Popup -->
        <AddTrackerPopup v-if="showAddTrackerPopup" :template="user.template" :closePopup="closeAddTrackerPopup" />
    </div>
</template>


<script setup>
import { ref } from 'vue';
import { useApi, useApiPrivate } from "@/composables/useApi";
import TrackerSettingsPopup from './TrackerSettingsPopup.vue';
import AddTrackerPopup from './AddTrackerPopup.vue'; // Import AddTrackerPopup
// Define props received from parent component
defineProps({
    trackers: Array, // Array of trackers passed from the parent
    addTracker: Function, // Function to add a new tracker
    user: Object, // The user object passed from the parent
});

const showSettingsPopup = ref(false);
const selectedTracker = ref(null);

const openSettingsPopup = (tracker) => {
    selectedTracker.value = tracker;
    showSettingsPopup.value = true;
};

// State for Add Tracker Popup
const showAddTrackerPopup = ref(false);

// Function to open the Add Tracker Popup
const openAddTrackerPopup = () => {
    showAddTrackerPopup.value = true;
};

// Function to close the Add Tracker Popup
const closeAddTrackerPopup = () => {
    showAddTrackerPopup.value = false;
};

// Function to close the settings popup
const closePopup = () => {
    showSettingsPopup.value = false;
    selectedTracker.value = null;
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
/* General tracker table styles */
.tracker-table {
    width: 100%;
    border-collapse: collapse;
    background-color: #f1e4cc;
    overflow: hidden;
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

.tracker-table tr {
    background-color: #ddd;
}


.settings-icon {
    font-size: 1.2rem;
    color: #333;
    cursor: pointer;
    color: #851515;
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

}

.add-tracker-btn:hover {
    transform: scale(1.1);
}

.name-input {
    background-color: transparent;
    border: none;
    font-size: 1rem;
    text-align: center;
    width: 100%;
    outline: none;
    color: inherit;
}

.name-input:focus {
    border-bottom: 1px solid #555;
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

.dark-mode .settings-icon {
    color: #E69543;
}

.dark-mode .add-tracker-btn {
    background-color: #E69543;
    color: #1f1f1f;

}
</style>
