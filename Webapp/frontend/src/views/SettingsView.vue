<template>
  <div :class="['settings-container-wrapper', (user.template ?? 'default') === 'dark' ? 'dark-mode' : '']"
    @click="resetAllWobbles">
    <div class="settings-container" @click.stop>
      <div class="settings-content">
        <!-- Settings Header -->
        <div class="settings-header">
          <h2 class="settings-title">{{ $t('SETTINGSVIEW-settings') }}</h2>
        </div>
        <div class="settings-body">
          <!-- Email Settings Card -->
          <div class="settings-card" :style="{
            transform: `translateX(${card1.offsetX}px) translateY(${card1.offsetY}px) rotate(${card1.rotation}deg)`
          }" @mousedown="startWobble(card1, $event)" @mouseup="stopWobble(card1)"
            @mousemove="wobbleCard(card1, $event)" @mouseleave="stopWobble(card1)">
            <h3 class="card-title">{{ $t('SETTINGSVIEW-email_settings') }}</h3>
            <p>{{ $t('SETTINGSVIEW-current_email') }}: {{ user.email }}</p>
            <form @submit.prevent="updateEmail" class="settings-form">
              <label for="newEmail" class="form-label">{{ $t('SETTINGSVIEW-new_email') }}</label>
              <input v-model="newEmail" id="newEmail" type="email" class="form-input"
                :placeholder="$t('SETTINGSVIEW-new_email')" required>
              <label for="confirmNewEmail" class="form-label">{{ $t('SETTINGSVIEW-confirm_email') }}</label>
              <input v-model="confirmNewEmail" id="confirmNewEmail" type="email" class="form-input"
                :placeholder="$t('SETTINGSVIEW-confirm_email')" required>
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

          <!-- Password Settings Card -->
          <div class="settings-card" :style="{
            transform: `translateX(${card2.offsetX}px) translateY(${card2.offsetY}px) rotate(${card2.rotation}deg)`
          }" @mousedown="startWobble(card2, $event)" @mouseup="stopWobble(card2)"
            @mousemove="wobbleCard(card2, $event)" @mouseleave="stopWobble(card2)">
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
          <div class="settings-card" :style="{
            transform: `translateX(${card3.offsetX}px) translateY(${card3.offsetY}px) rotate(${card3.rotation}deg)`
          }" @mousedown="startWobble(card3, $event)" @mouseup="stopWobble(card3)"
            @mousemove="wobbleCard(card3, $event)" @mouseleave="stopWobble(card3)">
            <h3 class="card-title">{{ $t('SETTINGSVIEW-phone_number_settings') }}</h3>
            <p v-if="user.number">{{ $t('SETTINGSVIEW-current_phone_number') }}: <strong>{{ user.number }}</strong></p>
            <p v-else>{{ $t('SETTINGSVIEW-no_phone_number') }}</p>
            <form @submit.prevent="updatePhoneNumber" class="settings-form">
              <label for="phoneNumber" class="form-label">{{ $t('SETTINGSVIEW-phone_number') }}</label>
              <input v-model="phoneNumber" id="phoneNumber" type="tel" class="form-input"
                :placeholder="$t('SETTINGSVIEW-enter_phone_number')" required>
              <label class="form-label">
                <input type="checkbox" v-model="notificationSettings.smsBatteryLow">
                {{ $t('SETTINGSVIEW-sms_battery_low') }}
              </label>
              <label class="form-label">
                <input type="checkbox" v-model="notificationSettings.smsGeofenceAlert">
                {{ $t('SETTINGSVIEW-sms_geofence_alert') }}
              </label>
              <button type="submit" class="form-submit-button">{{ $t('SETTINGSVIEW-save') }}</button>
            </form>
          </div>

          <!-- Language Settings Card -->
          <div class="settings-card" :style="{
            transform: `translateX(${card4.offsetX}px) translateY(${card4.offsetY}px) rotate(${card4.rotation}deg)`
          }" @mousedown="startWobble(card4, $event)" @mouseup="stopWobble(card4)"
            @mousemove="wobbleCard(card4, $event)" @mouseleave="stopWobble(card4)">
            <h3 class="card-title">{{ $t('SETTINGSVIEW-language_settings') }}</h3>
            <form @submit.prevent="updateSettings" class="settings-form">
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
        </div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed, onMounted } from 'vue';
import { useAuthStore } from '@/stores/auth';
import { useI18n } from 'vue-i18n';

const authStore = useAuthStore();
const { locale } = useI18n();
const user = computed(() => authStore.userDetail);

onMounted(async () => {
  await authStore.getUser();
  selectedLanguage.value = user.value?.language || 'EN';
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

// Wobble card logic
const createCardState = () => ({
  wobbling: false,
  rotation: 0,
  offsetX: 0,
  offsetY: 0
});
const card1 = ref(createCardState());
const card2 = ref(createCardState());
const card3 = ref(createCardState());
const card4 = ref(createCardState());

const startX = ref(0); // Track starting X position
const startY = ref(0); // Track starting Y position

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
    const rotation = deltaX * 0.05;
    card.rotation = rotation;
    card.offsetX = deltaX * 0.1;
    card.offsetY = deltaY * 0.1;
  }
};

// Reset all cards to their default position when clicking outside
const resetAllWobbles = () => {
  [card1, card2, card3, card4].forEach(card => {
    card.value.wobbling = false;
    card.value.rotation = 0;
    card.value.offsetX = 0;
    card.value.offsetY = 0;
  });
};

// Form actions for email, password, etc.
const updateEmail = async () => {
  if (newEmail.value !== confirmNewEmail.value) return;
  try {
    await authStore.updateEmail(newEmail.value);
    closeModal();
  } catch (error) { }
};

const updatePassword = async () => {
  if (newPassword.value !== confirmNewPassword.value) return;
  try {
    await authStore.updatePassword(oldPassword.value, newPassword.value, confirmNewPassword.value);
    closeModal();
  } catch (error) { }
};

const updatePhoneNumber = async () => {
  try {
    await authStore.updatePhoneNumber(phoneNumber.value);
    await authStore.getUser();
    closeModal();
  } catch (error) { }
};

const updateLanguage = async () => {
  try {
    await authStore.updateLanguage(selectedLanguage.value);
    locale.value = selectedLanguage.value;
  } catch (error) { }
};

const updateTemplate = async () => {
  try {
    await authStore.updateTemplate(selectedTheme.value);
    await authStore.getUser();
  } catch (error) { }
};

const updateSettings = async () => {
  try {
    await updateLanguage();
    await updateTemplate();
  } catch (error) { }
};
</script>

<style scoped>
body {
  font-family: 'Poppins', sans-serif;
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

.settings-container-wrapper {
  display: flex;
  flex-direction: column;
  min-height: 100vh;
  width: 100%;

}

.settings-container {
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: flex-start;
  width: 100%;
  background: linear-gradient(135deg, #f1e4cc 0%, #e6cc99 50%, #f1e4cc 100%);
}

.settings-content {
  padding: 40px;
}

.settings-header {
  display: flex;
  justify-content: center;
  align-items: center;
  margin-bottom: 20px;
  color: #333;
}

.settings-title {
  font-size: 24px;
  font-weight: 600;
  color: #1F1F1F;
}

.settings-body {
  display: flex;
  flex-direction: row;
  gap: 20px;
  justify-content: flex-start;
  flex-wrap: wrap;
}

.settings-card {
  flex: 1;
  min-width: 250px;
  box-shadow: 0 5px 10px rgba(0, 0, 0, 1);
  border-radius: 8px;
  padding: 20px;
  background: rgba(224, 224, 224, 0.9);
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  transition: transform 0.2s ease-out;
  cursor: grab;
  position: relative;
  user-select: none;
}

.settings-card:hover {
  transform: scale(1.05);
  transition: transform 0.3s ease;
}



input[type="checkbox"] {
  accent-color: #00543D;
}

.dark-mode input[type="checkbox"] {
  accent-color: #E69543;
}

.settings-card:active {
  cursor: grabbing;
}

.card-title {
  font-size: 18px;
  font-weight: 600;
  margin-bottom: 10px;
  color: #333;
}

.dark-mode .card-title {
  color: #ddd;
}



.form-label {
  font-size: 14px;
  font-weight: 500;
  display: block;
}

.form-input {
  border: 1px solid #d0d0d0;
  border-radius: 4px;
  padding: 10px;
  font-size: 14px;
  width: 100%;
  box-sizing: border-box;
}

.settings-form {
  display: flex;
  flex-direction: column;
  gap: 10px;
  align-items: flex-start;
  /* Center all items including the button */
}

.form-submit-button {
  background-color: #00543D;
  color: white;
  border: none;
  padding: 12px 20px;
  border-radius: 4px;
  cursor: pointer;
  font-size: 14px;
  margin-top: 15px;
  font-weight: 500;
  width: 100%;

}

.form-submit-button:hover {
  background-color: #00412F;

}

.dark-mode .form-submit-button:hover {
  background-color: #292929;
}



/* Dark mode styles */
.dark-mode .settings-title {
  color: #E69543;
  ;
}

.dark-mode .settings-container {
  background:
    linear-gradient(135deg, #1e1e1e 0%, #1a1a1a 50%, #1e1e1e 100%);
  color: #bbb;
}

.dark-mode .settings-card {
  background: #2e2e2e;
  color: #bbb;
  box-shadow: 0 0 5px rgba(255, 235, 235, 0.4);
}

.dark-mode .form-input {
  background-color: #333;
  color: #bbb;
  border: 1px solid #555;
}

.dark-mode .form-submit-button {
  background-color: #333;
  color: #bbb;
  box-shadow: 0 0 5px rgba(255, 235, 235, 0.4);
}

@media (max-width: 768px) {
  .settings-body {
    flex-direction: column;
  }
}
</style>
