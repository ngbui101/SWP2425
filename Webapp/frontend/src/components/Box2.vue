<script setup>
import { defineProps } from 'vue'

// Props definieren, um Tracker-Daten von App.vue zu empfangen
const props = defineProps({
  trackers: {
    type: Array,
    required: true
  }
})
</script>

<template>
  <div class="box box2">
    <div class="list-header">
      <h2>Mein Tracker</h2>
    </div>
    <ul class="tracker-list">
      <li v-for="(tracker, index) in props.trackers" :key="index" class="tracker-item">
        <strong>{{ tracker.name }}</strong> - Modus: {{ tracker.mode }} - Standort: {{ tracker.location }}
      </li>
    </ul>
  </div>
</template>

<style scoped>
.box2 {
  flex: 2;
}

.list-header {
  background-color: rgba(0, 0, 0, 0.2);
  padding: 10px;
  border-radius: 5px;
  margin-bottom: 10px;
  text-align: center;
}

.tracker-list {
  list-style: none;
  padding: 0;
}

.tracker-item {
  background-color: rgba(0, 0, 0, 0.1);
  padding: 10px;
  border-radius: 5px;
  margin: 10px 0;
  font-size: 1.2rem;
}
</style>
