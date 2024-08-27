<script setup>
import { defineProps, ref, onMounted, watch, onUnmounted } from 'vue'
import { computed } from 'vue'

// Props definieren, um Tracker-Daten von App.vue zu empfangen
const props = defineProps({
    trackers: {
        type: Array,
        required: true
    }
})

// Logik zur Verwaltung des Dropdown-Menüs und der Auswahl des aktuellen Trackers
const currentTracker = ref(props.trackers[0]) // Initial ausgewählter Tracker
const showDropdown = ref(false) // Sichtbarkeit des Dropdown-Menüs
const mapElement = ref(null)
let map = null // Store the map instance
let marker = null // Store the marker instance

const initializeMap = () => {
    if (mapElement.value) {
        if (map) {
            // Update map center and marker if map instance already exists
            map.setCenter({ lat: currentTracker.value.lat, lng: currentTracker.value.lng })
            if (marker) {
                marker.setPosition({ lat: currentTracker.value.lat, lng: currentTracker.value.lng })
            } else {
                marker = new google.maps.Marker({
                    position: { lat: currentTracker.value.lat, lng: currentTracker.value.lng },
                    map: map,
                    title: currentTracker.value.name
                })
            }
        } else {
            // Create a new map instance
            map = new google.maps.Map(mapElement.value, {
                center: { lat: currentTracker.value.lat, lng: currentTracker.value.lng },
                zoom: 12
            })
            marker = new google.maps.Marker({
                position: { lat: currentTracker.value.lat, lng: currentTracker.value.lng },
                map: map,
                title: currentTracker.value.name
            })
        }
    }
}

onMounted(() => {
    initializeMap()
})

watch(currentTracker, () => {
    initializeMap()
})

const tech = computed(() => {
    return currentTracker.value.mode === 'Langzeittracking' ? 'LTE' : 'GPS'
})
const mode = computed(() => {
    return currentTracker.value.mode
})
const latitude = computed(() => currentTracker.value.lat)
const longitude = computed(() => currentTracker.value.lng)
const temperature = computed(() => currentTracker.value.temperature || 'N/A') // Default to 'N/A' if temperature is not available

const toggleDropdown = () => {
    showDropdown.value = !showDropdown.value
}

const selectTracker = (tracker) => {
    currentTracker.value = tracker
    showDropdown.value = false // Dropdown-Menü schließen nach der Auswahl
}

// Klick außerhalb des Dropdown-Menüs schließen das Dropdown-Menü
const handleClickOutside = (event) => {
    if (showDropdown.value && !event.target.closest('.dropdown-menu') && !event.target.closest('.current-tracker')) {
        showDropdown.value = false
    }
}

onMounted(() => {
    document.addEventListener('click', handleClickOutside)
})

onUnmounted(() => {
    document.removeEventListener('click', handleClickOutside)
})

</script>

<template>
    <div class="box box3">
        <div class="flex-box box-small">
            <div class="sub-box current-tracker" @click="toggleDropdown">
                {{ currentTracker.name }}
                <div v-if="showDropdown" class="dropdown-menu">
                    <div v-for="tracker in props.trackers" :key="tracker.name" class="dropdown-item"
                        @click="selectTracker(tracker)">
                        {{ tracker.name }}
                    </div>
                </div>
            </div>
            <div class="sub-box tech">{{ tech }}</div>
            <div class="sub-box mode">{{ mode }}</div>
        </div>
        <div class="flex-box box-large" ref="mapElement"></div>
        <div class="flex-box box-rest">
            <div>Latitude: {{ latitude }}</div>
            <div>Longitude: {{ longitude }}</div>
            <div>Temperature: {{ temperature }} °C</div>
        </div>
    </div>
</template>

<style scoped>
.box3 {
    flex: 3;
    display: flex;
    flex-direction: column;
    padding: 20px;
}

.flex-box {
    background-color: rgba(0, 0, 0, 0.2);
}

.box-small {
    height: 60px;
    display: flex;
    position: relative;
}

.box-large {
    flex: 2;
    background-color: green;
}

.box-rest {
    flex: 1;
    background-color: transparent; /* Ensure no background color */
    padding: 10px; /* Optional: Add some padding for better readability */
}

.sub-box {
    flex: 1;
    background-color: rgba(0, 0, 0, 0.1);
    margin-right: 5px;
    text-align: center;
    line-height: 60px;
    font-size: 0.8rem;
    position: relative;
}

.sub-box:last-child {
    margin-right: 0;
}

.dropdown-menu {
    position: absolute;
    top: 100%;
    left: 0;
    background-color: #fff;
    color: #000;
    border: 1px solid #ccc;
    width: 100%;
    max-height: 200px;
    overflow-y: auto;
    z-index: 1000;
}

.dropdown-item {
    padding: 10px;
    cursor: pointer;
}

.dropdown-item:hover {
    background-color: #eee;
}
</style>
