<template>
    <div class="popup-overlay" @click.self="closePopup">
      <div class="popup-card" :class="[(template ?? 'default') === 'dark' ? 'dark-mode' : '']">
        <div class="popup-header">
          <h2>{{ $t('TrackersettingsPopup-EditTrackerSettings') }}</h2>
          <button class="close-btn" @click="closePopup">✖</button>
        </div>
  
        <div class="popup-body">
          <!-- Track Name Editing -->
          <div class="popup-section">
            <label for="tracker-name">{{ $t('TrackersettingsPopup-TrackerName') }}</label>
            <input type="text" id="tracker-name" v-model="trackerName" class="popup-input" />
          </div>
  
          <!-- Mode Toggle -->
          <div class="popup-section">
            <h3>{{ $t('TrackersettingsPopup-TrackingMode') }}</h3>
            <div class="mode-toggle">
              <button :class="{ active: !isLongTimeTracking }" @click="setRealTimeTracking">
                {{ $t('TrackersettingsPopup-RealTimeTracking') }}
              </button>
              <button :class="{ active: isLongTimeTracking }" @click="setLongTimeTracking">
                {{ $t('TrackersettingsPopup-LongTimeTracking') }}
              </button>
            </div>
          </div>
  
          <!-- Battery Data Checkbox -->
          <div class="popup-section">
            <label>
              <input type="checkbox" v-model="sendBatteryData" />
              {{ $t('TrackersettingsPopup-SendBatteryData') }}
            </label>
          </div>
  
          <!-- Humidity Data Checkbox -->
          <div class="popup-section">
            <label>
              <input type="checkbox" v-model="sendTemperatureData" />
              {{ $t('TrackersettingsPopup-SendHumidityData') }}
            </label>
          </div>
  
          <!-- Frequency Slider for Real-Time Mode -->
          <div class="popup-section" v-if="!isLongTimeTracking">
            <h3>{{ $t('TrackersettingsPopup-RealTimeFrequency') }}</h3>
            <input
              type="range"
              class="frequency-slider"
              v-model.number="selectedRealTimeStep"
              :min="0"
              :max="realTimeSteps.length - 1"
              step="1"
            />
            <p>{{ formattedRealTimeInterval }}</p>
          </div>
  
          <!-- Frequency Slider for Long-Time Mode -->
          <div class="popup-section" v-else>
            <h3>{{ $t('TrackersettingsPopup-LongTimeFrequency') }}</h3>
            <input
              type="range"
              class="frequency-slider"
              v-model.number="trackingInterval"
              min="1"
              max="24"
              step="1"
            />
            <p>{{ trackingInterval }} {{ $t('TrackersettingsPopup-Hours') }}</p>
          </div>
        </div>
  
        <div class="popup-footer">
          <button class="popup-save-btn" @click="saveChanges">
            {{ $t('TrackersettingsPopup-Save') }}
          </button>
        </div>
      </div>
    </div>
  </template>

<script setup>
import { ref, computed, onMounted } from 'vue';
import { useApiPrivate } from "@/composables/useApi";
import { useAuthStore } from "@/stores/auth";

const props = defineProps({
    trackerNameInitial: String,
    trackerModeInitial: String,
    sendingFrequencyInitial: Number,
    template: String,
    closePopup: Function,
    selectedTrackerId: String,
});

const emit = defineEmits(['updateTracker']);

const trackerName = ref(props.trackerNameInitial);
const isLongTimeTracking = ref(props.trackerModeInitial === 'LT');
const trackingInterval = ref(props.sendingFrequencyInitial || 1); // Default to 1 hour if not provided
// Add new reactive variables for checkboxes
const sendBatteryData = ref(props.trackerModeInitial.BatteryMode || false);
const sendTemperatureData = ref(props.trackerModeInitial.TemperatureMode || false);
// Real-time mode frequency steps (in seconds)
const realTimeSteps = [5, 10, 20, 30, 60, 120, 300, 600, 1800]; // 5s, 10s, 20s, 30s, 1min, 2min, 5min, 10min, 30min
const selectedRealTimeStep = ref(0);

const api = useApiPrivate();
const store = useAuthStore();

// Computed property to format the real-time interval display
const formattedRealTimeInterval = computed(() => {
    const seconds = realTimeSteps[selectedRealTimeStep.value];
    return seconds < 60 ? `${seconds} seconds` : `${Math.round(seconds / 60)} min${seconds / 60 > 1 ? 's' : ''}`;
});

onMounted(async () => {
    try {
        if (!props.selectedTrackerId) {
            throw new Error('selectedTrackerId is undefined');
        }

        const { data } = await api.get(`http://localhost:3500/api/mode/${props.selectedTrackerId}`);

        // Populate the tracking mode
        isLongTimeTracking.value = data.CellInfosMode;
        if (data.GnssMode) {
            const frequency = data.frequenz / 1000; // Convert milliseconds to seconds
            const index = realTimeSteps.indexOf(frequency);
            selectedRealTimeStep.value = index !== -1 ? index : 0;
        } else {
            trackingInterval.value = Math.round(data.frequenz / (60 * 60 * 1000)); // Convert milliseconds to hours
        }

        // Populate BatteryMode and TemperatureMode
        sendBatteryData.value = data.BatteryMode || false;
        sendTemperatureData.value = data.TemperatureMode || false;
    } catch (error) {
        console.error('Failed to fetch tracker mode:', error);
    }
});


const setRealTimeTracking = () => {
    isLongTimeTracking.value = false;
    selectedRealTimeStep.value = 0; // Reset to 5 seconds by default
};

const setLongTimeTracking = () => {
    isLongTimeTracking.value = true;
};

const saveChanges = async () => {
    try {
        if (!props.selectedTrackerId) {
            throw new Error('selectedTrackerId is undefined');
        }

        // Update tracker name if changed
        if (trackerName.value !== props.trackerNameInitial) {
            await api.put(`http://localhost:3500/api/tracker/${props.selectedTrackerId}`, { name: trackerName.value });
            console.log('Tracker name updated successfully');
        }

        // Prepare mode update payload
        const realTimeFrequency = realTimeSteps[selectedRealTimeStep.value];

        const updateData = {
            GnssMode: !isLongTimeTracking.value,
            CellInfosMode: isLongTimeTracking.value,
            BatteryMode: sendBatteryData.value, // Include battery mode
            TemperatureMode: sendTemperatureData.value, // Include temperature mode
            frequenz: isLongTimeTracking.value
                ? trackingInterval.value * 60 * 60 * 1000 // Long-Time Mode frequency in milliseconds
                : realTimeFrequency * 1000, // Real-Time Mode frequency in milliseconds
        };

        console.log('Update Data:', updateData); // Debug the payload

        await api.put(`http://localhost:3500/api/mode/${props.selectedTrackerId}`, updateData);

        console.log('Mode updated successfully');

        // Emit updated data to parent
        emit('updateTracker', {
            id: props.selectedTrackerId,
            name: trackerName.value,
            mode: isLongTimeTracking.value ? 'LT' : 'RT',
            frequency: updateData.frequenz,
            BatteryMode: sendBatteryData.value,
            TemperatureMode: sendTemperatureData.value,
        });

        props.closePopup();
    } catch (error) {
        console.error('Failed to save changes:', error);
    }
};


</script>


<style scoped>
/* Overlay background for the popup */
.popup-overlay {
    position: fixed;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background-color: rgba(0, 0, 0, 0.5);
    display: flex;
    justify-content: center;
    align-items: center;
    z-index: 1000;
}

.slider-steps {
    font-size: 0.9rem;
    color: #555;
}


/* Popup Card */
.popup-card {
    background: linear-gradient(135deg, #f1e4cc 0%, #e6cc99 50%, #f1e4cc 100%);
    padding: 20px;
    border: 1px solid #ddd;
    border-radius: 12px;
    width: 320px;
    max-width: 90vw;
    box-shadow: 0 5px 10px rgba(0, 0, 0, 1);
    text-align: center;
    transition: transform 0.3s, box-shadow 0.3s;
    position: relative;
}

.popup-card input[type="range"] {
    width: 100%;
    accent-color: #00543D;
}

.dark-mode .popup-card input[type="range"] {
    accent-color: #E69543;
}

@media screen and (min-width: 768px) {
    .popup-card {
        width: 400px;
    }
}

.dark-mode.popup-card {
    background: linear-gradient(135deg, #1e1e1e 0%, #141414 50%, #1e1e1e 100%);
    border-color: #555;
    color: #bbb;
    box-shadow: 0 4px 8px rgba(255, 255, 255, 0.1);
}

/* Popup Header */
.popup-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 10px;
}

.popup-header h2 {
    font-size: 1.5rem;
    margin: 0;
}

.close-btn {
    background: none;
    border: none;
    font-size: 1.5rem;
    cursor: pointer;
    color: #851515;
}

.close-btn:hover {
    color: #750f0f;
}

.dark-mode .close-btn {
    color: #E69543;
}

.dark-mode .close-btn:hover {
    color: #cc7e36;
}

/* Popup Body */
.popup-body {
    display: flex;
    flex-direction: column;
    gap: 15px;
}

.popup-section {
    text-align: left;
}

.popup-section label {
    font-size: 0.9rem;
    display: block;
    margin-bottom: 5px;
}

.popup-input {
    width: 100%;
    padding: 10px;
    font-size: 1rem;
    border-radius: 6px;
    border: 1px solid #ddd;
    outline: none;
    background: #ffffff;
    box-sizing: border-box;
}

.popup-input:focus {
    border-color: #009579;
    box-shadow: 0 0 5px rgba(0, 149, 121, 0.5);
}

.dark-mode .popup-input {
    background-color: #444;
    border-color: #777;
    color: #bbb;
}

/* Mode Toggle */
.mode-toggle {
    display: flex;
    border: 1px solid #333;
    border-radius: 20px;
    overflow: hidden;
    margin-top: 10px;
}

.dark-mode .mode-toggle {
    border: 1px solid #ddd;
}

.mode-toggle button {
    flex: 1;
    padding: 10px;
    font-weight: bold;
    background-color: #ddd;
    border: none;
    cursor: pointer;
    transition: background-color 0.3s;
}

.mode-toggle button.active {
    background-color: #C19A6B;
    color: #1f1f1f;
}

.dark-mode .mode-toggle button.active {
    background-color: #87c099;
    color: #333;
}

.mode-toggle button:not(.active) {
    background-color: #fff;
    color: #333;
}

.dark-mode .mode-toggle button:not(.active) {
    background-color: #2e2e2e;
    color: #fff;
}

.mode-toggle button:not(.active):hover {
    background-color: #ccc;
    color: #333;
}

/* Frequency Display */
.frequency-display p,
.frequency-slider p {
    font-size: 1.2rem;
    font-weight: bold;
}

.popup-body input[type="checkbox"] {
    accent-color: #00543D;
    /* Orange color */
}

/* Dark mode checkbox accent color */
.dark-mode .popup-body input[type="checkbox"] {
    accent-color: #E69543;
    /* Orange color */
}

.frequency-slider input[type="range"] {
    width: 100%;
    accent-color: #00543D;
}

.dark-mode .frequency-slider input[type="range"] {
    width: 100%;
    accent-color: #E69543;
}

/* Popup Footer */
.popup-footer {
    display: flex;
    justify-content: flex-end;
    gap: 10px;
    margin-top: 20px;
}

/* Save Button */
.popup-save-btn {
    background-color: #851515;
    color: #fff;
    padding: 10px 20px;
    font-size: 1rem;
    font-weight: bold;
    border-radius: 6px;
    border: none;
    cursor: pointer;
    transition: background-color 0.3s, transform 0.2s;
}

.popup-save-btn:hover {
    background-color: #750f0f;
    transform: scale(1.05);
}

.dark-mode .popup-save-btn {
    background-color: #E69543;
    color: #1f1f1f;
}

.dark-mode .popup-save-btn:hover {
    background-color: #cc7e36;
}
</style>
