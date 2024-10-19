<template>
  <div class="wrapper">
    <div class="container">
      <div class="logo-container">
        <img src="@/assets/logo-transparent.png" alt="Logo" class="logo">
      </div>
      <div class="registration form">
        <header>{{ $t('LOGINVIEW-sign_up') }}</header>
        <form @submit.prevent="register">
          <input type="text" v-model="registerData.email" :placeholder="$t('LOGINVIEW-enter_email')" required>
          <input type="password" v-model="registerData.password" :placeholder="$t('REGISTERVIEW-create_password')"
            required>
          <input type="password" v-model="registerData.password_confirm"
            :placeholder="$t('REGISTERVIEW-password_confirm')" required>
          <input type="submit" class="button" :value="$t('LOGINVIEW-sign_up')">
        </form>
        <div class="signup">
          <span class="signup">{{ $t('REGISTERVIEW-already_account') }}
            <RouterLink to="login">{{ $t('LOGINVIEW-login') }}</RouterLink>
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
const { locale } = useI18n();

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
    alert('Registration successful!'); // Show success message
    const emailContent = getWelcomeEmailContent(registerData.email);

    // Call the backend to send the welcome email
    await sendWelcomeEmail(emailContent);
    router.replace({ name: 'login' });
  } catch (err: any) {
    console.error('Register failed:', err);
    if (err.response) {
      console.error('Server Response:', err.response.data); // Log server error details
    }
    errorMessage.value = err.response?.data?.message || err.message; // Use server error message if available
  }
}

function getWelcomeEmailContent(email: string) {
  // Extract user name from the email (everything before the @)
  const userName = email.split('@')[0];

  if (locale.value === 'DE') {
    // German version
    return {
      to: email,
      subject: 'Willkommen bei BO-Tracker!',
      text: `Hallo ${userName},

Willkommen bei BO-Tracker! Wir freuen uns sehr, dich an Bord zu haben. BO-Tracker hilft dir dabei, deine Geräte einfach und effizient zu überwachen und zu verwalten.

Hier sind einige Dinge, die du direkt tun kannst:
- Entdecke dein Dashboard und sieh dir deine Tracker an.
- Passe deine Einstellungen für eine optimale Nutzung an.
- Kontaktiere unser Support-Team, wenn du Hilfe benötigst.

Wenn du Fragen hast, kannst du gerne auf diese E-Mail antworten. Wir sind immer für dich da.

Viel Spaß mit BO-Tracker! Wir freuen uns darauf, dir die beste Nutzung unserer Plattform zu ermöglichen.

Mit freundlichen Grüßen,
Dein BOTracker-Team`,
    };
  } else {
    // English version (default)
    return {
      to: email,
      subject: 'Welcome to BO-Tracker!',
      text: `Hi ${userName},

Welcome to BO-Tracker! We’re thrilled to have you on board. BO-Tracker is here to help you track and manage your devices with ease and efficiency.

Here are a few things you can do right away:
- Explore your dashboard to see your trackers.
- Customize your settings for an optimal experience.
- Reach out to our support team if you need any help.

If you have any questions, feel free to reply to this email. We’re always here to assist you.

Enjoy using BO-Tracker, and we look forward to helping you make the most of our platform!

Best regards,
The BOTracker Team`,
    };
  }
}


function setLanguage(language: string) {
  console.log('Language switched to:', language);
  locale.value = language; // Update the locale to the selected language
}

async function sendWelcomeEmail(emailContent: { to: string; subject: string; text: string }) {
  try {

    const response = await fetch('http://localhost:3500/api/mail/send', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(emailContent),
    });

    if (!response.ok) {
      throw new Error('Failed to send welcome email');
    }

    console.log('Welcome email sent successfully');
  } catch (error) {
    console.error('Error sending welcome email:', error);
  }
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
  box-shadow: 0 5px 10px rgba(0, 0, 0, 0.9);
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

.language-icon:hover {
  transform: scale(1.2);
}

/* Form styling */
.form header {
  font-size: 1.6rem;
  font-weight: 500;
  text-align: center;
  margin-bottom: 1.5rem;
  color: #333;
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

.form input[type="text"],
.form input[type="password"] {
  height: 60px;
  width: 100%;
  padding: 0 15px;
  font-size: 17px;
  margin-bottom: 1.3rem;
  border: 2px solid #333;
  border-radius: 6px;
  outline: none;
  transition: border 0.3s ease-in-out;
}

.form input[type="text"]:focus,
.form input[type="password"]:focus {
  box-shadow: 0 0 5px rgba(0, 149, 121, 0.5);
  border-color: #006653;
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
