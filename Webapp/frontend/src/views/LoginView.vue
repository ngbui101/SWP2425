<template>
  <div class="wrapper">
    <div class="logo-container">
      <img src="@/assets/logo-transparent.png" alt="Logo" class="logo">
    </div>
    <div class="container">
      <div class="language-switch">
        <span>
          <a href="#" @click="setLanguage('de')">
            <img src="@/assets/icons8-deutschland-emoji-48.png" alt="DE" class="language-icon">
          </a>
          <a href="#" @click="setLanguage('en')">
            <img src="@/assets/icons8-großbritannien-emoji-48.png" alt="EN" class="language-icon">
          </a>
        </span>
      </div>
      <div class="login form">
        <header>Login to continue</header>
        <form @submit.prevent="submit">
          <input type="text" v-model="loginData.email" placeholder="Enter your email" required>
          <input type="password" v-model="loginData.password" placeholder="Enter your password" required>
          <a href="#">Forgot password?</a>
          <input type="submit" class="button" value="Login">
        </form>
        <div class="signup">
          <span class="signup">Don't have an account?
            <RouterLink to="register">Sign Up</RouterLink>
          </span>
        </div>
      </div>
    </div>
  </div>
</template>



<script setup lang="ts">
import { reactive, ref } from 'vue';
import { useRouter } from 'vue-router';
import { useAuthStore } from '@/stores/auth';

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

function setLanguage(language: string) {
  console.log('Language switched to:', language);
  // Handle the language switch logic here
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

body {
  min-height: 100vh;
  width: 100%;
  background: #009579;
}

.wrapper {
  position: absolute;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  max-width: 430px;
  width: 100%;
  text-align: center;
}

.logo-container {
  margin-bottom: 1rem;
}

.logo {
  width: 100%;
  max-width: 430px; /* Matches the max-width of the container */
  height: auto;
}

.container {
  position: relative; /* Make the container a relative positioned element */
  background: #fff;
  border-radius: 7px;
  box-shadow: 0 5px 10px rgba(0, 0, 0, 0.3);
  padding: 2rem;
}

.language-switch {
  position: absolute;
  top: 10px;
  right: 10px;
  font-size: 16px;
  color: black;
}

.language-switch a {
  margin: 0 5px;
  text-decoration: none;
}

.language-icon {
  width: 24px; /* Adjust the size as needed */
  height: 24px;
  vertical-align: middle; /* Align icons vertically with the text */
}

.form header {
  font-size: 2rem;
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
  background: #009579;
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
