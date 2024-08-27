<!-- Box1.vue -->
<script setup>
import { ref } from 'vue'

const showMenu = ref(false)

const toggleMenu = () => {
  showMenu.value = !showMenu.value
}
</script>

<template>
  <div class="box box1">
    <div class="dropdown">
      <div class="drop-down-button" @click="toggleMenu">
        Username ☰ <!-- Burger-Menü Symbol -->
      </div>
      <ul v-if="showMenu" class="dropdown-menu">
        <li>Daten ändern</li>
        <li>Logout</li>
      </ul>
    </div>
  </div>
</template>

<style scoped>
.box1 {
  flex: 1;
}

.dropdown {
  position: relative;
  text-align: center;
}

.drop-down-button {
  cursor: pointer;
  background-color: rgba(0, 0, 0, 0.5);
  padding: 10px;
  border-radius: 5px;
}

.dropdown-menu {
  list-style: none;
  margin: 10px 0 0;
  padding: 0;
  background-color: rgba(0, 0, 0, 0.5);
  border-radius: 5px;
  position: absolute;
  top: 100%;
  left: 50%;
  transform: translateX(-50%);
  min-width: 150px;
}

.dropdown-menu li {
  padding: 10px;
  cursor: pointer;
  text-align: center;
}

.dropdown-menu li:hover {
  background-color: rgba(255, 255, 255, 0.2);
}
</style>
^s