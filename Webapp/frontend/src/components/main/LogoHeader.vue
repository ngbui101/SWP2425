<template>
  <div class="header">
    <img src="/src/assets/logo-transparent.png" alt="Company Logo" class="logo" />
    <div class="email-placeholder">Logged in as: {{ user.email }}</div>
  </div>
</template>

<script setup lang="ts">
import { computed, ref, onMounted } from 'vue';
import { useAuthStore } from '@/stores/auth';

const authStore = useAuthStore();
const user = computed(() => authStore.userDetail);

// Computed property for user email


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
  background-color: #ffffff;
  padding: 10px;
  border-bottom: 1px solid #ddd;
  height: 70px; /* Explicit height to match navbar's top value */
  font-family: 'Poppins', sans-serif; /* Set font to Poppins */
}

.logo {
  height: 50px;
  width: auto;
}

.email-placeholder {
  position: absolute;
  bottom: 10px;
  left: 10px;
  font-size: 14px;
  color: #333; /* Text color */
}
</style>
