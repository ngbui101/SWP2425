<template>
    <div class="wrapper">
        <div class="forgot-password-container">
            <div class="logo-container">
                <img src="@/assets/logo-transparent.png" alt="Logo" class="logo" />
            </div>
            <div class="forgot-password form">
                <header>{{ $t('RESETVIEW-passwort_zuruecksetzen') }}</header>
                <form @submit.prevent="submitForgotPassword">
                    <input type="text" v-model="forgotPasswordEmail"
                        :placeholder="$t('RESETVIEW-email_adresse_eingeben')" required />
                    <input type="submit" class="button" :value="$t('RESETVIEW-zuruecksetzen')" />
                </form>

                <!-- Display success or error message -->
                <div v-if="forgotPasswordMessage" class="forgot-password-message">{{ forgotPasswordMessage }}</div>

                <div class="help">
                    <span class="help">{{ $t('RESETVIEW-benötigen_sie_hilfe') }}
                        <RouterLink to="support" class="link">{{ $t('RESETVIEW-support') }}</RouterLink>
                    </span>
                    <br />
                    <span class="help">
                        {{ $t('RESETVIEW-back_to') }} <RouterLink to="login" class="link">Login</RouterLink>
                    </span>
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
    </div>
</template>

<script setup lang="ts">
import { ref } from 'vue';
import { useRouter } from 'vue-router';
import { useI18n } from 'vue-i18n';

const router = useRouter();
const forgotPasswordEmail = ref('');
const forgotPasswordMessage = ref<string | null>(null);

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
            forgotPasswordMessage.value = 'Check your email for the new password!';
        } else {
            forgotPasswordMessage.value = data.message;
        }
    } catch (error) {
        forgotPasswordMessage.value = 'An error occurred. Please try again.';
    }
}

const { locale } = useI18n();

function setLanguage(language: string) {
    locale.value = language;
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

.forgot-password-container {
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
}

.logo-container {
    margin-bottom: 1rem;
}

.logo {
    width: 100%;
    max-width: 200px;
    height: auto;
    margin: 0 auto;
}

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

.form input:focus {
    box-shadow: 0 1px 0 rgba(0, 0, 0, 0.2);
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

.form input[type="text"],
.form input[type="password"] {
    height: 60px;
    width: 100%;
    padding: 0 15px;
    font-size: 17px;
    margin-bottom: 1.3rem;
    border: 2px solid #1f1f1f;
    /* Updated the outline color */
    border-radius: 6px;
    outline: none;
    transition: border 0.3s ease-in-out;
}

.form input[type="text"]:focus,
.form input[type="password"]:focus {
    box-shadow: 0 0 5px rgba(0, 149, 121, 0.5);
    /* Add shadow for better focus */
    border-color: #006653;
    /* Darker green on focus */
}

.help {
    font-size: 17px;
    text-align: center;
    color: #1f1f1f;
}

.link {
    color: #009579;
    cursor: pointer;
}

.link:hover {
    text-decoration: underline;
}

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
</style>
