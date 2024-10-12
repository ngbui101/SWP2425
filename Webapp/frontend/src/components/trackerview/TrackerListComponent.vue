<template>
    <div>
        <table class="tracker-table">
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
                    <td>{{ tracker.name }}</td>
                    <td>{{ tracker.mode }}</td>
                    <td>{{ tracker.location }}</td>
                    <td>{{ tracker.latestMeasurement ? tracker.latestMeasurement.latitude || 'N/A' : 'N/A' }}</td>
                    <td>{{ tracker.latestMeasurement ? tracker.latestMeasurement.longitude || 'N/A' : 'N/A' }}</td>
                    <td>{{ tracker.latestMeasurement ? tracker.latestMeasurement.temperature || 'N/A' : 'N/A' }}°C</td>
                    <td>{{ tracker.latestMeasurement ? tracker.latestMeasurement.humidity || 'N/A' : 'N/A' }}%</td>
                    <td>{{ tracker.latestMeasurement ? tracker.latestMeasurement.battery || 'N/A' : 'N/A' }}</td>
                    <td>{{ tracker.imei || 'N/A' }}</td>
                    <td>
                        <i class="fas fa-cog settings-icon"></i>
                    </td>
                </tr>
            </tbody>
        </table>

        <!-- Add Tracker Button below the table -->
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
/* Include the styles for the tracker table */
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
    text-align: left;
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
    background-color: #ececec;
}

.settings-icon {
    font-size: 1.2rem;
    color: #333;
    cursor: pointer;
}

.add-tracker-wrapper {
    display: flex;
    justify-content: center;
    margin-top: 20px;
}

.add-tracker-btn {
    background-color: #28a745;
    color: white;
    padding: 10px 20px;
    border: none;
    border-radius: 5px;
    cursor: pointer;
    font-size: 1rem;
}
</style>