<template>
    <div :class="['container', (user.settings?.template ?? 'default') === 'dark' ? 'dark-mode' : '']">
        <div class="mapview-container">
            <div v-if="trackers.length === 0" class="overlay">
                <p class="overlay-text">No trackers available. Please add a tracker.</p>
            </div>
            <!-- Tracker and Timestamp Range Selection Card -->
            <div v-if="trackers.length > 0" class="tracker-info-card">
                <div class="card-body">
                    <!-- Select Tracker Dropdown -->
                    <label for="tracker-dropdown" class="dropdown-label">
                        {{ $t("MapHistory-SelectTracker") }}:
                        <select id="tracker-dropdown" class="tracker-dropdown" v-model="selectedTracker"
                            @change="updateSelectedTrackerMeasurements">
                            <option v-for="tracker in trackers" :key="tracker._id" :value="tracker._id">
                                {{ tracker.name }}
                            </option>
                        </select>
                    </label>

                    <!-- From Timestamp Dropdown with Date Picker -->
                    <label for="from-timestamp-dropdown" class="dropdown-label">
                        {{ $t("MapHistory-FromTimestamp") }}:
                        <div class="timestamp-selection">
                            <select id="from-timestamp-dropdown"
                                :class="['tracker-dropdown', !fromTimestamp && errorMessage ? 'error-dropdown' : '']"
                                v-model="fromTimestamp" @change="updateTimestampRange">
                                <option v-for="measurement in selectedTrackerMeasurements" :key="measurement._id"
                                    :value="measurement.createdAt">
                                    {{ formatTimestamp(measurement.createdAt) }}
                                </option>
                            </select>
                            <DatePicker v-model="fromDate" type="date" :disabled-date="disableUnavailableDates"
                                @change="handleFromDateChange" :clearable="false" placeholder="Start" />
                        </div>
                    </label>

                    <!-- To Timestamp Dropdown with Date Picker -->
                    <label for="to-timestamp-dropdown" class="dropdown-label">
                        {{ $t("MapHistory-ToTimestamp") }}:
                        <div class="timestamp-selection">
                            <select id="to-timestamp-dropdown"
                                :class="['tracker-dropdown', !toTimestamp && errorMessage ? 'error-dropdown' : '']"
                                v-model="toTimestamp" @change="updateTimestampRange">
                                <option v-for="measurement in selectedTrackerMeasurements" :key="measurement._id"
                                    :value="measurement.createdAt">
                                    {{ new Date(measurement.createdAt).toLocaleString() }}
                                </option>
                            </select>
                            <DatePicker v-model="toDate" type="date" :disabled-date="disableUnavailableDates"
                                @change="handleToDateChange" :clearable="false" placeholder="End" />
                        </div>
                    </label>

                    <!-- Error message display -->
                    <p v-if="errorMessage" class="error-message">{{ errorMessage }}</p>

                    <!-- Use Pin for Every Measurement Checkbox and Filter Button -->
                    <div class="pin-filter-container">
                        <label class="checkbox-label">
                            <input type="checkbox" v-model="usePinForEveryMeasurement" />
                            {{ $t("MapHistory-UsePinforeveryMeasurement") }}
                        </label>

                    </div>

                    <!-- Build History Button -->
                    <button class="build-history-button" @click="buildHistory">Build History</button>

                    <!-- Scrollable List of Selected Measurements within the card, only visible after clicking "Build History" -->
                    <div v-if="showMeasurementsList && filteredMeasurementsForHistory.length > 0"
                        class="measurements-list">
                        <div class="measurements-list-header">
                            <span>Timestamp</span>
                            <span>Position</span>
                            <span>Lat</span>
                            <span>Long</span>
                        </div>
                        <div class="measurements-list-content">
                            <div v-for="measurement in filteredMeasurementsForHistory" :key="measurement._id"
                                class="measurement-item">
                                <!-- Format timestamp to show "day.month hour:minute" -->
                                <span>{{ formatTimestamp(measurement.createdAt) }}</span>
                                <span>{{ measurement.address }}</span>
                                <span>{{ measurement.latitude.toFixed(4) }}</span>
                                <span>{{ measurement.longitude.toFixed(4) }}</span>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
        <!-- Map Card -->
        <div v-if="trackers.length > 0" class="card">
            <div class="card-header">
                <p class="map-title">{{ mapHistoryTitle }}</p>
            </div>

            <!-- Map Container -->
            <div class="map-container">
                <div ref="mapElement" class="map"></div>

                <!-- Grey Overlay for dark mode -->
                <div v-if="(user.settings?.template ?? 'default') === 'dark'" class="map-overlay"></div>
            </div>
        </div>
        <!-- Conditionally render the Measurement Filter Popup -->
        <HistoryTimeStampFilterPopup v-if="isMeasurementFilterPopupOpen" :template="user.settings?.template"
            :filters="user.settings.measurementFilters" :closePopup="closeMeasurementFilters"
            :applyFilters="applyMeasurementFilters" />
    </div>
</template>

<script setup>
import { ref, computed, onMounted, watch } from 'vue';
import axios from 'axios';
import { useAuthStore } from "@/stores/auth";
import DatePicker from 'vue-datepicker-next';
import './styles_maphistory.css';
import 'vue-datepicker-next/index.css';
import HistoryTimeStampFilterPopup from './HistoryTimestampFilterPopup.vue'
// Open and close the measurement filter popup
const formatTimestamp = (timestamp) => {
    const date = new Date(timestamp);
    const day = date.getDate().toString().padStart(2, '0');
    const month = (date.getMonth() + 1).toString().padStart(2, '0');
    const year = date.getFullYear();
    const hours = date.getHours().toString().padStart(2, '0');
    const minutes = date.getMinutes().toString().padStart(2, '0');
    const seconds = date.getSeconds().toString().padStart(2, '0');
    return `${day}.${month}.${year} ${hours}:${minutes}:${seconds}`;
};
const isMeasurementFilterPopupOpen = ref(false);
const openMeasurementFilters = () => {
    isMeasurementFilterPopupOpen.value = true;
};
const closeMeasurementFilters = () => {
    isMeasurementFilterPopupOpen.value = false;
};

// Function to apply filters
const applyMeasurementFilters = (filters) => {

    // Implement the filtering logic based on `filters` received from the popup
    // This could be storing `filters` and using them to filter `selectedTrackerMeasurements`
};
const authStore = useAuthStore();
const user = computed(() => authStore.userDetail);
const measurementFilters = computed(() => user.value.settings?.measurementFilters || { mode: [] });
const filteredMeasurementsForHistory = ref([]);
const trackers = ref([]);
const selectedTracker = ref(null);
const fromDate = ref(null);
const toDate = ref(null);
const fromTimestamp = ref(null);
const toTimestamp = ref(null);
const selectedTrackerMeasurements = ref([]);
const usePinForEveryMeasurement = ref(false);
const errorMessage = ref('');
const mapElement = ref(null);
const showMeasurementsList = ref(false);
let map = null;
let markers = [];
let path = null;


// Generate a list of unique measurement dates
const measurementDates = computed(() => {
    const dates = new Set();
    selectedTrackerMeasurements.value.forEach((m) => {
        const date = new Date(m.createdAt);
        date.setHours(0, 0, 0, 0); // Normalize to midnight for comparison
        dates.add(date.getTime()); // Store as timestamp for easy comparison
    });
    return Array.from(dates);
});

// Disable unavailable dates in date picker
const disableUnavailableDates = (date) => {
    return !measurementDates.value.includes(date.setHours(0, 0, 0, 0)); // Disable if date is not in measurementDates
};
const updateTimestampRange = () => {
    // Add logic here if necessary

};
// Automatically set the earliest timestamp for the selected date in the FROM dropdown
const handleFromDateChange = (date) => {
    // Only set the date (clear the time)
    fromDate.value = new Date(date.setHours(0, 0, 0, 0));

    // Find the earliest timestamp on the selected date
    const measurementsOnDate = selectedTrackerMeasurements.value.filter((m) => {
        const mDate = new Date(m.createdAt);
        return (
            mDate.getFullYear() === fromDate.value.getFullYear() &&
            mDate.getMonth() === fromDate.value.getMonth() &&
            mDate.getDate() === fromDate.value.getDate()
        );
    });

    if (measurementsOnDate.length > 0) {
        // Set `fromTimestamp` to the earliest timestamp on the selected date
        fromTimestamp.value = measurementsOnDate[0].createdAt;
    }
    updateTimestampRange();
};

const handleToDateChange = (date) => {
    const selectedDate = new Date(date);
    selectedDate.setHours(23, 59, 59, 999); // Set to end of the day (23:59)
    toDate.value = selectedDate;

    // Filter measurements to find those on the selected date
    const measurementsOnDate = selectedTrackerMeasurements.value.filter((m) => {
        const mDate = new Date(m.createdAt);
        return mDate.getFullYear() === selectedDate.getFullYear() &&
            mDate.getMonth() === selectedDate.getMonth() &&
            mDate.getDate() === selectedDate.getDate();
    });

    if (measurementsOnDate.length > 0) {
        // Set `toTimestamp` to the latest timestamp on the selected date
        toTimestamp.value = measurementsOnDate[measurementsOnDate.length - 1].createdAt;
    }
    updateTimestampRange();
};



// Computed property for the map history title
const mapHistoryTitle = computed(() => {
    if (fromTimestamp.value && toTimestamp.value) {
        return `Map history from ${formatTimestamp(fromTimestamp.value)} to ${formatTimestamp(toTimestamp.value)}`;
    } else if (fromTimestamp.value) {
        return `Map history from ${formatTimestamp(fromTimestamp.value)}`;
    } else {
        return "Map history";
    }
});

// Fetch trackers and their measurements
const fetchTrackersForUser = async () => {
    try {
        const token = authStore.accessToken;
        const config = { headers: { Authorization: `Bearer ${token}` } };

        const response = await axios.get('http://localhost:3500/api/tracker/user/', config);
        trackers.value = response.data;

        for (const tracker of trackers.value) {
            const measurementsResponse = await axios.get(`http://localhost:3500/api/position/tracker/${tracker._id}`, config);
            tracker.measurements = measurementsResponse.data;

            if (!selectedTracker.value) {
                selectedTracker.value = tracker._id;
            }
        }

        updateSelectedTrackerMeasurements();
    } catch (error) {
        console.error('Failed to fetch trackers or measurements:', error);
    }
};

// Update measurements for selected tracker and filter out invalid coordinates
const updateSelectedTrackerMeasurements = () => {
    const tracker = trackers.value.find(t => t._id === selectedTracker.value);
    if (tracker) {
        selectedTrackerMeasurements.value = tracker.measurements
            .filter(m => {
                const latitude = parseFloat(m.latitude);
                const longitude = parseFloat(m.longitude);
                return !isNaN(latitude) && !isNaN(longitude);
            })
            .map(m => ({
                ...m,
                latitude: parseFloat(m.latitude),
                longitude: parseFloat(m.longitude),
            }))
            .sort((a, b) => new Date(b.createdAt) - new Date(a.createdAt));
    }
};

// Function to draw numbered pins for each measurement
const drawPinsWithGroupedInfoWindow = (measurements) => {
    markers.forEach(marker => marker.setMap(null)); // Clear previous markers
    markers = []; // Reset markers array

    // Step 1: Group measurements by location (latitude and longitude)
    const locationGroups = new Map();

    measurements.forEach((m, index) => {
        const key = `${m.latitude},${m.longitude}`;
        if (!locationGroups.has(key)) {
            locationGroups.set(key, []);
        }
        locationGroups.get(key).push({ ...m, index: index + 1 });
    });

    // Step 2: Create markers and info windows for each unique location
    locationGroups.forEach((groupedMeasurements, key) => {
        const { latitude, longitude } = groupedMeasurements[0];

        // Create a single marker for the location
        const marker = new google.maps.Marker({
            position: { lat: latitude, lng: longitude },
            map,
            label: `${groupedMeasurements[0].index}`, // Show the first index as label
        });

        // Create combined content for the info window
        const infoContent = groupedMeasurements
            .map(m => `Measurement ${m.index}: ${new Date(m.createdAt).toLocaleString()}`)
            .join("<br>");

        const infoWindow = new google.maps.InfoWindow({
            content: infoContent
        });

        // Show info window on hover
        marker.addListener("mouseover", () => {
            infoWindow.open(map, marker);
        });
        marker.addListener("mouseout", () => {
            infoWindow.close();
        });

        markers.push(marker);
    });
};

// Function to draw pins for only the first and last measurements
const drawPinsForFirstAndLast = (measurements) => {
    markers.forEach(marker => marker.setMap(null)); // Clear previous markers
    markers = []; // Reset markers array

    measurements.forEach((m, index) => {
        // Draw numbered pins for the first and last measurements only
        if (index === 0 || index === measurements.length - 1) {
            const marker = new google.maps.Marker({
                position: { lat: m.latitude, lng: m.longitude },
                map,
                label: `${index + 1}`,
                title: `Point ${index + 1}`
            });
            markers.push(marker);
        }
    });
};
const getReverseGeocodingAddress = async (lat, lng) => {
    const geocodingUrl = `http://localhost:3500/api/geocode?lat=${lat}&lng=${lng}`;

    try {
        const response = await axios.get(geocodingUrl);
        if (response.data && response.data.address) {
            const address = response.data.address;

            // Extract zip code and city (with support for special characters)
            const zipCityMatch = address.match(/\b\d{5}\b\s+[A-Za-zÀ-ÖØ-öø-ÿ\s]+/);
            return zipCityMatch ? zipCityMatch[0] : 'Unknown Location';
        } else {
            return 'Unknown Location';
        }
    } catch (error) {
        console.error('Failed to perform reverse geocoding:', error);
        return 'Unknown Location';
    }
};

// Build history action with validation and error handling
const buildHistory = async () => {
    const tracker = trackers.value.find(t => t._id === selectedTracker.value);

    // Validate that both timestamps are selected
    if (!fromTimestamp.value || !toTimestamp.value) {
        errorMessage.value = "You have to select a start- and endpoint first";
        return;
    }

    // Validate that the start date is not later than the end date
    if (new Date(fromTimestamp.value) > new Date(toTimestamp.value)) {
        errorMessage.value = "The starting point of the history cannot be later than the endpoint";
        return;
    }

    errorMessage.value = ''; // Clear error if all validations pass

    const measurements = tracker.measurements
        .filter(m =>
            new Date(m.createdAt) >= new Date(fromTimestamp.value) &&
            new Date(m.createdAt) <= new Date(toTimestamp.value)
        )
        .map(m => ({
            ...m,
            latitude: parseFloat(m.latitude),
            longitude: parseFloat(m.longitude),
        }))
        .filter(m => !isNaN(m.latitude) && !isNaN(m.longitude)) // Filter out NaN values
        .sort((a, b) => new Date(a.createdAt) - new Date(b.createdAt));

    // Fetch reverse geocoding addresses for each filtered measurement
    for (const measurement of measurements) {
        measurement.address = await getReverseGeocodingAddress(measurement.latitude, measurement.longitude);
    }

    // After all addresses are retrieved, update the filteredMeasurementsForHistory list and display it
    filteredMeasurementsForHistory.value = measurements;
    showMeasurementsList.value = true;

    // Map drawing logic remains the same
    markers.forEach(marker => marker.setMap(null));
    if (path) path.setMap(null);

    const pathCoordinates = measurements.map(m => ({ lat: m.latitude, lng: m.longitude }));
    path = new google.maps.Polyline({
        path: pathCoordinates,
        geodesic: true,
        strokeColor: "#FF0000",
        strokeOpacity: 1.0,
        strokeWeight: 2,
    });
    path.setMap(map);

    if (usePinForEveryMeasurement.value) {
        drawPinsWithGroupedInfoWindow(measurements);
    } else {
        drawPinsForFirstAndLast(measurements);
    }

    const bounds = new google.maps.LatLngBounds();
    pathCoordinates.forEach(coord => bounds.extend(coord));
    map.fitBounds(bounds);
};

// Initialize the map
const initializeMap = () => {
    if (!mapElement.value) return;

    const position = { lat: 51.47686, lng: 7.20766 };

    if (!map) {
        map = new google.maps.Map(mapElement.value, {
            center: position,
            zoom: 12,
            streetViewControl: false
        });
    } else {
        map.setCenter(position);
    }
};

// Load Google Maps API script dynamically if needed
const loadGoogleMapsScript = () => {
    return new Promise((resolve, reject) => {
        if (window.google && window.google.maps) {
            resolve();
        } else if (!document.querySelector("script[src*='maps.googleapis.com']")) {
            const script = document.createElement('script');
            script.src = `https://maps.googleapis.com/maps/api/js?key=AIzaSyDzh7DicT6KmawobOi6iir27IFEQBsRhRo&libraries=geometry`;
            script.async = true;
            script.defer = true;
            script.onload = resolve;
            script.onerror = reject;
            document.head.appendChild(script);
        }
    });
};

onMounted(async () => {
    await authStore.getUser();
    await fetchTrackersForUser();
    loadGoogleMapsScript().then(() => {
        initializeMap();
    });

});

watch(selectedTracker, updateSelectedTrackerMeasurements);
</script>

<style scoped>
.timestamp-selection {
    display: flex;
    align-items: center;
    gap: 10px;
}


.error-dropdown {
    border-color: red;
}

.error-message {
    color: red;
    font-size: 0.9rem;
}

.filters-button {
    margin-top: 20px;
    font-size: 0.9rem;
    background-color: #C19A6B;
    /* Same color as "Switch Mode" */
    color: #1f1f1f;
    border: none;
    border-radius: 18px;
    cursor: pointer;
    transition: all 0.3s ease;
}

.filters-button:hover {
    background-color: #8f6b3f;
}

.dark-mode .filters-button {
    background-color: #E69543;
    color: #1f1f1f;
}

.dark-mode .filters-button:hover {
    background-color: #bd7227;
}

.pin-filter-container {
    display: flex;
    align-items: center;
    gap: 10px;
}

.measurements-list {
    margin-top: 20px;
    max-height: 180px;
    overflow-y: auto;
    overflow-x: auto;
    /* Allow horizontal scrolling */
    border: 1px solid #ddd;
    border-radius: 6px;
    background-color: #f9f9f9;
}

.measurements-list-header,
.measurement-item {
    display: grid;
    grid-template-columns: 2fr 3fr 1fr 1fr;
    /* Adjusted for spacing */
    padding: 5px 15px;
    /* More space */
    font-size: 0.9rem;
    column-gap: 10px;
    /* Add gap between columns */
}

.measurements-list-header {
    font-weight: bold;
    background-color: #ececec;
    border-bottom: 1px solid #ddd;
}

.measurement-item:nth-child(odd) {
    background-color: #f7f7f7;
}

.measurement-item:nth-child(even) {
    background-color: #ffffff;
}

.measurements-list-content {
    max-height: 140px;
    /* Limit height for scrolling */
    overflow-y: auto;
}
</style>