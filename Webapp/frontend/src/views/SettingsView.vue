<template>
  <div class="settings-container">
    <div class="settings-content">
      <!-- Settings Header -->
      <div class="settings-header">
        <h2 class="settings-title">{{ $t('SETTINGSVIEW-settings') }}</h2>
      </div>
      <div class="settings-body">
        <!-- Email Settings Card -->
        <div class="settings-card">
          <h3 class="card-title">{{ $t('SETTINGSVIEW-email_settings') }}</h3>
          <p>{{ $t('SETTINGSVIEW-current_email') }}: {{ user.email }}</p>
          <form @submit.prevent="updateEmail" class="settings-form">
            <label for="newEmail" class="form-label">{{ $t('SETTINGSVIEW-new_email') }}</label>
            <input v-model="newEmail" id="newEmail" type="email" class="form-input" :placeholder=" $t('SETTINGSVIEW-new_email')" required>
            <label for="confirmNewEmail" class="form-label">{{ $t('SETTINGSVIEW-confirm_email') }}</label>
            <input v-model="confirmNewEmail" id="confirmNewEmail" type="email" class="form-input" :placeholder="$t('SETTINGSVIEW-confirm_email')" required>
            <button type="submit" class="form-submit-button">{{ $t('SETTINGSVIEW-save') }}</button>
          </form>
        </div>

        <!-- Password Settings Card -->
        <div class="settings-card">
          <h3 class="card-title">{{ $t('SETTINGSVIEW-password_settings') }}</h3>
          <form @submit.prevent="updatePassword" class="settings-form">
            <label for="oldPassword" class="form-label">{{ $t('SETTINGSVIEW-current_password') }}</label>
            <input v-model="oldPassword" id="oldPassword" type="password" class="form-input" :placeholder="$t('SETTINGSVIEW-current_password')" required>
            <label for="newPassword" class="form-label">{{ $t('SETTINGSVIEW-new_password') }}</label>
            <input v-model="newPassword" id="newPassword" type="password" class="form-input" placeholder="New Password" required>
            <label for="confirmNewPassword" class="form-label">{{ $t('SETTINGSVIEW-confirm_new_password') }}</label>
            <input v-model="confirmNewPassword" id="confirmNewPassword" type="password" class="form-input" placeholder="Confirm New Password" required>
            <button type="submit" class="form-submit-button">{{ $t('SETTINGSVIEW-save') }}</button>
          </form>
        </div>

        <!-- Phone Number Settings Card -->
        <div class="settings-card">
          <h3 class="card-title">{{ $t('SETTINGSVIEW-phone_number_settings') }}</h3>
          <p>
  {{ user.number
    ? $t('SETTINGSVIEW-current_phone_number', { number: user.number })
    : $t('SETTINGSVIEW-no_phone_number') }}
</p>

          <form @submit.prevent="updatePhoneNumber" class="settings-form">
            <label for="phoneNumber" class="form-label">{{ $t('SETTINGSVIEW-phone_number') }}</label>
            <input v-model="phoneNumber" id="phoneNumber" type="tel" class="form-input" :placeholder="$t('SETTINGSVIEW-enter_phone_number')" required>
            <button type="submit" class="form-submit-button">{{ $t('SETTINGSVIEW-save') }}</button>
          </form>
        </div>

        <!-- Language Settings Card -->
        <div class="settings-card">
          <h3 class="card-title">{{ $t('SETTINGSVIEW-language_settings') }}</h3>
          <form @submit.prevent="updateLanguage" class="settings-form">
            <label for="language" class="form-label">{{ $t('SETTINGSVIEW-select_language') }}</label>
            <select v-model="selectedLanguage" id="language" class="form-input">
              <option value="EN">{{ $t('SETTINGSVIEW-english') }}</option>
              <option value="DE">{{ $t('SETTINGSVIEW-german') }}</option>
            </select>
            <!-- Theme Selection -->
    <label for="theme" class="form-label">{{ $t('SETTINGSVIEW-select_color_scheme') }}</label>
    <select v-model="selectedTheme" id="theme" class="form-input">
      <option value="light">{{ $t('SETTINGSVIEW-light') }}</option>
      <option value="dark">{{ $t('SETTINGSVIEW-dark') }}</option>
    </select>
            <button type="submit" class="form-submit-button">{{ $t('SETTINGSVIEW-save') }}</button>
          </form>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed, onMounted } from 'vue';
import { useAuthStore } from '@/stores/auth'; // Adjust the path as needed
import { useI18n } from 'vue-i18n';
// Initialize the auth store
const authStore = useAuthStore();

// Computed property for user details
const user = computed(() => authStore.userDetail);
const { locale } = useI18n();
// Fetch the user details when the component is mounted
onMounted(async () => {
  await authStore.getUser();
});

// Form data refs
const newEmail = ref('');
const confirmNewEmail = ref('');
const oldPassword = ref('');
const newPassword = ref('');
const confirmNewPassword = ref('');
const phoneNumber = ref('');
const selectedLanguage = ref('EN'); // Default to English
const selectedTheme = ref('light'); // Default to Light theme
// Method to close the modal
const emit = defineEmits(['close-modal']);
const closeModal = () => {
  emit('close-modal');
};

// Form submission handlers
const updateEmail = async () => {
  if (newEmail.value !== confirmNewEmail.value) {
    alert('Emails do not match');
    return;
  }
  try {
    await authStore.updateEmail(newEmail.value);
    alert('Email updated successfully');
    closeModal();
  } catch (error) {
    alert(`Error updating email: ${error.message}`);
  }
};

const updatePassword = async () => {
  if (newPassword.value !== confirmNewPassword.value) {
    alert('Passwords do not match');
    return;
  }
  // Handle password update logic
};

const updatePhoneNumber = async () => {
  // Handle phone number update logic
};

const updateLanguage = async () => {
  
  try {
    await authStore.updateLanguage(selectedLanguage.value);
    alert('Language updated successfully');
    locale.value = selectedLanguage.value;
  } catch (error) {
    alert(`Error updating Language: ${error.message}`);
  }
};


</script>

<style scoped>
 @import url('https://fonts.googleapis.com/css2?family=Poppins:wght@200;300;400;500;600;700&display=swap');

 body {
  font-family: 'Poppins', sans-serif;
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

.settings-container {
  display: flex;
  align-items: flex-start;
  justify-content: center; /* Center the settings container horizontally */
  padding: 20px;
  
  
}

.settings-content {
  background-color: white;
  
  
  /* Set a max-width to control the container width */
  border-radius: 8px;
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.4);
  padding: 20px;
  margin: 20px; /* Add some margin around the content to ensure it doesn't touch the screen edges */
}

.settings-header {
  display: flex;
  justify-content:center;
  align-items: center;
  margin-bottom: 20px;
}

.settings-title {
  font-size: 24px;
  font-weight: 600;
}

.settings-body {
  display: flex;
  flex-direction: row;
  gap: 20px;
  flex-wrap: wrap;
  justify-content: space-between;
}

.settings-card {
  flex: 1;
  min-width: 250px;
  border: 1px solid #e0e0e0;
  border-radius: 8px;
  padding: 20px;
  background: #fafafa;
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  
}

.card-title {
  font-size: 18px;
  font-weight: 600;
  margin-bottom: 10px;
}

.settings-form {
  display: flex;
  flex-direction: column;
  gap: 10px;
  
}

.form-label {
  font-size: 14px;
  font-weight: 500;
}

.form-input {
  border: 1px solid #d0d0d0;
  border-radius: 4px;
  padding: 10px;
  font-size: 14px;
  width: 100%;
  box-sizing: border-box; /* Ensure padding does not affect the width */
}

.form-submit-button {
  background-color: #00543D;
  color: white;
  border: none;
  padding: 10px;
  border-radius: 4px;
  cursor: pointer;
  font-size: 14px;
  font-weight: 500;
}

.form-submit-button:hover {
  background-color: #009579;
}

@media (max-width: 768px) {
  .settings-body {
    flex-direction: column;
  }
}

</style>
