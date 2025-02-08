<template>
    <div :class="['container', (user.settings?.template ?? 'default') === 'dark' ? 'dark-mode' : '']">
        <div class="mapview-container">
            <!-- Left Side: Route Info Card -->
            <div class="saved-routes-card">
                <div class="card-body">
                    <h2 class="card-title-route">{{ $t('SavedRoutes') }}</h2>

                    <div class="controls-container">
                        <!-- Dropdown for Saved Route Selection -->
                        <div class="route-dropdown">
                            <label for="route-select">{{ $t('SelectSavedRoute') }}:</label>
                            <select id="route-select" v-model="selectedRouteId" @change="displaySelectedRoute">
                                <option v-for="route in savedRoutes" :key="route._id" :value="route._id">
                                    {{ route.name }}
                                </option>
                            </select>
                        </div>

                        <!-- Route Statistics -->
                        <div v-if="selectedRoute" class="stats-grid">
                            <div class="stat-item">
                                <strong>{{ $t('TotalTime') }}</strong>
                                <p>{{ formattedTime }}</p>
                            </div>
                            <div class="stat-item">
                                <strong>{{ $t('TotalSteps') }}</strong>
                                <p>{{ selectedRoute.steps }}</p>
                            </div>
                            <div class="stat-item">
                                <strong>{{ $t('AvgSpeed') }}</strong>
                                <p>{{ (selectedRoute.avgSpeed * 3.6).toFixed(2) }} km/h</p>
                            </div>
                            <div class="stat-item">
                                <strong>{{ $t('MaxSpeed') }}</strong>
                                <p>{{ (selectedRoute.maxSpeed * 3.6).toFixed(2) }} km/h</p>
                            </div>
                        </div>
                    </div>
                </div>
            </div>

            <!-- Right Side: Map View -->
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
import axios from 'axios';
import { useAuthStore } from '@/stores/auth';

const authStore = useAuthStore();
const user = ref(authStore.userDetail);

const savedRoutes = ref([]);
const selectedRouteId = ref(null);
const selectedRoute = computed(() => savedRoutes.value.find(route => route._id === selectedRouteId.value));

const formattedTime = computed(() => {
    if (!selectedRoute.value) return '00:00:00';
    const seconds = selectedRoute.value.totalTime;
    const hours = Math.floor(seconds / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);
    const remainingSeconds = seconds % 60;
    return `${hours.toString().padStart(2, '0')}:${minutes.toString().padStart(2, '0')}:${remainingSeconds.toString().padStart(2, '0')}`;
});

let map = null;
let routePolyline = null;
let startMarker = null;
let stopMarker = null;
const mapElement = ref(null);

const fetchSavedRoutes = async () => {
    try {
        const token = authStore.accessToken;
        const config = { headers: { Authorization: `Bearer ${token}` } };

        // Fetch routes from the backend
        const response = await axios.get('http://localhost:3500/api/routes', config);
        savedRoutes.value = response.data;
    } catch (error) {
        console.error('Failed to fetch saved routes:', error);
        alert('Error fetching routes');
    }
};

// Initialize Google Maps
const initializeMap = async () => {
    if (map) return;

    await waitForGoogleMaps();

    map = new google.maps.Map(mapElement.value, {
        center: { lat: 37.7749, lng: -122.4194 },
        zoom: 13
    });
};

// Display the selected route on the map
const displaySelectedRoute = () => {
    const route = selectedRoute.value;
    if (!route || !map) return;

    // Clear existing polyline and markers
    if (routePolyline) routePolyline.setMap(null);
    if (startMarker) startMarker.setMap(null);
    if (stopMarker) stopMarker.setMap(null);

    // Draw new polyline
    routePolyline = new google.maps.Polyline({
        path: route.positions,
        geodesic: true,
        strokeColor: '#FF0000',
        strokeOpacity: 1.0,
        strokeWeight: 2,
    });
    routePolyline.setMap(map);

    // Add start and stop markers
    startMarker = new google.maps.Marker({
        position: route.positions[0],
        map,
        title: 'Start',
        icon: {
            path: google.maps.SymbolPath.CIRCLE,
            scale: 8,
            fillColor: 'green',
            fillOpacity: 1,
            strokeColor: 'green',
            strokeWeight: 2,
        },
    });

    stopMarker = new google.maps.Marker({
        position: route.positions[route.positions.length - 1],
        map,
        title: 'Stop',
        icon: {
            path: google.maps.SymbolPath.CIRCLE,
            scale: 8,
            fillColor: 'red',
            fillOpacity: 1,
            strokeColor: 'red',
            strokeWeight: 2,
        },
    });

    // Fit the map bounds to the route
    const bounds = new google.maps.LatLngBounds();
    route.positions.forEach(point => bounds.extend(point));
    map.fitBounds(bounds);
};

// Utility function to wait for Google Maps API
const waitForGoogleMaps = async () => {
    return new Promise(resolve => {
        const checkGoogle = () => {
            if (window.google && window.google.maps) {
                resolve();
            } else {
                setTimeout(checkGoogle, 100);
            }
        };
        checkGoogle();
    });
};

// Load Google Maps script
const loadGoogleMapsScript = () => {
    return new Promise((resolve, reject) => {
        if (window.google && window.google.maps) {
            resolve();
        } else if (!document.querySelector("script[src*='maps.googleapis.com']")) {
            const script = document.createElement('script');
            script.src = `https://maps.googleapis.com/maps/api/js?key=YOUR_GOOGLE_MAPS_API_KEY&libraries=geometry`;
            script.async = true;
            script.defer = true;
            script.onload = resolve;
            script.onerror = reject;
            document.head.appendChild(script);
        } else {
            script.onload = resolve;
        }
    });
};

// Initialize map and fetch routes on component mount
onMounted(async () => {
    if (!window.google || !window.google.maps) {
        await loadGoogleMapsScript();
    }
    await initializeMap();
    await fetchSavedRoutes();  // Fetch saved routes after maps are loaded
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
.saved-routes-card {
    flex: 1;
    min-width: 300px;
    max-width: 400px;
    background: #f8f9fa;
    padding: 20px;
    border-radius: 8px;
    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
    border: 1px solid #fff;
}

.dark-mode .saved-routes-card {
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

.route-dropdown {
    margin-bottom: 20px;
    text-align: center;
}

.route-dropdown select {
    padding: 10px;
    font-size: 1rem;
    border-radius: 5px;
    border: 1px solid #ccc;
    width: 100%;
}

.route-dropdown label {
    display: block;
    margin-bottom: 5px;
    font-weight: bold;
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
</style>
