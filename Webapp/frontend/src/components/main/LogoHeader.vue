<template>
  <div class="header">
    <div class="logo-container">
      <img src="/src/assets/logo-transparent.png" alt="Company Logo" class="logo" />
    </div>
    <div class="email-placeholder">Logged in as: {{ user.email }}</div>
  </div>
</template>

<script setup lang="ts">
import { computed, ref, onMounted } from 'vue';
import { useAuthStore } from '@/stores/auth';
import { useI18n } from 'vue-i18n'; // Import the useI18n function
const authStore = useAuthStore();
const user = computed(() => authStore.userDetail);
const { locale } = useI18n();
// Computed property for user email

onMounted(async () => {
  await authStore.getUser();
  if (user.value?.language) {
    locale.value = user.value.language; // Set the locale to the user's language
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
  background-color: #ffffff;
  padding: 10px;
  border-bottom: 1px solid #ddd;
  height: 70px; /* Explicit height to match navbar's top value */
  font-family: 'Poppins', sans-serif; /* Set font to Poppins */
  position: relative;
}

.logo-container {
  position: absolute;
  left: 50%;
  transform: translateX(-50%);
  height: 100%;
  display: flex;
  align-items: center;
}

.logo {
  height: 50px;
  width: auto;
}

.email-placeholder {
  position: absolute;
  bottom: 0px;
  right: 10px;
  font-size: 14px;
  color: #333; /* Text color */
  
}
@media (max-width: 768px) {
  .email-placeholder {
    display: none; /* Hide the "Logged in as..." text on mobile screens */
  }
}
</style>
