<template>
  <div :class="['settings-container', (user.template ?? 'default') === 'dark' ? 'dark-mode' : '']">
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
              <input v-model="newEmail" id="newEmail" type="email" class="form-input"
                :placeholder="$t('SETTINGSVIEW-new_email')" required>
              <label for="confirmNewEmail" class="form-label">{{ $t('SETTINGSVIEW-confirm_email') }}</label>
              <input v-model="confirmNewEmail" id="confirmNewEmail" type="email" class="form-input"
                :placeholder="$t('SETTINGSVIEW-confirm_email')" required>
              <button type="submit" class="form-submit-button">{{ $t('SETTINGSVIEW-save') }}</button>
            </form>
          </div>

          <!-- Password Settings Card -->
          <div class="settings-card">
            <h3 class="card-title">{{ $t('SETTINGSVIEW-password_settings') }}</h3>
            <form @submit.prevent="updatePassword" class="settings-form">
              <label for="oldPassword" class="form-label">{{ $t('SETTINGSVIEW-current_password') }}</label>
              <input v-model="oldPassword" id="oldPassword" type="password" class="form-input"
                :placeholder="$t('SETTINGSVIEW-current_password')" required>
              <label for="newPassword" class="form-label">{{ $t('SETTINGSVIEW-new_password') }}</label>
              <input v-model="newPassword" id="newPassword" type="password" class="form-input"
                placeholder="New Password" required>
              <label for="confirmNewPassword" class="form-label">{{ $t('SETTINGSVIEW-confirm_new_password') }}</label>
              <input v-model="confirmNewPassword" id="confirmNewPassword" type="password" class="form-input"
                placeholder="Confirm New Password" required>
              <button type="submit" class="form-submit-button">{{ $t('SETTINGSVIEW-save') }}</button>
            </form>
          </div>

          <!-- Phone Number Settings Card -->
          <div class="settings-card">
            <h3 class="card-title">{{ $t('SETTINGSVIEW-phone_number_settings') }}</h3>
            <p>
              {{ user.number
                ? $t('SETTINGSVIEW-current_phone_number')
                : $t('SETTINGSVIEW-no_phone_number') }}
            </p>

            <!-- If user.number exists, display the phone number in a separate <p> tag -->
            <p v-if="user.number"><strong>{{ user.number }}</strong></p>

            <form @submit.prevent="updatePhoneNumber" class="settings-form">
              <label for="phoneNumber" class="form-label">{{ $t('SETTINGSVIEW-phone_number') }}</label>
              <input v-model="phoneNumber" id="phoneNumber" type="tel" class="form-input"
                :placeholder="$t('SETTINGSVIEW-enter_phone_number')" required>
              <button type="submit" class="form-submit-button">{{ $t('SETTINGSVIEW-save') }}</button>
            </form>
          </div>

          <!-- Language Settings Card -->
          <div class="settings-card">
            <h3 class="card-title">{{ $t('SETTINGSVIEW-language_settings') }}</h3>
            <form @submit.prevent="updateSettings" class="settings-form"> <!-- Update method here -->
              <label for="language" class="form-label">{{ $t('SETTINGSVIEW-select_language') }}</label>
              <select v-model="selectedLanguage" id="language" class="form-input">
                <option value="EN">{{ $t('SETTINGSVIEW-english') }}</option>
                <option value="DE">{{ $t('SETTINGSVIEW-german') }}</option>
              </select>

              <!-- Theme Selection -->
              <label for="theme" class="form-label">{{ $t('SETTINGSVIEW-select_color_scheme') }}</label>
              <select v-model="selectedTheme" id="theme" class="form-input">
                <option value="default">{{ $t('SETTINGSVIEW-light') }}</option>
                <option value="dark">{{ $t('SETTINGSVIEW-dark') }}</option>
              </select>

              <button type="submit" class="form-submit-button">{{ $t('SETTINGSVIEW-save') }}</button>
            </form>
          </div>


          <!-- Notification Settings Card -->
          <div class="settings-card">
            <h3 class="card-title">{{ $t('SETTINGSVIEW-notification_settings') }}</h3>
            <form @submit.prevent="updateNotificationSettings" class="settings-form">
              <label class="form-label">
                <input type="checkbox" v-model="notificationSettings.smsBatteryLow">
                {{ $t('SETTINGSVIEW-sms_battery_low') }}
              </label>
              <label class="form-label">
                <input type="checkbox" v-model="notificationSettings.smsGeofenceAlert">
                {{ $t('SETTINGSVIEW-sms_geofence_alert') }}
              </label>
              <label class="form-label">
                <input type="checkbox" v-model="notificationSettings.emailBatteryLow">
                {{ $t('SETTINGSVIEW-email_battery_low') }}
              </label>
              <label class="form-label">
                <input type="checkbox" v-model="notificationSettings.emailGeofenceAlert">
                {{ $t('SETTINGSVIEW-email_geofence_alert') }}
              </label>
              <button type="submit" class="form-submit-button">{{ $t('SETTINGSVIEW-save') }}</button>
            </form>
          </div>
        </div>
      </div>
    </div>

  </div>
</template>
<script setup lang="ts">
import { ref, computed, onMounted } from 'vue';
import { useAuthStore } from '@/stores/auth'; // Adjust the path as needed
import { useI18n } from 'vue-i18n';

const authStore = useAuthStore();
const { locale } = useI18n();
const user = computed(() => authStore.userDetail);

onMounted(async () => {
  await authStore.getUser();
  selectedLanguage.value = user.value?.language || 'EN'; // Default to EN if not set
  selectedTheme.value = user.value?.template || 'default';

});

const newEmail = ref('');
const confirmNewEmail = ref('');
const oldPassword = ref('');
const newPassword = ref('');
const confirmNewPassword = ref('');
const phoneNumber = ref('');

const selectedLanguage = ref('EN');
const selectedTheme = ref('default');
const notificationSettings = ref({
  smsBatteryLow: false,
  smsGeofenceAlert: false,
  emailBatteryLow: false,
  emailGeofenceAlert: false,
});

const emit = defineEmits(['close-modal']);
const closeModal = () => {
  emit('close-modal');
};

const updateEmail = async () => {
  if (newEmail.value !== confirmNewEmail.value) {
    // alert('Emails do not match');
    return;
  }
  try {
    await authStore.updateEmail(newEmail.value);
    // alert('Email updated successfully');
    closeModal();
  } catch (error) {
    //   alert(`Error updating email: ${error.message}`);
  }
};

const updatePassword = async () => {
  if (newPassword.value !== confirmNewPassword.value) {
    // alert('Passwords do not match');
    return;
  }

  try {
    // Call your store's API method to update the password
    await authStore.updatePassword(oldPassword.value, newPassword.value, confirmNewPassword.value);
    // alert('Password updated successfully');
    closeModal(); // Close the modal after successful update
  } catch (error: any) {
    // alert(`Error updating password: ${error.message}`);
  }
};


const updatePhoneNumber = async () => {
  try {
    // Call your store's API method to update the phone number
    await authStore.updatePhoneNumber(phoneNumber.value);
    // alert('Phone number updated successfully');
    await authStore.getUser();
    closeModal(); // Close the modal after successful update
  } catch (error: any) {
    // alert(`Error updating phone number: ${error.message}`);
  }
};


const updateLanguage = async () => {
  try {
    // Update user language with the selected value
    await authStore.updateLanguage(selectedLanguage.value);
    // alert('Language updated successfully');

    // Update the locale for i18n based on the selected language
    locale.value = selectedLanguage.value;
  } catch (error) {
    // alert(`Error updating Language: ${error.message}`);
  }
};
const updateTemplate = async () => {
  try {
    await authStore.updateTemplate(selectedTheme.value);
    await authStore.getUser(); // Refresh user data
  } catch (error) {
    // alert(`Error updating theme: ${error.message}`);
  }
};
const updateSettings = async () => {
  try {
    // Call both updateLanguage and updateTemplate methods
    await updateLanguage();
    await updateTemplate();
    // alert('Settings updated successfully');
  } catch (error) {
    // alert(`Error updating settings: ${error.message}`);
  }
};


const updateNotificationSettings = async () => {
  // Handle the logic for saving the notification settings
  try {
    //await authStore.updateNotificationSettings(notificationSettings.value);
    console.log("hi test")
    // alert('Notification settings updated successfully');
  } catch (error) {
    //  alert(`Error updating notification settings: ${error.message}`);
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
  flex-direction: column;
  /* Column layout for stacking */
  align-items: flex-mid;
  /* Align items at the start */
  justify-content: flex-start;
  /* Avoid unnecessary stretching */
  height: auto;
  /* Allow content to adjust height dynamically */
  min-height: 100vh;
  /* Ensure the settings container takes at least full screen height */
  width: 100%;
  background: #f1e4cc;
}

.settings-content {
  padding: 40px;



  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.);

  /* Add some margin around the content to ensure it doesn't touch the screen edges */
}


.settings-header {
  display: flex;
  justify-content: center;
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
  /* Set to row for wider screens */
  gap: 20px;
  justify-content: flex-start;
  flex-wrap: wrap;
  /* This will wrap the elements if there are too many to fit */
}

.settings-card {
  flex: 1;
  min-width: 250px;

  box-shadow: 0 5px 10px rgba(0, 0, 0, 0.3);
  border-radius: 8px;
  padding: 20px;
  background: #ddd;
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
  box-sizing: border-box;
  /* Ensure padding does not affect the width */
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

.dark-mode .settings-container {
  background: #1e1e1e;
  /* Dark background */
  color: #bbb;
  /* Light text color */
}

.dark-mode .settings-card {
  background: #2e2e2e;
  /* Dark card background */
  color: #bbb;
  box-shadow: 0 0 5px rgba(255, 235, 235, 0.3);

  /* Light text color */
}

.dark-mode .form-input {
  background-color: #333;
  /* Dark background for text fields */
  color: #bbb;
  /* Light grey text color for inputs */
  border: 1px solid #555;
  /* Darker border for inputs */
}

.dark-mode .form-submit-button {
  background-color: #333;
  /* Darker button */
  color: #bbb;
  /* Lighter text */
}


@media (max-width: 768px) {
  .settings-body {
    flex-direction: column;
  }
}
</style>
