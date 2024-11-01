<template>
  <div :class="['settings-container-wrapper', (user.settings?.template ?? 'default') === 'dark' ? 'dark-mode' : '']"
    @click="resetAllWobbles">
    <div class="settings-container" @click.stop>
      <div class="settings-content">
        <!-- Settings Header -->
        <div class="settings-header">
          <h2 class="settings-title">{{ $t('SETTINGSVIEW-settings') }}</h2>
        </div>
        <div class="settings-body">

          <!-- Email Settings Card -->
          <div class="settings-card" :style="getTransformStyle(card1)" @mousedown="startWobble(card1, $event)"
            @mouseup="stopWobble(card1)" @mousemove="wobbleCard(card1, $event)" @mouseleave="stopWobble(card1)">
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
          <div class="settings-card" :style="getTransformStyle(card2)" @mousedown="startWobble(card2, $event)"
            @mouseup="stopWobble(card2)" @mousemove="wobbleCard(card2, $event)" @mouseleave="stopWobble(card2)">
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
          <div class="settings-card" :style="getTransformStyle(card3)" @mousedown="startWobble(card3, $event)"
            @mouseup="stopWobble(card3)" @mousemove="wobbleCard(card3, $event)" @mouseleave="stopWobble(card3)">
            <h3 class="card-title">{{ $t('SETTINGSVIEW-phone_number_settings') }}</h3>
            <p v-if="user.number">{{ $t('SETTINGSVIEW-current_phone_number') }}: <strong>{{ user.number }}</strong></p>
            <p v-else>{{ $t('SETTINGSVIEW-no_phone_number') }}</p>
            <form @submit.prevent="updatePhoneNumber" class="settings-form">
              <label for="phoneNumber" class="form-label">{{ $t('SETTINGSVIEW-phone_number') }}</label>
              <input v-model="phoneNumber" id="phoneNumber" type="tel" class="form-input"
                :placeholder="$t('SETTINGSVIEW-enter_phone_number')" required>
              <button type="submit" class="form-submit-button">{{ $t('SETTINGSVIEW-save') }}</button>
            </form>
          </div>

          <!-- Language and Theme Settings Card -->
          <div class="settings-card" :style="getTransformStyle(card4)" @mousedown="startWobble(card4, $event)"
            @mouseup="stopWobble(card4)" @mousemove="wobbleCard(card4, $event)" @mouseleave="stopWobble(card4)">
            <h3 class="card-title">{{ $t('SETTINGSVIEW-language_settings') }}</h3>
            <form @submit.prevent="updateLanguageAndTheme" class="settings-form">
              <label for="language" class="form-label">{{ $t('SETTINGSVIEW-select_language') }}</label>
              <select v-model="selectedLanguage" id="language" class="form-input">
                <option value="EN">{{ $t('SETTINGSVIEW-english') }}</option>
                <option value="DE">{{ $t('SETTINGSVIEW-german') }}</option>
              </select>
              <label for="theme" class="form-label">{{ $t('SETTINGSVIEW-select_color_scheme') }}</label>
              <select v-model="selectedTheme" id="theme" class="form-input">
                <option value="default">{{ $t('SETTINGSVIEW-light') }}</option>
                <option value="dark">{{ $t('SETTINGSVIEW-dark') }}</option>
              </select>
              <button type="submit" class="form-submit-button">{{ $t('SETTINGSVIEW-save') }}</button>
            </form>
          </div>

          <!-- Notification Settings Card -->
          <div class="settings-card" :style="getTransformStyle(card5)" @mousedown="startWobble(card5, $event)"
            @mouseup="stopWobble(card5)" @mousemove="wobbleCard(card5, $event)" @mouseleave="stopWobble(card5)">
            <h3 class="card-title">{{ $t('SETTINGSVIEW-notification_settings') }}</h3>
            <form @submit.prevent="updateNotifications" class="settings-form">
              <h4>{{ $t('SETTINGSVIEW-sms_notifications') }}</h4>
              <label class="form-label">
                <input type="checkbox" v-model="notificationSettings.smsBatteryLow">
                {{ $t('SETTINGSVIEW-sms_battery_low') }}
              </label>
              <label class="form-label">
                <input type="checkbox" v-model="notificationSettings.smsGeofenceAlert">
                {{ $t('SETTINGSVIEW-sms_geofence_alert') }}
              </label>
              <label class="form-label">
                <input type="checkbox" v-model="notificationSettings.smsMotionAlert">
                {{ $t('SETTINGSVIEW-sms_motion_alert') }}
              </label>

              <h4>{{ $t('SETTINGSVIEW-email_notifications') }}</h4>
              <label class="form-label">
                <input type="checkbox" v-model="notificationSettings.emailBatteryLow">
                {{ $t('SETTINGSVIEW-email_battery_low') }}
              </label>
              <label class="form-label">
                <input type="checkbox" v-model="notificationSettings.emailGeofenceAlert">
                {{ $t('SETTINGSVIEW-email_geofence_alert') }}
              </label>
              <label class="form-label">
                <input type="checkbox" v-model="notificationSettings.emailMotionAlert">
                {{ $t('SETTINGSVIEW-email_motion_alert') }}
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
import { useAuthStore } from '@/stores/auth';
import { useI18n } from 'vue-i18n';
import './style_settings.css';

const authStore = useAuthStore();
const { locale } = useI18n();
const user = computed(() => authStore.userDetail);

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
  smsMotionAlert: false,
  emailBatteryLow: false,
  emailGeofenceAlert: false,
  emailMotionAlert: false,
});

onMounted(() => {
  if (user.value?.settings) {
    // Set the language and theme
    selectedLanguage.value = user.value.settings.language || 'EN';
    selectedTheme.value = user.value.settings.template || 'default';

    // Set the notification settings based on the current user settings
    notificationSettings.value = {
      smsBatteryLow: user.value.settings.notifications.sms.batteryLow || false,
      smsGeofenceAlert: user.value.settings.notifications.sms.geofence || false,
      smsMotionAlert: user.value.settings.notifications.sms.motionSensor || false,
      emailBatteryLow: user.value.settings.notifications.email.batteryLow || false,
      emailGeofenceAlert: user.value.settings.notifications.email.geofence || false,
      emailMotionAlert: user.value.settings.notifications.email.motionSensor || false,
    };
  }
});



const updateEmail = async () => {
  if (newEmail.value !== confirmNewEmail.value) {
    alert("Email addresses do not match!");
    return;
  }
  try {
    await authStore.updateEmail(newEmail.value);
    alert("Email updated successfully!");
  } catch (error) {
    alert("Failed to update email.");
    console.error("Error updating email:", error);
  }
};


const updatePassword = async () => {
  if (newPassword.value !== confirmNewPassword.value) {
    alert("Passwords do not match!");
    return;
  }
  try {
    await authStore.updatePassword(oldPassword.value, newPassword.value, confirmNewPassword.value);
    alert("Password updated successfully!");
  } catch (error) {
    alert("Failed to update password.");
    console.error("Error updating password:", error);
  }
};

const updatePhoneNumber = async () => {
  try {
    await authStore.updatePhoneNumber(phoneNumber.value);
    await authStore.getUser();
    alert("Phone number updated successfully!");
  } catch (error) {
    alert("Failed to update phone number.");
    console.error("Error updating phone number:", error);
  }
};


const updateLanguageAndTheme = async () => {
  try {
    await authStore.updateSettings({
      language: selectedLanguage.value,
      template: selectedTheme.value,
    });

    // Update frontend language immediately
    locale.value = selectedLanguage.value;
    alert("Language and theme updated successfully");
  } catch (error) {
    alert("Failed to update language and theme", "error");
  }
};

const updateNotifications = async () => {
  try {
    await authStore.updateSettings({
      notifications: {
        sms: {
          batteryLow: notificationSettings.value.smsBatteryLow,
          geofence: notificationSettings.value.smsGeofenceAlert,
          motionSensor: notificationSettings.value.smsMotionAlert,
        },
        email: {
          batteryLow: notificationSettings.value.emailBatteryLow,
          geofence: notificationSettings.value.emailGeofenceAlert,
          motionSensor: notificationSettings.value.emailMotionAlert,
        },
      },
    });
    alert("Notification settings updated successfully!");
  } catch (error) {
    alert("Failed to update notification settings.");
    console.error("Error updating notifications:", error);
  }
};


// Helper to get transform style for wobble effect
const getTransformStyle = (card) => ({
  transform: `translateX(${card.offsetX}px) translateY(${card.offsetY}px) rotate(${card.rotation}deg)`,
});

const emit = defineEmits(['close-modal']);
const closeModal = () => emit('close-modal');

// Wobble logic
const createCardState = () => ({
  wobbling: false,
  rotation: 0,
  offsetX: 0,
  offsetY: 0,
});

const card1 = ref(createCardState());
const card2 = ref(createCardState());
const card3 = ref(createCardState());
const card4 = ref(createCardState());
const card5 = ref(createCardState());

const startX = ref(0);
const startY = ref(0);

const startWobble = (card, event) => {
  card.wobbling = true;
  startX.value = event.clientX;
  startY.value = event.clientY;
  document.body.style.userSelect = 'none';
};

const stopWobble = (card) => {
  card.wobbling = false;
  card.rotation = 0;
  card.offsetX = 0;
  card.offsetY = 0;
};

const wobbleCard = (card, event) => {
  if (card.wobbling) {
    const deltaX = event.clientX - startX.value;
    const deltaY = event.clientY - startY.value;
    card.rotation = deltaX * 0.05;
    card.offsetX = deltaX * 0.1;
    card.offsetY = deltaY * 0.1;
  }
};

const resetAllWobbles = () => {
  [card1, card2, card3, card4, card5].forEach(card => {
    card.value.wobbling = false;
    card.value.rotation = 0;
    card.value.offsetX = 0;
    card.value.offsetY = 0;
  });
};
</script>
<style scoped>
.notification {
  position: fixed;
  top: 10px;
  right: 10px;
  padding: 10px;
  border-radius: 5px;
  color: white;
  font-weight: bold;
}

.notification.success {
  background-color: #28a745;
}

.notification.error {
  background-color: #dc3545;
}
</style>