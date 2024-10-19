<template>
  <div :class="['header', (user.template ?? 'default') === 'dark' ? 'dark-mode' : '']" ref="tour1">
    <!-- Logo Section -->
    <div class="logo-container">
      <router-link to="/">
        <!-- First image for text -->
        <img v-if="user.template === 'dark'" src="/src/assets/logo-text-dark.png" alt="Company Logo Dark"
          class="logo-text" />
        <img v-else src="/src/assets/logo-text.png" alt="Company Logo" class="logo-text" />

        <!-- Second image for icon -->
        <img src="/src/assets/headerlogo_icon.gif" alt="Company Logo Icon" class="logo-icon" />
      </router-link>
    </div>

    <!-- Logged in as Section -->
    <div class="email-placeholder">Logged in as: {{ user.email }}</div>
  </div>
</template>

<script setup>
import { computed, onMounted } from 'vue';
import { useAuthStore } from '@/stores/auth';

import { tour1 } from '@/routes/tourRefs.js';  // Import the shared logoRef

const authStore = useAuthStore();
const user = computed(() => authStore.userDetail);


onMounted(async () => {
  await authStore.getUser();


});
</script>

<style scoped>
@import url('https://fonts.googleapis.com/css2?family=Poppins:wght@400&display=swap');

.header {
  position: sticky;
  top: 0;
  z-index: 1000;
  display: flex;
  justify-content: center;
  align-items: center;
  background: linear-gradient(180deg, rgba(255, 255, 255, 1) 20%, rgb(212, 236, 206) 80%);
  padding: 10px;

  height: 100px;
  font-family: 'Poppins', sans-serif;
  position: relative;
}

.header.dark-mode {
  background: linear-gradient(135deg, #1e1e1e 0%, #141414 50%, #1e1e1e 100%);
}



.logo-container {
  position: absolute;
  left: 50%;
  transform: translateX(-50%);
  height: 100%;
  display: flex;
  align-items: center;
  white-space: nowrap;
  /* Prevent the text and icon from wrapping */
}

.logo-text,
.logo-icon {
  height: 50px;
  width: auto;
  flex-shrink: 0;
  /* Prevent shrinking */
}

.logo-icon {
  margin-left: 10px;
  /* Adds some spacing between text and icon */
}

.email-placeholder {
  position: absolute;
  bottom: 0px;
  right: 10px;
  font-size: 14px;
  color: #333;
}

.header.dark-mode .email-placeholder {
  color: #5A976D;
}


@media (max-width: 768px) {
  .email-placeholder {
    display: none;
  }
}
</style>
