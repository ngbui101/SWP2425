<template>
  <div ref="tour1" class="header">
    <!-- Logo Section -->
    <div class="logo-container">
      <router-link to="/">
        <!-- First image for text -->
        <img src="/src/assets/logo-text.png" alt="Company Logo Text" class="logo-text" />
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
import { useI18n } from 'vue-i18n';
import { tour1 } from '@/routes/tourRefs.js';  // Import the shared logoRef

const authStore = useAuthStore();
const user = computed(() => authStore.userDetail);
const { locale } = useI18n();

onMounted(async () => {
  await authStore.getUser();
  if (user.value?.language) {
    locale.value = user.value.language;
  }
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

  height: 70px;
  font-family: 'Poppins', sans-serif;
  position: relative;
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

@media (max-width: 768px) {
  .email-placeholder {
    display: none;
  }
}
</style>
