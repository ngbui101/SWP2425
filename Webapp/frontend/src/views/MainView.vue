<script setup>
import { ref } from 'vue'
import UserMenu from '../components/main/UserMenu.vue'
import TrackerList from '../components/main/TrackerList.vue'
import MapComponent from '../components/main/MapComponent.vue'

const trackers = ref([
  { name: 'Tracker 1', mode: 'Langzeittracking', location: 'Berlin', lat: 52.52, lng: 13.405, temperature: 18 },
  { name: 'Tracker 2', mode: 'Echtzeittracking', location: 'München', lat: 48.1351, lng: 11.582, temperature: 22 },
  { name: 'Tracker 3', mode: 'Langzeittracking', location: 'Hamburg', lat: 53.5511, lng: 9.9937, temperature: 16 },
  { name: 'Tracker 4', mode: 'Echtzeittracking', location: 'Köln', lat: 50.9375, lng: 6.9603, temperature: 20 },
  { name: 'Tracker 5', mode: 'Langzeittracking', location: 'Frankfurt', lat: 50.1109, lng: 8.6821, temperature: 19 },
])

const currentTracker = ref(trackers.value[0])

const updateTracker = (tracker) => {
  currentTracker.value = tracker
}
</script>

<template>
  <div class="container">
    <UserMenu />
    <TrackerList :trackers="trackers" @update-tracker="updateTracker" />
    <MapComponent :trackers="trackers" :currentTracker="currentTracker" @update-tracker="updateTracker" />
  </div>
</template>

<style scoped>
.container {
  display: flex;
  height: 100vh;
}
</style>
