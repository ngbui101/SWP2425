<template>
  <div class="wrapper">
    <div class="container">
      <div class="logo-container">
        <img src="@/assets/logo-transparent.png" alt="Logo" class="logo">
      </div>
      <div class="login form">
        <header>{{ $t('LOGINVIEW-login_to_continue') }}</header>
        <form @submit.prevent="submit">
          <input type="text" v-model="loginData.email" :placeholder="$t('LOGINVIEW-enter_email')" required>
          <input type="password" v-model="loginData.password" :placeholder=" $t('LOGINVIEW-enter_password')" required>
          <a href="#">{{ $t('LOGINVIEW-forgot_password') }}</a>
          <input type="submit" class="button" :value="$t('LOGINVIEW-login')">
        </form>
        <div class="signup">
          <span class="signup">{{ $t('LOGINVIEW-dont_have_account') }}
            <RouterLink to="register">{{ $t('LOGINVIEW-sign_up') }}</RouterLink>
          </span>
        </div>
      </div>
      <div class="language-switch">
        <span>
          <a href="#" @click.prevent="setLanguage('DE')">
            <img src="@/assets/icons8-deutschland-emoji-48.png" alt="DE" class="language-icon">
          </a>
          <a href="#" @click.prevent="setLanguage('EN')">
            <img src="@/assets/icons8-großbritannien-emoji-48.png" alt="EN" class="language-icon">
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

const loginData = reactive({
  email: '',
  password: ''
});

const errorMessage = ref<string>('');

async function submit() {
  console.log('Submit function triggered'); // Debug log
  try {
    console.log('Attempting to log in with:', loginData);
    const response = await authStore.login(loginData);
    console.log('Login successful, response:', response);
    router.replace({ name: 'main' }); // Always redirect to 'main' route after login
  } catch (err: any) {
    console.error('Login failed:', err);
    errorMessage.value = err.message;
  }
}

const { locale } = useI18n();

function setLanguage(language: string) {
  console.log('Language switched to:', language);
  locale.value = language; // Update the locale to the selected language
}

function goToSignup() {
  router.push({ name: 'register' }); // Assuming you have a signup route
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
  height: 100%; /* Ensure it occupies the full height */
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
