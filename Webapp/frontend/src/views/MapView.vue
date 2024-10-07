<template>
  <div class="map-container">
    <div class="details-panel">
      <!-- Dropdown and details for trackers -->
      <div class="dropdown">
        <button class="dropdown-button" @click="toggleDropdown">
          {{ currentTracker.name }}
          <span class="dropdown-arrow">&#9662;</span>
        </button>
        <div v-if="showDropdown" class="dropdown-menu">
          <div
            v-for="tracker in trackers"
            :key="tracker.id"
            @click="selectTracker(tracker)"
            class="dropdown-item"
          >
            {{ tracker.name }}
          </div>
        </div>
      </div>

      <!-- Display tracker details -->
      <div class="tracker-details">
        <p><strong>Modus:</strong> {{ currentTracker.modus }}</p>
        <p><strong>Location:</strong> {{ currentTracker.location }}</p>
        <p><strong>Latitude:</strong> {{ currentTracker.lat }}</p>
        <p><strong>Longitude:</strong> {{ currentTracker.lng }}</p>
        <p><strong>Battery:</strong> {{ currentTracker.battery }}%</p>
        <p><strong>Accuracy:</strong> {{ currentTracker.accuracy }} m</p>
        <p><strong>Temperature:</strong> {{ currentTracker.temperature }} °C</p>
        <p><strong>Humidity:</strong> {{ currentTracker.humidity }} %</p>

      </div>
    </div>

    <!-- Map panel where the Google Maps will be rendered -->
    <div ref="mapElement" class="map"></div>
  </div>
</template>

<script setup>
import { ref, onMounted, watch } from 'vue'
import { useTrackerStore } from '@/stores/trackerStore'

const store = useTrackerStore() // Use the same store

// Get all trackers and default to the first one
const trackers = store.trackers
const currentTracker = ref(trackers[0])

const showDropdown = ref(false)
const mapElement = ref(null)
let map = null
let marker = null

// Toggle dropdown for selecting trackers
const toggleDropdown = () => {
  showDropdown.value = !showDropdown.value
}

// Select the tracker and update map
const selectTracker = (tracker) => {
  currentTracker.value = tracker
  showDropdown.value = false
  initializeMap() // Reinitialize the map with new tracker data
}

// Load the Google Maps script dynamically
const loadGoogleMapsScript = () => {
  return new Promise((resolve, reject) => {
    if (window.google && window.google.maps) {
      resolve() // Maps API already loaded
    } else {
      const script = document.createElement('script')
      script.src = `https://maps.googleapis.com/maps/api/js?key=AIzaSyDzh7DicT6KmawobOi6iir27IFEQBsRhRo`;
      script.async = true
      script.defer = true
      script.onload = resolve
      script.onerror = reject
      document.head.appendChild(script)
    }
  })
}

// Initialize the map with the current tracker location
const initializeMap = () => {
  if (!mapElement.value) return

  const position = { lat: currentTracker.value.lat, lng: currentTracker.value.lng }

  if (map) {
    map.setCenter(position) // Update map center
    if (marker) {
      marker.setPosition(position) // Update marker position
    } else {
      marker = new google.maps.Marker({ position, map, title: currentTracker.value.name })
    }
  } else {
    map = new google.maps.Map(mapElement.value, {
      center: position,
      zoom: 12, // Set initial zoom level
    })
    marker = new google.maps.Marker({
      position,
      map,
      title: currentTracker.value.name,
    })
  }
}

// Initialize the map when the component mounts
onMounted(async () => {
  try {
    await loadGoogleMapsScript()
    initializeMap()
  } catch (error) {
    console.error('Failed to load Google Maps script:', error)
  }
})

// Watch for changes in the current tracker and update the map
watch(currentTracker, initializeMap)
</script>

<style scoped>
.map-container {
  display: flex;
  height: 100%;
}

.details-panel {
  flex: 0.3;
  max-width: 250px;
  padding: 20px;
  background-color: white;
  border: 1px solid #ddd;
  box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
}

.dropdown {
  margin-bottom: 20px;
}

.dropdown-button {
  background-color: #00543D;
  color: white;
  padding: 10px;
  border-radius: 4px; /* Optionale Abrundung der Ecken */
  cursor: pointer;
}

.dropdown-menu {
  background-color: white;
  border: 1px solid #ddd;
  box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
  position: absolute;
  z-index: 1;
}

.dropdown-item {
  padding: 10px;
  cursor: pointer;
}

.dropdown-item:hover {
  background-color: rgba(0, 84, 61, 0.1);
}

.tracker-details {
  margin-top: 20px;
}

.map {
  flex: 1;
  height: 400px;
}
</style>
