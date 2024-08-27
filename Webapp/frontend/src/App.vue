<script setup>
import { ref } from 'vue'
import Box1 from './components/Box1.vue'
import Box2 from './components/Box2.vue'
import Box3 from './components/Box3.vue'

// Tracker-Daten in App.vue
const trackers = ref([
  { name: 'Tracker 1', mode: 'Langzeittracking', location: 'Berlin', lat: 52.5200, lng: 13.4050, temperature: 18 },
  { name: 'Tracker 2', mode: 'Echtzeittracking', location: 'München', lat: 48.1351, lng: 11.5820, temperature: 22 },
  { name: 'Tracker 3', mode: 'Langzeittracking', location: 'Hamburg', lat: 53.5511, lng: 9.9937, temperature: 16 },
  { name: 'Tracker 4', mode: 'Echtzeittracking', location: 'Köln', lat: 50.9375, lng: 6.9603, temperature: 20 },
  { name: 'Tracker 5', mode: 'Langzeittracking', location: 'Frankfurt', lat: 50.1109, lng: 8.6821, temperature: 19 },
])

</script>

<template>
  <div class="container">
    <Box1 />
    <Box2 :trackers="trackers" />
    <Box3 :trackers="trackers" />
  </div>
</template>

<style scoped>
.container {
  display: flex;
  height: 100vh;
}

.box {
  color: rgb(11, 0, 0);
  font-size: 1.5rem;
  padding: 5px;
}
</style>
