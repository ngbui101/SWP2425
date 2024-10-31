<template>
    <div class="popup-overlay" @click.self="closePopup">
        <div class="popup-card" :class="[(template ?? 'default') === 'dark' ? 'dark-mode' : '']">
            <div class="popup-header">
                <h2>Add New Tracker</h2>
                <button class="close-btn" @click="closePopup">✖</button>
            </div>

            <div class="popup-body">
                <!-- Tracker Name Input -->
                <div class="popup-section">
                    <label for="tracker-name">Tracker Name</label>
                    <input type="text" id="tracker-name" v-model="trackerName" class="popup-input" />
                </div>

                <!-- IMEI Input -->
                <div class="popup-section">
                    <label for="tracker-imei">IMEI</label>
                    <input type="text" id="tracker-imei" v-model="trackerImei" class="popup-input" />
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
import { useApi, useApiPrivate } from "@/composables/useApi";

// Define props to accept data passed from the parent
const props = defineProps({
    template: String,           // Template value for dark mode
    closePopup: Function        // Function to close the popup
});

// Reactive variables for tracker name and IMEI
const trackerName = ref('');
const trackerImei = ref('');

// Save tracker to backend using a PUT request
const saveChanges = async () => {
    try {
        // Use useApiPrivate to set up an authorized API request
        const api = useApiPrivate();

        // Send PUT request to create a new tracker
        const response = await api.post('http://localhost:3500/api/tracker', {
            name: trackerName.value,
            imei: trackerImei.value
        });

        console.log('Tracker created successfully:', response.data);

        // Close popup and clear input fields
        props.closePopup();
        trackerName.value = '';
        trackerImei.value = '';

    } catch (error) {
        console.error('Failed to create tracker:', error);
        alert('Failed to create tracker. Please try again.');
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
    /* Button color */
}

.close-btn:hover {
    color: #750f0f;
    /* Darker hover color */
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
    background: #ddd;
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

/* Popup Footer */
.popup-footer {
    margin-top: 20px;
}

.popup-save-btn {
    background-color: #851515;
    /* Button color */
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
    /* Darker hover color */
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
