<template>
    <div :class="['container', (user.settings?.template ?? 'default') === 'dark' ? 'dark-mode' : '']">
        <div class="mapview-container">
            <!-- Left Info Container -->
            <div class="tracker-info-card">
                <div class="card-body">
                    <h2 class="card-title-route">{{ $t('RouteDetails') }}</h2>

                    <!-- Timer and Start/Stop Buttons -->
                    <div class="controls-container">
                        <div v-if="!isRouteActive" class="start-container">
                            <button class="start-button" @click="startRoute">{{ $t('Start') }}</button>
                        </div>
                        <div v-else class="stop-container">
                            <p class="timer-text">{{ formattedTime }}</p>
                            <button class="stop-button" @click="stopRoute">{{ $t('Stop') }}</button>
                        </div>
                        <div v-if="!isRouteActive && timerVisible" class="time-display">
                            <p>{{ $t('Time') }}: {{ formattedTime }}</p>
                        </div>
                    </div>

                    <!-- Statistics -->
                    <div class="stats-grid">
                        <div class="stat-item">
                            <strong>{{ $t('Steps') }}</strong>
                            <p>{{ stepsWalked }}</p>
                        </div>
                        <div class="stat-item">
                            <strong>{{ $t('AvgSpeed') }}</strong>
                            <p>{{ avgSpeed }} km/h</p>
                        </div>
                        <div class="stat-item">
                            <strong>{{ $t('MaxSpeed') }}</strong>
                            <p>{{ maxSpeed }} km/h</p>
                        </div>
                        <div class="stat-item">
                            <strong>{{ $t('TotalDistance') }}</strong>
                            <p>{{ totalDistance }} km</p>
                        </div>
                    </div>
                </div>
            </div>

            <!-- Right Map Container -->
            <div class="card map-card">

                <div class="map-container">
                    <div ref="mapElement" class="map"></div>
                </div>
            </div>
        </div>
    </div>
</template>


<script setup>
import { ref, computed, onMounted } from 'vue';
import { useAuthStore } from "@/stores/auth";

// Store references
const authStore = useAuthStore();
const user = computed(() => authStore.userDetail);

// Timer and route state
const isRouteActive = ref(false);
const timerVisible = ref(false);
const timer = ref(0);
let timerInterval = null;

// Statistics data
const stepsWalked = ref(0);
const avgSpeed = ref(0);
const maxSpeed = ref(0);
const totalDistance = ref(0);

// Timer formatting
const formattedTime = computed(() => {
    const minutes = Math.floor(timer.value / 60);
    const seconds = timer.value % 60;
    return `${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')}`;
});

// Start route logic
const startRoute = () => {
    isRouteActive.value = true;
    timerVisible.value = true;
    timer.value = 0;
    stepsWalked.value = 0;
    avgSpeed.value = 0;
    maxSpeed.value = 0;
    totalDistance.value = 0;

    // Simulated updates for demo purposes
    timerInterval = setInterval(() => {
        timer.value += 1;
        stepsWalked.value += Math.floor(Math.random() * 3) + 1; // Random step count
        avgSpeed.value = (Math.random() * 10 + 5).toFixed(2); // Random average speed
        maxSpeed.value = (Math.random() * 15 + 10).toFixed(2); // Random max speed
        totalDistance.value = (stepsWalked.value * 0.0008).toFixed(2); // Approx. conversion to km
    }, 1000);
};

// Stop route logic
const stopRoute = () => {
    isRouteActive.value = false;
    clearInterval(timerInterval);
    timerInterval = null;
};

// Initialize map
const mapElement = ref(null);
let map = null;
const initializeMap = () => {
    map = new google.maps.Map(mapElement.value, {
        center: { lat: 51.1657, lng: 10.4515 }, // Example: Germany's lat/lng
        zoom: 6,
        streetViewControl: false,
    });
};

// Fetch user details and initialize map
onMounted(async () => {
    await authStore.getUser();
    initializeMap();
});
</script>


<style scoped>
.card-title-route {
    font-size: 1.5rem;
    font-weight: bold;
    color: #ffae00;
    text-align: center;
}

.container {
    display: flex;
    flex-direction: column;
    gap: 20px;
    padding: 20px;
}

.mapview-container {
    display: flex;
    flex-wrap: wrap;
    gap: 20px;
    justify-content: space-between;
}

/* Info Card */
.tracker-info-card {
    flex: 1;
    min-width: 300px;
    max-width: 400px;
    background: #f8f9fa;
    padding: 20px;
    border-radius: 8px;
    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
}

.dark-mode .tracker-info-card {
    background: #2e2e2e;
    color: #ddd;
}

.card-body {
    display: flex;
    flex-direction: column;
    align-items: center;
}

.controls-container {
    display: flex;
    flex-direction: column;
    align-items: center;
    margin-bottom: 20px;
}

.timer-text {
    font-size: 2rem;
    font-weight: bold;
    margin: 10px 0;
}

.time-display p {
    font-size: 1.5rem;
    font-weight: bold;
}

.start-button,
.stop-button {
    font-size: 1.2rem;
    padding: 10px 20px;
    border-radius: 5px;
    border: none;
    cursor: pointer;
    transition: background-color 0.3s ease;
}

.start-button {
    background-color: #28a745;
    color: #fff;
}

.start-button:hover {
    background-color: #218838;
}

.stop-button {
    background-color: #dc3545;
    color: #fff;
}

.stop-button:hover {
    background-color: #c82333;
}

/* Stats Grid */
.stats-grid {
    display: grid;
    grid-template-columns: repeat(2, 1fr);
    gap: 20px;
    width: 100%;
}

.stat-item {
    text-align: center;
    padding: 10px;
    background: #e6cc99;
    border-radius: 8px;
    font-size: 1.2rem;
    font-weight: bold;
}

.dark-mode .stat-item {
    background: #444;
    color: #ddd;
}

/* Map Card */
.map-card {
    flex: 2;
    min-width: 600px;
    background: #f8f9fa;
    padding: 20px;
    border-radius: 8px;
    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
}

.dark-mode .map-card {
    background: #2e2e2e;
    color: #ddd;
}

.map-container {
    width: 100%;
    height: 500px;
    border-radius: 8px;
}

.map {
    width: 100%;
    height: 100%;
}

.dark-mode .map-container {
    border: 1px solid #555;
}

/* Responsive Design */
@media screen and (max-width: 768px) {
    .mapview-container {
        flex-direction: column;
        align-items: center;
    }

    .tracker-info-card,
    .map-card {
        width: 100%;
    }

    .stats-grid {
        grid-template-columns: 1fr;
    }
}
</style>
