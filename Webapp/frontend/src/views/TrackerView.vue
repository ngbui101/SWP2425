<script setup>
import { defineProps, defineEmits } from 'vue'

const props = defineProps({
  trackers: { type: Array, required: true }
})

const emit = defineEmits(['update-tracker'])

const selectTracker = (tracker) => {
  emit('update-tracker', tracker)
}
</script>

<template>
  <div class="box">
    <div class="list-header">Mein Tracker</div>
    <ul class="tracker-list">
      <li v-for="tracker in props.trackers" :key="tracker.name" class="tracker-item" @click="selectTracker(tracker)">
        <strong>{{ tracker.name }}</strong> - Modus: {{ tracker.mode }} - Standort: {{ tracker.location }}
      </li>
    </ul>
  </div>
</template>

<style scoped>
.box {
  flex: 2;
}

.list-header {
  height: 60px;
  line-height: 60px;
  background-color: rgba(0, 0, 0, 0.2);
  border-radius: 5px;
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
  cursor: pointer;
}

.tracker-item:hover {
  background-color: rgba(0, 0, 0, 0.2);
}
</style>
