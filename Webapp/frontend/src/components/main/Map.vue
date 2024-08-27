<script setup>
import { defineProps, defineEmits, ref, onMounted, watch, onUnmounted, computed, nextTick } from 'vue'

const props = defineProps({
  trackers: { type: Array, required: true },
  currentTracker: { type: Object, required: true }
})

const emit = defineEmits(['update-tracker'])

const mapElement = ref(null)
let map = null
let marker = null
const showDropdown = ref(false)

const initializeMap = () => {
  if (!mapElement.value) return

  const position = { lat: props.currentTracker.lat, lng: props.currentTracker.lng }

  if (map) {
    map.setCenter(position)
    if (marker) {
      marker.setPosition(position)
    } else {
      marker = new google.maps.Marker({ position, map, title: props.currentTracker.name })
    }
  } else {
    map = new google.maps.Map(mapElement.value, { center: position, zoom: 12 })
    marker = new google.maps.Marker({ position, map, title: props.currentTracker.name })
  }
}

onMounted(() => {
  initializeMap()
  document.addEventListener('click', handleClickOutside)
})

onUnmounted(() => {
  document.removeEventListener('click', handleClickOutside)
})

watch(() => props.currentTracker, initializeMap)

const tech = computed(() => (props.currentTracker.mode === 'Langzeittracking' ? 'LTE' : 'GPS'))

const toggleDropdown = () => {
  showDropdown.value = !showDropdown.value
}

const selectTracker = async (tracker) => {
  emit('update-tracker', tracker)
  await nextTick() // Warten auf den nächsten DOM-Update-Zyklus, bevor das Dropdown geschlossen wird
  showDropdown.value = false
}

const handleClickOutside = (event) => {
  if (showDropdown.value && !event.target.closest('.dropdown-menu') && !event.target.closest('.sub-box')) {
    showDropdown.value = false
  }
}
</script>

<template>
  <div class="box">
    <div class="flex-box small">
      <div class="sub-box current-tracker" @click="toggleDropdown">
        {{ props.currentTracker.name }}
        <div v-if="showDropdown" class="dropdown-menu">
          <div v-for="tracker in props.trackers" :key="tracker.name" @click="selectTracker(tracker)">
            {{ tracker.name }}
          </div>
        </div>
      </div>
      <div class="sub-box">{{ tech }}</div>
      <div class="sub-box">{{ props.currentTracker.mode }}</div>
    </div>
    <div class="flex-box large" ref="mapElement"></div>
    <div class="flex-box rest">
      <div>Latitude: {{ props.currentTracker.lat }}</div>
      <div>Longitude: {{ props.currentTracker.lng }}</div>
      <div>Temperature: {{ props.currentTracker.temperature || 'N/A' }} °C</div>
    </div>
  </div>
</template>

<style scoped>
.box {
  flex: 3;
  display: flex;
  flex-direction: column;
  padding: 0 20px;
}

.flex-box {
  background-color: rgba(0, 0, 0, 0.2);
}

.flex-box.small {
  height: 60px;
  display: flex;
  border-radius: 5px;
}

.flex-box.large {
  flex: 2;
}

.flex-box.rest {
  flex: 1;
  padding: 10px;
  background-color: transparent;
}

.sub-box {
  flex: 1;
  text-align: center;
  line-height: 60px;
  position: relative;
  cursor: pointer; /* Indikator, dass die Box klickbar ist */
}

.dropdown-menu {
  position: absolute;
  top: 100%;
  left: 50%;
  transform: translateX(-50%);
  background-color: #fff;
  border: 1px solid #ccc;
  z-index: 1000;
  width: 100%;
}

.dropdown-menu div {
  padding: 10px;
  cursor: pointer;
  text-align: center;
}

.dropdown-menu div:hover {
  background-color: #eee;
}
</style>
