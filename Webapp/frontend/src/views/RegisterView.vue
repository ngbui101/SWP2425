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
      <div class="registration form">
        <header>Sign Up</header>
        <form @submit.prevent="register">
          <input type="text" v-model="registerData.email" placeholder="Enter your email">
          <input type="password" v-model="registerData.password" placeholder="Create a password">
          <input type="password" v-model="registerData.password_confirm" placeholder="Confirm your password">
          <input type="submit" class="button" value="Signup">
        </form>
        <div class="signup">
          <span class="signup">Already have an account?
            <RouterLink to="login">Login</RouterLink>
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

const registerData = reactive({
  email: '',
  password: '',
  password_confirm: ''
});

const errorMessage = ref<string>('');

async function register() {
  console.log('Register function triggered'); // Debug log
  try {
  if (registerData.password !== registerData.password_confirm) {
    throw new Error('Passwords do not match!');
  }
  console.log('Payload:', registerData);
  const response = await authStore.register(registerData);
  router.replace({ name: 'login' });
} catch (err: any) {
  console.error('Register failed:', err);
  if (err.response) {
    console.error('Server Response:', err.response.data); // Log server error details
  }
  errorMessage.value = err.response?.data?.message || err.message; // Use server error message if available
}
}


function setLanguage(language) {
      console.log('Language switched to:', language);
      // Handle the language switch logic here
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
  