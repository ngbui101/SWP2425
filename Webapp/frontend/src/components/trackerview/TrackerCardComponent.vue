<template>
    <div class="card-view">
        <div class="tracker-card" v-for="tracker in trackers" :key="tracker._id">
            <!-- Settings Icon at the top-right corner -->
            <div class="settings-icon">
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
                    <span>{{ tracker.latestMeasurement ? tracker.latestMeasurement.battery || '89' : '89' }}%</span>
                </div>
            </div>

            <div class="card-header">
                <h3>{{ tracker.name }}</h3>
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

        <!-- Add Tracker Button below the card view -->
        <div class="add-tracker-wrapper">
            <button class="add-tracker-btn" @click="addTracker">
                <i class="fas fa-plus"></i> Add Tracker
            </button>
        </div>
    </div>
</template>

<script setup>
defineProps({
    trackers: Array, // Props passed from parent
    addTracker: Function, // Function to add a tracker
});
</script>


<style scoped>
.card-view {
    display: flex;
    flex-wrap: wrap;
    gap: 20px;
    justify-content: center;
}

.tracker-card {
    background-color: #f1f1f1;
    padding: 20px;
    border: 1px solid #ddd;
    border-radius: 12px;
    width: 260px;
    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
    text-align: center;
    transition: transform 0.3s, box-shadow 0.3s;
    position: relative;
    border: 1px solid #00543D;
}

.tracker-card:hover {
    transform: translateY(-5px);
    box-shadow: 0 6px 12px rgba(0, 0, 0, 0.2);
}

/* Settings Icon */
.settings-icon {
    position: absolute;
    top: 10px;
    right: 10px;
    font-size: 18px;
    color: #555;
    cursor: pointer;
}

.settings-icon:hover {
    color: #000;
}

/* Battery Bar Wrapper */
.battery-wrapper {
    display: flex;
    justify-content: center;
    align-items: center;
    margin-bottom: 10px;
    position: relative;
}

/* Battery Bar (Phone-like) */
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

/* Battery Fill (Green) */
.battery-fill {
    background-color: #28a745;
    height: 100%;
    transition: width 0.3s;
}

/* Battery Indicator Text */
.battery-indicator {
    font-size: 0.8rem;
    font-weight: bold;
    color: #333;
}

.card-header {
    margin-top: 10px;
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

/* Add Tracker Button */
.add-tracker-wrapper {
    display: flex;
    justify-content: center;
    margin-top: 10px;
    /* Reduce the margin above the button */
}

.add-tracker-btn {
    background-color: #28a745;
    color: white;
    padding: 4px 10px;
    /* Minimal padding */
    border: none;
    border-radius: 5px;
    cursor: pointer;
    font-size: 0.9rem;
    /* Adjust font size */
    line-height: 1;
    height: 40px;
    /* Set explicit height */
    display: flex;
    align-items: center;
    /* Ensure content is vertically centered */
    justify-content: center;
    /* Ensure content is horizontally centered */
    width: 150px;
    /* Optional: adjust width if needed */
}
</style>