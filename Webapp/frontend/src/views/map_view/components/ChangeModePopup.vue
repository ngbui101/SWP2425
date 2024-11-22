<template>
    <div class="popup-overlay" @click.self="closePopup">
        <div class="popup-card" :class="[(template ?? 'default') === 'dark' ? 'dark-mode' : '']">
            <div class="popup-header">
                <h2>Change Tracking Mode</h2>
                <button class="close-btn" @click="closePopup">✖</button>
            </div>

            <div class="popup-body">
                <!-- Mode Toggle -->
                <div class="popup-section">
                    <h3>Tracking Mode</h3>
                    <div class="mode-toggle">
                        <button :class="{ active: !isLongTimeTracking }" @click="setRealTimeTracking">
                            Real-Time-Tracking
                        </button>
                        <button :class="{ active: isLongTimeTracking }" @click="setLongTimeTracking">
                            Long-Time-Tracking
                        </button>
                    </div>
                </div>

                <!-- Frequency Slider for Real-Time Mode -->
                <div class="popup-section" v-if="!isLongTimeTracking">
                    <h3>Sending Frequency (Real-Time)</h3>
                    <input type="range" v-model.number="selectedRealTimeStep" :min="0" :max="realTimeSteps.length - 1"
                        step="1" />
                    <p>{{ formattedRealTimeInterval }}</p>
                </div>

                <!-- Frequency Slider for Long-Time Mode -->
                <div class="popup-section" v-else>
                    <h3>Sending Frequency (Long-Time)</h3>
                    <input type="range" v-model.number="trackingInterval" min="1" max="24" step="1" />
                    <p>{{ trackingInterval }} hour(s)</p>
                </div>
            </div>

            <div class="popup-footer">
                <button class="popup-save-btn" @click="applyChanges">{{ $t('CurrentMap-apply') }}</button>
            </div>
        </div>
    </div>
</template>

<script setup>
import { ref, computed, onMounted } from 'vue';
import { useAuthStore } from "@/stores/auth";
import { useApiPrivate } from "@/composables/useApi";

const store = useAuthStore();
const api = useApiPrivate();

const props = defineProps({
    closePopup: Function,
    template: String,
    selectedTrackerId: String,
});

const isLongTimeTracking = ref(false);
const trackingInterval = ref(1); // Default interval of 1 hour for long-time tracking
const realTimeSteps = [5, 10, 20, 30, 60, 120, 300, 600, 1800]; // Real-time frequency options (in seconds)
const selectedRealTimeStep = ref(0);

const formattedRealTimeInterval = computed(() => {
    const seconds = realTimeSteps[selectedRealTimeStep.value];
    return seconds < 60 ? `${seconds} seconds` : `${Math.round(seconds / 60)} min${seconds / 60 > 1 ? 's' : ''}`;
});

// Fetch current mode and frequency details when the component is mounted
onMounted(async () => {
    try {
        if (!props.selectedTrackerId) {
            throw new Error('selectedTrackerId is undefined');
        }

        // Fetch the mode details for the selected tracker
        const { data } = await api.get(`http://localhost:3500/api/mode/${props.selectedTrackerId}`);
        isLongTimeTracking.value = data.CellInfosMode;

        if (data.GnssMode) {
            const frequency = data.frequenz / 1000; // Convert milliseconds to seconds
            const index = realTimeSteps.indexOf(frequency);
            selectedRealTimeStep.value = index !== -1 ? index : 0;
        } else if (data.CellInfosMode) {
            trackingInterval.value = Math.round(data.frequenz / (60 * 60 * 1000)); // Convert milliseconds to hours
        }
    } catch (error) {
        console.error('Failed to fetch tracker mode details:', error);
    }
});

const setRealTimeTracking = () => {
    isLongTimeTracking.value = false;
    selectedRealTimeStep.value = 0; // Reset to default 5 seconds
};

const setLongTimeTracking = () => {
    isLongTimeTracking.value = true;
};

const applyChanges = async () => {
    try {
        const realTimeFrequency = realTimeSteps[selectedRealTimeStep.value];
        await store.updateTrackerMode(
            props.selectedTrackerId,
            !isLongTimeTracking.value,
            isLongTimeTracking.value ? undefined : realTimeFrequency,
            isLongTimeTracking.value ? trackingInterval.value : undefined
        );
        console.log("Mode updated successfully");
        props.closePopup();
    } catch (error) {
        console.error("Failed to update mode:", error);
    }
};
</script>

<style scoped>
/* Popup Overlay */
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
    position: relative;
    transition: transform 0.3s, box-shadow 0.3s;
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

.popup-section h3 {
    font-size: 1rem;
    margin-bottom: 5px;
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
input[type="range"] {
    width: 100%;
    accent-color: #00543D;
}

.dark-mode input[type="range"] {
    accent-color: #E69543;
}

/* Popup Footer */
.popup-footer {
    display: flex;
    justify-content: flex-end;
    gap: 10px;
    margin-top: 20px;
}

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
