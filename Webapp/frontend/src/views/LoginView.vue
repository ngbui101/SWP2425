<template>
  <div class="wrapper">
    <div class="container">
      <div class="logo-container">
        <img src="@/assets/logo-transparent.png" alt="Logo" class="logo" />
      </div>
      <div class="login form">
        <header>{{ $t('LOGINVIEW-login_to_continue') }}</header>
        <form @submit.prevent="submit">
          <input type="text" v-model="loginData.email" :placeholder="$t('LOGINVIEW-enter_email')" required />
          <input type="password" v-model="loginData.password" :placeholder="$t('LOGINVIEW-enter_password')" required />
          <a href="#" @click="openForgotPasswordPopup">{{ $t('LOGINVIEW-forgot_password') }}</a>
          <input type="submit" class="button" :value="$t('LOGINVIEW-login')" />
        </form>
        <div class="signup">
          <span class="signup">{{ $t('LOGINVIEW-dont_have_account') }}
            <RouterLink to="register">{{ $t('LOGINVIEW-sign_up') }}</RouterLink>
          </span>
        </div>
      </div>

      <!-- Forgot Password Popup -->
      <div v-if="showForgotPasswordPopup" class="forgot-password-popup">
        <div class="popup-content">
          <h2>{{ $t('LOGINVIEW-forgot_password') }}</h2>
          <form @submit.prevent="submitForgotPassword">
            <input type="text" v-model="forgotPasswordEmail" :placeholder="$t('LOGINVIEW-enter_email')" required />
            <input type="submit" class="button" :value="$t('LOGINVIEW-submit')" />
          </form>

          <!-- Display success or error message -->
          <div v-if="forgotPasswordMessage" class="forgot-password-message">{{ forgotPasswordMessage }}</div>

          <button class="close-button" @click="closeForgotPasswordPopup">{{ $t('LOGINVIEW-close') }}</button>
        </div>
      </div>

      <div class="language-switch">
        <span>
          <a href="#" @click.prevent="setLanguage('DE')">
            <img src="@/assets/icons8-deutschland-emoji-48.png" alt="DE" class="language-icon" />
          </a>
          <a href="#" @click.prevent="setLanguage('EN')">
            <img src="@/assets/icons8-großbritannien-emoji-48.png" alt="EN" class="language-icon" />
          </a>
        </span>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { reactive, ref } from 'vue';
import { useRouter } from 'vue-router';
import { useAuthStore } from '@/stores/auth';
import { useI18n } from 'vue-i18n';

const router = useRouter();
const authStore = useAuthStore();
const showForgotPasswordPopup = ref(false);
const forgotPasswordEmail = ref('');
const forgotPasswordMessage = ref<string | null>(null); // Message to display to the user

function openForgotPasswordPopup() {
  showForgotPasswordPopup.value = true;
}

function closeForgotPasswordPopup() {
  showForgotPasswordPopup.value = false;
  forgotPasswordMessage.value = null; // Reset message when closing the popup
}

async function submitForgotPassword() {
  try {
    const response = await fetch('http://localhost:3500/api/auth/forgot-password', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({ email: forgotPasswordEmail.value }),
    });

    const data = await response.json();
    if (data.success) {
      forgotPasswordMessage.value = 'Check your email for the new password!'; // Display success message
    } else {
      forgotPasswordMessage.value = data.message; // Display error message
    }
  } catch (error) {
    forgotPasswordMessage.value = 'An error occurred. Please try again.'; // Display generic error message
  }
}

const loginData = reactive({
  email: '',
  password: ''
});

const errorMessage = ref<string>('');

async function submit() {
  try {
    const response = await authStore.login(loginData);
    router.replace({ name: 'main' });
  } catch (err: any) {
    errorMessage.value = err.message;
  }
}

const { locale } = useI18n();

function setLanguage(language: string) {
  locale.value = language;
}

function goToSignup() {
  router.push({ name: 'register' });
}
</script>


<style scoped>
@import url('https://fonts.googleapis.com/css2?family=Poppins:wght@200;300;400;500;600;700&display=swap');

* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
  font-family: 'Poppins', sans-serif;
}

/* Background for everything except the logo and container */
.wrapper {
  display: flex;
  justify-content: center;
  align-items: center;
  min-height: 100vh;
  background-image: url('@/assets/bg-image.png');
  background-size: cover;
  background-position: center;
  background-repeat: no-repeat;
}

.forgot-password-popup {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background-color: rgba(0, 0, 0, 0.5);
  display: flex;
  justify-content: center;
  align-items: center;
  z-index: 1000;
}

.popup-content {
  background-color: white;
  padding: 2rem;
  border-radius: 8px;
  text-align: center;
  box-shadow: 0px 8px 16px rgba(0, 0, 0, 0.2);
  width: 100%;
  max-width: 400px;
}

.popup-content h2 {
  font-size: 1.5rem;
  margin-bottom: 1rem;
}

.popup-content input[type='text'],
.popup-content input[type='submit'] {
  width: 100%;
  padding: 12px;
  margin-bottom: 1rem;
  border: 1px solid #ddd;
  border-radius: 6px;
  font-size: 16px;
}

.popup-content input.button {
  color: #fff;
  background-color: #006653;
  cursor: pointer;
  transition: 0.3s;
}

.popup-content input.button:hover {
  background-color: #004d40;
}

.close-button {
  background-color: #ff6b6b;
  color: white;
  padding: 8px 12px;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  transition: 0.3s;
}

.close-button:hover {
  background-color: #ff4d4d;
}

.forgot-password-message {
  margin-top: 1rem;
  font-size: 0.9rem;
  color: #333;
}

.container {
  background-color: #fff;
  border-radius: 7px;
  box-shadow: 0 5px 10px rgba(0, 0, 0, 0.3);
  padding: 2rem;
  max-width: 430px;
  width: 100%;
  text-align: center;
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  height: 100%;
  /* Ensure it occupies the full height */
}

/* Ensure the logo is centered at the top */
.logo-container {
  margin-bottom: 1rem;
}

.logo {
  width: 100%;
  max-width: 200px;
  height: auto;
  margin: 0 auto;
}

/* Language switch at the bottom and centered */
.language-switch {
  display: flex;
  justify-content: center;
  margin-top: 2rem;
}

.language-switch a {
  margin: 0 10px;
}

.language-icon {
  width: 24px;
  height: 24px;
  vertical-align: middle;
}

/* Form styling */
.form header {
  font-size: 1.6rem;
  font-weight: 500;
  text-align: center;
  margin-bottom: 1.5rem;
}

.form input {
  height: 60px;
  width: 100%;
  padding: 0 15px;
  font-size: 17px;
  margin-bottom: 1.3rem;
  border: 1px solid #ddd;
  border-radius: 6px;
  outline: none;
}

.form input:focus {
  box-shadow: 0 1px 0 rgba(0, 0, 0, 0.2);
}

.form a {
  font-size: 16px;
  color: #009579;
  text-decoration: none;
}

.form a:hover {
  text-decoration: underline;
}

.form input.button {
  color: #fff;
  background: rgb(23, 102, 81);
  font-size: 1.2rem;
  font-weight: 500;
  letter-spacing: 1px;
  margin-top: 1.7rem;
  cursor: pointer;
  transition: 0.4s;
}

.form input.button:hover {
  background: #006653;
}

/* Signup section */
.signup {
  font-size: 17px;
  text-align: center;
}

.signup label {
  color: #009579;
  cursor: pointer;
}

.signup label:hover {
  text-decoration: underline;
}
</style>
