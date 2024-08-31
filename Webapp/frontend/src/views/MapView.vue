<template>
  <div class="outer-container">
    <div class="map-container">
      <div class="details-panel">
        <div class="dropdown">
          <button class="dropdown-button" @click="toggleDropdown">
            {{ currentTracker.name }} 
            <span class="dropdown-arrow">&#9662;</span>
          </button>
          <div class="dropdown-divider"></div>
          <div v-if="showDropdown" class="dropdown-menu">
            <div
              v-for="tracker in trackers"
              :key="tracker.name"
              @click="selectTracker(tracker)"
              class="dropdown-item"
            >
              {{ tracker.name }}
            </div>
          </div>
        </div>

        <div class="tracker-details">
          <p><strong>Mode:</strong> {{ currentTracker.mode }}</p>
          <p><strong>Location:</strong> {{ currentTracker.location }}</p>
          <p><strong>Latitude:</strong> {{ currentTracker.lat }}</p>
          <p><strong>Longitude:</strong> {{ currentTracker.lng }}</p>
          <p><strong>Temperature:</strong> {{ currentTracker.temperature }} °C</p>
          <p><strong>Battery:</strong> {{ currentTracker.battery }}%</p>
          <p><strong>Accuracy:</strong> {{ currentTracker.accuracy }} m</p>
        </div>
      </div>
      <div class="map-panel">
        <div ref="mapElement" class="map"></div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, watch } from 'vue';

const trackers = ref([
  { name: 'Tracker 1', mode: 'Langzeittracking', location: 'Berlin', lat: 52.52, lng: 13.405, temperature: 18, battery: 90, accuracy: 10 },
  { name: 'Tracker 2', mode: 'Echtzeittracking', location: 'München', lat: 48.1351, lng: 11.582, temperature: 22, battery: 85, accuracy: 8 },
  { name: 'Tracker 3', mode: 'Langzeittracking', location: 'Hamburg', lat: 53.5511, lng: 9.9937, temperature: 16, battery: 80, accuracy: 12 },
  { name: 'Tracker 4', mode: 'Echtzeittracking', location: 'Köln', lat: 50.9375, lng: 6.9603, temperature: 20, battery: 95, accuracy: 5 },
  { name: 'Tracker 5', mode: 'Langzeittracking', location: 'Frankfurt', lat: 50.1109, lng: 8.6821, temperature: 19, battery: 78, accuracy: 5 },
]);

const currentTracker = ref(trackers.value[0]);
const showDropdown = ref(false);
const mapElement = ref(null);
let map = null;
let marker = null;

const toggleDropdown = () => {
  showDropdown.value = !showDropdown.value;
};

const selectTracker = (tracker) => {
  currentTracker.value = tracker;
  showDropdown.value = false;
};

const loadGoogleMapsScript = () => {
  return new Promise((resolve, reject) => {
    if (window.google && window.google.maps) {
      resolve();
    } else {
      const script = document.createElement('script');
      script.src = `https://maps.googleapis.com/maps/api/js?key=AIzaSyDzh7DicT6KmawobOi6iir27IFEQBsRhRo`;
      script.async = true;
      script.defer = true;
      script.onload = resolve;
      script.onerror = reject;
      document.head.appendChild(script);
    }
  });
};

const initializeMap = () => {
  if (!mapElement.value) return;

  const position = { lat: currentTracker.value.lat, lng: currentTracker.value.lng };

  if (map) {
    map.setCenter(position);
    if (marker) {
      marker.setPosition(position);
    } else {
      marker = new google.maps.Marker({ position, map, title: currentTracker.value.name });
    }
  } else {
    map = new google.maps.Map(mapElement.value, { center: position, zoom: 12 });
    marker = new google.maps.Marker({ position, map, title: currentTracker.value.name });
  }
};

onMounted(async () => {
  try {
    await loadGoogleMapsScript();
    initializeMap();
  } catch (error) {
    console.error('Failed to load Google Maps script:', error);
  }
});

watch(currentTracker, initializeMap);
</script>

<style scoped>
.outer-container {
  width: 150vh;
  height: 60vh;
  margin-left: auto;
  margin-right: auto;
}

.map-container {
  display: flex;
  height: 100%;
  align-items: stretch;
  margin: 0;
  padding: 0;
  overflow: hidden;
}

.details-panel {
  flex: 0.2;
  max-width: 250px;
  padding: 20px;
  background-color: #f0f0f0;
  border: 1px solid #ddd;
  box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
  height: 100%;
  box-sizing: border-box;
  overflow-y: auto;
}

.tracker-details {
  flex-grow: 1;
}

.map-panel {
  flex: 1;
  position: relative;
  height: 100%;
  margin: 0;
  padding: 0;
  background-color: transparent;
  border: none;
  box-shadow: none;
  overflow: hidden;
}

.map {
  width: 100%;
  height: 100%;
  margin: 0;
  padding: 0;
  box-sizing: border-box;
  border-radius: 0;
}

.dropdown {
  position: relative;
}

.dropdown-button {
  padding: 10px;
  background-color: #fff;
  color: rgb(0, 0, 0);
  border: none;
  border-radius: 5px;
  cursor: pointer;
  display: flex;
  align-items: center;
  justify-content: space-between;
  width: 100%;
  font-size: 16px;
  position: relative;
  font-family: 'Poppins', sans-serif;
}

.dropdown-arrow {
  margin-left: 10px;
  font-size: 12px;
}

.dropdown-divider {
  width: 100%;
  height: 3px;
  background-color: #ddd;
  margin-top: 10px;
}

.dropdown-menu {
  position: absolute;
  top: 100%;
  left: 0;
  background-color: white;
  border: 1px solid #ddd;
  border-radius: 5px;
  width: 100%;
  box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
  z-index: 10;
  max-height: 150px;
  overflow-y: auto;
}

.dropdown-item {
  padding: 10px;
  cursor: pointer;
}

.dropdown-item:hover {
  background-color: #f0f0f0;
}
</style>
