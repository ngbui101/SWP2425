<script setup>
import { computed, ref, onMounted } from 'vue';
import { useRouter } from 'vue-router';
import { useAuthStore } from '@/stores/auth';
import SettingsPopup from '@/views/SettingsView.vue';

const showMenu = ref(false);
const showModal = ref(false); // Reactive property to control the modal visibility
const router = useRouter();
const authStore = useAuthStore();
const user = computed(() => authStore.userDetail);

onMounted(async () => {
  await authStore.getUser();
});

const toggleMenu = () => {
  showMenu.value = !showMenu.value;
};

// Logout function
async function logout() {
  try {
    await authStore.logout();
    router.replace({ name: 'login' }); // Redirect to the login page after logout
  } catch (err) {

  }
}

// Open the modal
const openModal = () => {
  showModal.value = true;
};

// Close the modal
const closeModal = () => {
  showModal.value = false;
};
</script>

<template>
  <div class="box box1">
    <div class="dropdown">
      <div class="drop-down-button" @click="toggleMenu">
        ☰ {{ user?.email }} <!-- Conditionally render the username if available, or fallback to 'User' -->
      </div>
      <ul v-if="showMenu" class="dropdown-menu">
        <li @click="openModal">Daten ändern</li> <!-- Open the modal here -->
        <li @click="logout">Logout</li> <!-- Attach the logout function here -->
      </ul>
    </div>

    <!-- Include the SettingsModal component and bind its visibility -->
    <SettingsPopup :showModal="showModal" :user="user" @close-modal="closeModal" />
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
  height: 40px;
  cursor: pointer;
  background-color: rgba(0, 0, 0, 0.2);
  padding: 10px;
  border-radius: 5px;
}

.dropdown-menu {
  list-style: none;
  margin: 0;
  padding: 0;
  background-color: rgba(0, 0, 0, 0.2);
  border-radius: 0 0 5px 5px;
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
