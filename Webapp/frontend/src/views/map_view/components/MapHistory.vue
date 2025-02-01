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
                    <label for="tracker-dropdown" class="dropdown-label" ref="mapHistoryTour1">
                        {{ $t("MapHistory-SelectTracker") }}:
                        <select id="tracker-dropdown" class="tracker-dropdown" v-model="selectedTracker"
                            @change="updateSelectedTrackerMeasurements">
                            <option v-for="tracker in trackers" :key="tracker._id" :value="tracker._id">
                                {{ tracker.name }}
                            </option>
                        </select>
                    </label>

                    <!-- From Timestamp Dropdown with Date Picker -->
                    <label for="from-timestamp-dropdown" class="dropdown-label" ref="mapHistoryTour2">
                        {{ $t("MapHistory-FromTimestamp") }}
                        <div class="timestamp-selection">
                            <select id="from-timestamp-dropdown"
                                :class="['tracker-dropdown', !fromTimestamp && errorMessage ? 'error-dropdown' : '']"
                                v-model="fromTimestamp" @change="updateTimestampRange">
                                <option v-for="measurement in selectedTrackerMeasurements" :key="measurement._id"
                                    :value="measurement.createdAt">
                                    {{ new Date(measurement.createdAt).toLocaleString() }}
                                </option>
                            </select>
                            <DatePicker ref="mapHistoryTour4" v-model="fromDate" type="date"
                                :disabled-date="disableUnavailableDates" @change="handleFromDateChange"
                                :clearable="false" placeholder="Start" />
                        </div>
                    </label>

                    <!-- To Timestamp Dropdown with Date Picker -->
                    <label for="to-timestamp-dropdown" class="dropdown-label" ref="mapHistoryTour3">
                        {{ $t("MapHistory-ToTimestamp") }}
                        <div class="timestamp-selection">
                            <select id="to-timestamp-dropdown"
                                :class="['tracker-dropdown', !toTimestamp && errorMessage ? 'error-dropdown' : '']"
                                v-model="toTimestamp" @change="updateTimestampRange">
                                <option v-for="measurement in filteredToTimestamps" :key="measurement._id"
                                    :value="measurement.createdAt">
                                    {{ new Date(measurement.createdAt).toLocaleString() }}
                                </option>
                            </select>
                            <DatePicker v-model="toDate" type="date" :disabled-date="disableUnavailableDatesForToDate"
                                @change="handleToDateChange" :clearable="false" :placeholder="$t('MapHistory-End')" />
                        </div>
                    </label>


                    <!-- Error message display -->
                    <p v-if="errorMessage" class="error-message">{{ errorMessage }}</p>

                    <!-- Use Pin for Every Measurement Checkbox and Filter Button -->
                    <div class="pin-filter-container" ref="mapHistoryTour5">
                        <label class="checkbox-label">
                            <input type="checkbox" v-model="usePinForEveryMeasurement" />
                            {{ $t("MapHistory-UsePinforeveryMeasurement") }}
                        </label>

                    </div>

                    <!-- Build History Button -->
                    <button class="build-history-button" ref="mapHistoryTour6" @click="buildHistory">{{
                        $t("BuildHistory") }}</button>

                    <!-- Scrollable List of Selected Measurements within the card -->
                    <div v-if="showMeasurementsList && filteredMeasurementsForHistory.length > 0"
                        class="measurements-list">
                        <!-- Header Row -->
                        <div class="measurements-list-header">
                            <span>#</span>
                            <span>{{ $t("Timestamp") }}</span>
                            <span>{{ $t("Mode") }}</span>
                            <span>{{ $t("Position") }}</span>
                            <span>Lat</span>
                            <span>Long</span>
                        </div>
                        <!-- Content Rows -->
                        <div class="measurements-list-content">
                            <div v-for="(measurement, index) in filteredMeasurementsForHistory" :key="measurement._id"
                                class="measurement-item">
                                <!-- Display Index, Timestamp, Mode, Address, Lat, Long -->
                                <span>{{ index + 1 }}</span>
                                <span>{{ formatTimestamp(measurement.createdAt) }}</span>
                                <span>{{ measurement.mode }}</span>
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
        <div v-if="trackers.length > 0" class="card" ref="mapHistoryTour7">
            <div class="card-header">

            </div>

            <!-- Map Container -->
            <div class="map-container">
                <div ref="mapElement" class="map"></div>

                <!-- Grey Overlay for dark mode -->
                <div v-if="(user.settings?.template ?? 'default') === 'dark'" class="map-overlay"></div>
                <div v-if="isLoading" class="map-loading-overlay">
                    <div class="loading-content">
                        <div class="spinner"></div>
                        <p>{{ $t("MapHistory-LoadingMessage") }}</p>
                        <button class="cancel-button" @click="cancelBuildHistory">{{ $t("MapHistory-CancelButton")
                            }}</button>
                    </div>
                </div>
            </div>
            <!-- Legend Section -->
            <div class="legend" ref="mapHistoryTour8">
                <span :style="{ color: modeColors.GPS }" class="legend-item">
                    <i class="fas fa-map-pin"></i> GPS&nbsp;&nbsp;
                </span>
                <span :style="{ color: modeColors.LTE }" class="legend-item">
                    <i class="fas fa-map-pin"></i> LTE&nbsp;&nbsp;
                </span>
                <span :style="{ color: modeColors.NBIOT }" class="legend-item">
                    <i class="fas fa-map-pin"></i> NBIOT&nbsp;&nbsp;
                </span>
                <span :style="{ color: modeColors.GSM }" class="legend-item">
                    <i class="fas fa-map-pin"></i> GSM&nbsp;&nbsp;
                </span>
            </div>


        </div>




    </div>
</template>

<script setup>
import { ref, computed, onMounted, watch } from 'vue';
import axios from 'axios';
import { useAuthStore } from "@/stores/auth";
import DatePicker from 'vue-datepicker-next';
import './styles_maphistory.css';

import 'vue-datepicker-next/index.css';
import {
    mapHistoryTour1,
    mapHistoryTour2,
    mapHistoryTour3,
    mapHistoryTour4,
    mapHistoryTour5,
    mapHistoryTour6,
    mapHistoryTour7,
    mapHistoryTour8
} from '@/routes/tourRefs.js';

const isLoading = ref(false);
let abortController = null;

const fromTimestamp = ref(null);
const toTimestamp = ref(null);

const modeColors = computed(() => ({
    GPS: "#228B22",  // Green
    LTE: "#FFA500",  // Orange
    NBIOT: "#0000FF", // Blue
    GSM: "#FF69B4",   // Pink
}));



const modeAccuracy = computed(() => ({
    green: "0-25m",
    yellow: "26-50m",
    red: ">50m",
}));



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

const authStore = useAuthStore();
const user = computed(() => authStore.userDetail);

const filteredMeasurementsForHistory = ref([]);
const trackers = ref([]);
const selectedTracker = ref(null);
const fromDate = ref(null);
const toDate = ref(null);

const selectedTrackerMeasurements = ref([]);
const usePinForEveryMeasurement = ref(false);
const errorMessage = ref('');
const mapElement = ref(null);
const showMeasurementsList = ref(false);
let map = null;
let markers = [];
let path = null;
let accuracyCircle = null;

const disableUnavailableDatesForToDate = (date) => {
    // Get all unique valid dates from measurements
    const validDates = new Set(
        selectedTrackerMeasurements.value.map((m) => {
            const mDate = new Date(m.createdAt);
            return mDate.toDateString(); // Normalize date format
        })
    );

    // Disable dates that:
    // 1. Are earlier than `FromDate`
    // 2. Do not exist in the list of valid measurement dates
    return (
        (fromDate.value && date.getTime() < fromDate.value.getTime()) || // Earlier than `FromDate`
        !validDates.has(date.toDateString()) // Not a valid measurement date
    );
};

const disableUnavailableDates = (date) => {
    // Get all unique dates from `selectedTrackerMeasurements`
    const validDates = new Set(
        selectedTrackerMeasurements.value.map((m) => {
            const mDate = new Date(m.createdAt);
            return mDate.toDateString(); // Normalize date format to compare
        })
    );

    // Disable the date if it's not in the valid dates set
    return !validDates.has(date.toDateString());
};


const updateTimestampRange = () => {
    // Add logic here if necessary

};
// Automatically set the earliest timestamp for the selected date in the FROM dropdown
const handleFromDateChange = (date) => {
    fromDate.value = new Date(date.setHours(0, 0, 0, 0)); // Start of the day

    // Find the first measurement on the selected `fromDate`
    const measurementsOnDate = selectedTrackerMeasurements.value.filter((m) => {
        const mDate = new Date(m.createdAt);
        return mDate.toDateString() === fromDate.value.toDateString();
    });

    if (measurementsOnDate.length > 0) {
        fromTimestamp.value = measurementsOnDate[0].createdAt;

        // Reset `toTimestamp` if it's invalid
        if (toTimestamp.value && new Date(toTimestamp.value) <= new Date(fromTimestamp.value)) {
            toTimestamp.value = null;
        }
    } else {
        fromTimestamp.value = null; // Reset if no measurements are found
    }
};




const handleToDateChange = (date) => {
    toDate.value = new Date(date.setHours(23, 59, 59, 999)); // Set to the end of the day

    const measurementsOnDate = selectedTrackerMeasurements.value.filter((m) => {
        const mDate = new Date(m.createdAt);
        return (
            mDate.getFullYear() === toDate.value.getFullYear() &&
            mDate.getMonth() === toDate.value.getMonth() &&
            mDate.getDate() === toDate.value.getDate()
        );
    });

    if (measurementsOnDate.length > 0) {
        // Set `toTimestamp` to the latest timestamp on the selected day
        toTimestamp.value = measurementsOnDate[measurementsOnDate.length - 1].createdAt;
    } else {
        toTimestamp.value = null; // Reset if no valid measurements are found
    }
};


watch(fromTimestamp, () => {
    // Ensure `toTimestamp` is valid when `fromTimestamp` changes
    if (toTimestamp.value && new Date(toTimestamp.value) <= new Date(fromTimestamp.value)) {
        toTimestamp.value = null; // Reset if invalid
    }
});

watch(toTimestamp, () => {
    // Additional logic can be added if needed for `toTimestamp`
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
const filteredToTimestamps = computed(() => {
    // Ensure that only timestamps newer than `fromTimestamp` are shown
    if (!fromTimestamp.value) {
        return selectedTrackerMeasurements.value; // Show all if no `fromTimestamp` is selected
    }

    return selectedTrackerMeasurements.value.filter(
        (measurement) => new Date(measurement.createdAt) > new Date(fromTimestamp.value)
    );
});



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
            .sort((a, b) => new Date(a.createdAt) - new Date(b.createdAt));
    }
};

// Function to draw numbered pins for each measurement
const drawPinsWithGroupedInfoWindow = (measurements) => {
    markers.forEach(marker => marker.setMap(null)); // Clear previous markers
    markers = []; // Reset markers array

    const locationGroups = new Map();
    measurements.forEach((m, index) => {
        const key = `${m.latitude},${m.longitude}`;
        if (!locationGroups.has(key)) {
            locationGroups.set(key, []);
        }
        locationGroups.get(key).push({ ...m, index: index + 1 });
    });

    locationGroups.forEach((groupedMeasurements) => {
        const { latitude, longitude, mode } = groupedMeasurements[0];
        const color = modeColors.value[mode] || "#000000"; // Default to black if mode is undefined

        const marker = new google.maps.Marker({
            position: { lat: latitude, lng: longitude },
            map,
            label: `${groupedMeasurements[0].index}`,
            icon: {
                path: google.maps.SymbolPath.CIRCLE,
                fillColor: color,
                fillOpacity: 1,
                strokeColor: "#000000",
                strokeWeight: 1,
                scale: 8,
            },
        });

        const infoContent = groupedMeasurements
            .map(m => `Measurement ${m.index}: ${new Date(m.createdAt).toLocaleString()} (Mode: ${m.mode})`)
            .join("<br>");

        const infoWindow = new google.maps.InfoWindow({
            content: infoContent,
        });

        marker.addListener("mouseover", () => infoWindow.open(map, marker));
        marker.addListener("mouseout", () => infoWindow.close());

        marker.addListener("click", () => {
            if (accuracyCircle) accuracyCircle.setMap(null); // Clear previous circle

            const accuracy = groupedMeasurements[0].accuracy;
            if (!isNaN(accuracy)) {
                accuracyCircle = new google.maps.Circle({
                    map,
                    center: { lat: latitude, lng: longitude },
                    radius: accuracy,
                    fillColor: color,
                    fillOpacity: 0.2,
                    strokeColor: color,
                    strokeOpacity: 0.8,
                    strokeWeight: 1,
                });
            }
        });

        markers.push(marker);
    });
};





// Function to draw pins for only the first and last measurements
const drawPinsForFirstAndLast = (measurements) => {
    // Clear previous markers
    markers.forEach(marker => marker.setMap(null));
    markers = [];

    measurements.forEach((m, index) => {
        // Only draw pins for the first and last measurements
        if (index === 0 || index === measurements.length - 1) {
            const color = modeColors.value[m.mode] || "#000000"; // Default to black if mode is undefined

            const marker = new google.maps.Marker({
                position: { lat: m.latitude, lng: m.longitude },
                map,
                label: `${index + 1}`, // Add index as a label
                title: `Point ${index + 1}`,
                icon: {
                    path: google.maps.SymbolPath.CIRCLE,
                    fillColor: color,
                    fillOpacity: 1,
                    strokeColor: "#000000",
                    strokeWeight: 1,
                    scale: 8,
                },
            });

            markers.push(marker);
        }
    });
};
const getReverseGeocodingAddress = async (lat, lng, signal) => {
    const geocodingUrl = `http://localhost:3500/api/geocode?lat=${lat}&lng=${lng}`;

    try {
        const response = await axios.get(geocodingUrl, { signal });
        if (response.data && response.data.address) {
            const address = response.data.address;

            // Extract zip code and city (with support for special characters)
            const zipCityMatch = address.match(/\b\d{5}\b\s+[A-Za-zÀ-ÖØ-öø-ÿ\s]+/);
            return zipCityMatch ? zipCityMatch[0] : 'Unknown Location';
        } else {
            return 'Unknown Location';
        }
    } catch (error) {
        if (axios.isCancel(error)) {
            console.log('Reverse geocoding request canceled');
            throw new Error('Operation canceled by the user.');
        }
        console.error('Failed to perform reverse geocoding:', error);
        return 'Unknown Location';
    }
};

// Build history action with validation and error handling
const buildHistory = async () => {
    // Initialize AbortController
    abortController = new AbortController();
    const { signal } = abortController;

    // Start loading
    isLoading.value = true;

    const tracker = trackers.value.find(t => t._id === selectedTracker.value);

    try {
        // Validate that both timestamps are selected
        if (!fromTimestamp.value || !toTimestamp.value) {
            errorMessage.value = $t("MapHistory-ErrorStartEndRequired");
            return;
        }

        // Validate that the start date is not later than the end date
        if (new Date(fromTimestamp.value) > new Date(toTimestamp.value)) {
            errorMessage.value = $t("MapHistory-ErrorStartAfterEnd");
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
            // Check if the request has been aborted
            if (signal.aborted) throw new Error('Operation canceled by the user.');

            measurement.address = await getReverseGeocodingAddress(measurement.latitude, measurement.longitude);

            // Optional: Add a small delay to simulate processing time and allow cancellation
            await new Promise(resolve => setTimeout(resolve, 50));
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
            // Even when the checkbox is unchecked, use colored pins for the first and last measurements
            drawPinsForFirstAndLast(measurements);
        }

        const bounds = new google.maps.LatLngBounds();
        pathCoordinates.forEach(coord => bounds.extend(coord));
        map.fitBounds(bounds);
    } catch (error) {
        if (error.message === 'Operation canceled by the user.') {
            console.log('Build history was canceled.');
            errorMessage.value = $t("MapHistory-Canceled");
        } else {
            console.error('Error building history:', error);
            errorMessage.value = $t("MapHistory-BuildError");
        }
    } finally {
        // Stop loading
        isLoading.value = false;
        abortController = null; // Reset the controller
    }
};

// Cancel buildHistory method
const cancelBuildHistory = () => {
    if (abortController) {
        abortController.abort();
    }
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

watch(fromDate, (newFromDate) => {
    if (toDate.value && new Date(toDate.value) < newFromDate) {
        toDate.value = null; // Reset `toDate` if it's no longer valid
        toTimestamp.value = null; // Reset the timestamp too
    }
});

watch(selectedTracker, updateSelectedTrackerMeasurements);
</script>

<style scoped>
.cancel-button {
    padding: 10px 20px;
    background-color: #C19A6B;
    /* Match filters-button background */
    color: #1f1f1f;
    /* Match filters-button text color */
    border: none;
    border-radius: 18px;
    /* Match filters-button border-radius */
    cursor: pointer;
    font-size: 0.9rem;
    /* Consistent font size */
    transition: background-color 0.3s ease;
}

.cancel-button:hover {
    background-color: #8f6b3f;
    /* Darker shade on hover */
}

/* Adjustments for Dark Mode */
.dark-mode .cancel-button {
    background-color: #E69543;
    /* Match dark-mode filters-button background */
    color: #1f1f1f;
}

.dark-mode .cancel-button:hover {
    background-color: #bd7227;
    /* Darker shade on hover for dark mode */
}

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
    /* Allow vertical scrolling */
    overflow-x: auto;
    /* Allow horizontal scrolling */
    border: 1px solid #ddd;
    border-radius: 6px;
    background-color: #f9f9f9;
}

.measurements-list-header,
.measurement-item {
    display: grid;
    grid-template-columns: 50px 180px 100px 300px 100px 100px;
    /* Define column widths */
    padding: 5px 15px;
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
    /* Limit height for vertical scrolling */
    overflow-y: auto;
}

.map-loading-overlay {
    position: absolute;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background-color: rgba(255, 255, 255, 0.8);
    /* Semi-transparent background */
    display: flex;
    align-items: center;
    justify-content: center;
    z-index: 1000;
    /* Ensure it sits above the map but below global overlays */
    flex-direction: column;
    /* Stack text and button vertically */
    gap: 20px;
    /* Space between spinner, text, and button */
}

.loading-content {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 15px;
    /* Space between spinner, text, and button */
}

.loading-overlay {
    position: fixed;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background-color: rgba(255, 255, 255, 0.8);
    /* Semi-transparent background */
    display: flex;
    align-items: center;
    justify-content: center;
    z-index: 1000;
    /* Ensure it sits above other elements */
}

.spinner {
    border: 8px solid #f3f3f3;
    /* Light grey */
    border-top: 8px solid #3498db;
    /* Blue */
    border-radius: 50%;
    width: 60px;
    height: 60px;
    animation: spin 1s linear infinite;
}

/* Spinner Animation */
@keyframes spin {
    0% {
        transform: rotate(0deg);
    }

    100% {
        transform: rotate(360deg);
    }
}
</style>