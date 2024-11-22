<template>
    <div class="popup-overlay" @click.self="closePopup">
        <div class="popup-card" :class="[(template ?? 'default') === 'dark' ? 'dark-mode' : '']">
            <div class="popup-header">
                <h2>Measurement Details</h2>
                <button class="close-btn" @click="closePopup">✖</button>
            </div>

            <div class="popup-body">
                <!-- Display tracker details with timestamps on the same line -->
                <div class="details-section">
                    <p>
                        <strong>Latitude:</strong>
                        <span class="detail">{{ tracker.detailsWithTimestamps?.latitude?.value || 'N/A' }}</span>
                        <span class="timestamp">{{ tracker.detailsWithTimestamps?.latitude?.timestamp || 'N/A' }}</span>
                    </p>
                    <p>
                        <strong>Longitude:</strong>
                        <span class="detail">{{ tracker.detailsWithTimestamps?.longitude?.value || 'N/A' }}</span>
                        <span class="timestamp">{{ tracker.detailsWithTimestamps?.longitude?.timestamp || 'N/A'
                            }}</span>
                    </p>
                    <p>
                        <strong>Temperature:</strong>
                        <span class="detail">{{ tracker.detailsWithTimestamps?.temperature?.value || 'N/A' }}</span>
                        <span class="timestamp">{{ tracker.detailsWithTimestamps?.temperature?.timestamp || 'N/A'
                            }}</span>
                    </p>
                    <p>
                        <strong>Humidity:</strong>
                        <span class="detail">{{ tracker.detailsWithTimestamps?.humidity?.value || 'N/A' }}</span>
                        <span class="timestamp">{{ tracker.detailsWithTimestamps?.humidity?.timestamp || 'N/A' }}</span>
                    </p>
                    <p>
                        <strong>Battery:</strong>
                        <span class="detail">{{ tracker.detailsWithTimestamps?.battery?.value || 'N/A' }}</span>
                        <span class="timestamp">{{ tracker.detailsWithTimestamps?.battery?.timestamp || 'N/A' }}</span>
                    </p>
                </div>
            </div>
        </div>
    </div>
</template>

<script setup>
import { defineProps } from 'vue';

// Define props to accept data passed from the parent
const props = defineProps({
    tracker: {
        type: Object,
        required: true,
        default: () => ({
            detailsWithTimestamps: {
                latitude: { value: 'N/A', timestamp: null },
                longitude: { value: 'N/A', timestamp: null },
                temperature: { value: 'N/A', timestamp: null },
                humidity: { value: 'N/A', timestamp: null },
                battery: { value: 'N/A', timestamp: null },
            },
        }),
    },
    template: String, // Template value for dark mode
    closePopup: Function, // Function to close the popup
});
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
    text-align: left;
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
    margin-bottom: 15px;
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
    gap: 10px;
}

/* Details Section */
.details-section p {
    margin: 5px 0;
    font-size: 1rem;
    display: flex;
    justify-content: space-between;
    align-items: center;
}

.details-section .detail {
    text-align: center;
    /* Center the detail in the middle column */
}

.details-section .timestamp {
    color: #444;
    /* Default timestamp color */
    font-size: 0.9rem;
}

.dark-mode .details-section .timestamp {
    color: #E69543;
    /* Orange color for dark mode */
}
</style>
