<template>
    <div class="popup-overlay" @click.self="closePopup">
        <div class="popup-card" :class="[(template ?? 'default') === 'dark' ? 'dark-mode' : '']">
            <div class="popup-header">
                <h2>Edit Tracker Settings(NO FUNCTIONALITY YET)</h2>
                <button class="close-btn" @click="closePopup">✖</button>
            </div>

            <div class="popup-body">
                <!-- Track Name Editing -->
                <div class="popup-section">
                    <label for="tracker-name">Tracker Name</label>
                    <input type="text" id="tracker-name" v-model="trackerName" class="popup-input" />
                </div>

                <!-- Mode Dropdown -->
                <div class="popup-section">
                    <label for="tracker-mode">Mode</label>
                    <select id="tracker-mode" v-model="trackerMode" class="popup-dropdown">
                        <option value="RT">RT</option>
                        <option value="LT">LT</option>
                    </select>
                </div>

                <!-- Sending Frequency Dropdown -->
                <div class="popup-section">
                    <label for="sending-frequency">Sending Frequency</label>
                    <select id="sending-frequency" v-model="sendingFrequency" class="popup-dropdown">
                        <option value="5">5 min</option>
                        <option value="10">10 min</option>
                        <option value="60">60 min</option>
                        <option value="720">12 h</option>
                        <option value="1440">24 h</option>
                    </select>
                </div>

                <!-- Geofence Toggle -->
                <div class="popup-section">
                    <label for="geofence-toggle">Geofence</label>
                    <div class="toggle-container">
                        <div class="toggle-switch" :class="{ active: geofenceActive }"
                            @click="geofenceActive = !geofenceActive">
                            <div class="toggle-handle" :class="{ active: geofenceActive }"></div>
                        </div>
                        <span class="toggle-text">{{ geofenceActive ? 'ON' : 'OFF' }}</span>
                    </div>
                </div>

                <!-- Motion Sensor Toggle -->
                <div class="popup-section">
                    <label for="motion-sensor-toggle">Motion Sensor</label>
                    <div class="toggle-container">
                        <div class="toggle-switch" :class="{ active: motionSensorActive }"
                            @click="motionSensorActive = !motionSensorActive">
                            <div class="toggle-handle" :class="{ active: motionSensorActive }"></div>
                        </div>
                        <span class="toggle-text">{{ motionSensorActive ? 'ON' : 'OFF' }}</span>
                    </div>
                </div>
            </div>

            <div class="popup-footer">
                <button class="popup-save-btn" @click="saveChanges">Save</button>
            </div>
        </div>
    </div>
</template>

<script setup>
import { ref } from 'vue';

const props = defineProps({
    trackerNameInitial: String,
    trackerModeInitial: String,
    geofenceInitial: Boolean,
    motionSensorInitial: Boolean,
    sendingFrequencyInitial: Number,
    template: String,
    closePopup: Function
});

const trackerName = ref(props.trackerNameInitial);
const trackerMode = ref(props.trackerModeInitial);
const geofenceActive = ref(props.geofenceInitial);
const motionSensorActive = ref(props.motionSensorInitial);
const sendingFrequency = ref(props.sendingFrequencyInitial);

const saveChanges = () => {
    console.log('Saving:', trackerName.value, trackerMode.value, geofenceActive.value, motionSensorActive.value, sendingFrequency.value);
    props.closePopup();
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

/* Toggle Container */
.toggle-container {
    display: flex;
    align-items: center;
    gap: 10px;
}

.toggle-text {
    font-size: 1rem;
    font-weight: bold;
    color: #333;
}

.dark-mode .toggle-text {
    color: #bbb;
}

/* Toggle Switch */
.toggle-switch {
    width: 40px;
    height: 20px;
    background-color: #ddd;
    border-radius: 12px;
    position: relative;
    cursor: pointer;
    transition: background-color 0.3s;
}

.toggle-switch.active {
    background-color: #009579;
}

.toggle-handle {
    width: 18px;
    height: 18px;
    background-color: white;
    border-radius: 50%;
    position: absolute;
    top: 1px;
    left: 1px;
    transition: left 0.3s;
}

.toggle-handle.active {
    left: 21px;
}

.dark-mode .toggle-switch {
    background-color: #555;
}

.dark-mode .toggle-switch.active {
    background-color: #E69543;
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

.popup-input,
.popup-dropdown {
    width: 100%;
    padding: 10px;
    font-size: 1rem;
    border-radius: 6px;
    border: 1px solid #ddd;
    outline: none;
    background: #ddd;
}

.popup-input:focus,
.popup-dropdown:focus {
    border-color: #009579;
    box-shadow: 0 0 5px rgba(0, 149, 121, 0.5);
}

.dark-mode .popup-input,
.dark-mode .popup-dropdown {
    background-color: #444;
    border-color: #777;
    color: #bbb;
}

/* Popup Footer */
.popup-footer {
    margin-top: 20px;
}

.popup-save-btn {
    background-color: #851515;
    color: #fff;
    padding: 12px 30px;
    font-size: 1rem;
    font-weight: bold;
    border-radius: 6px;
    border: none;
    cursor: pointer;
    transition: background-color 0.3s ease, transform 0.2s;
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
