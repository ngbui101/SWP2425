<template>
    <div :class="['contact-page', (user.settings?.template ?? 'default') === 'dark' ? 'dark-mode' : '']">
        <div class="contact-form-container">
            <h2 class="contact-form-title">{{ $t('ContactForm-title') }}</h2>
            <p class="contact-form-description">
                {{ $t('ContactForm-description') }}
            </p>
            <form class="contact-form" @submit.prevent="submitContactForm">
                <div class="form-group">
                    <label for="email" class="form-label">{{ $t('ContactForm-emailLabel') }}</label>
                    <input 
                        type="email" 
                        id="email" 
                        class="form-input" 
                        v-model="email" 
                        :placeholder="$t('ContactForm-emailPlaceholder')" 
                        required 
                    />
                </div>
                <div class="form-group">
                    <label for="message" class="form-label">{{ $t('ContactForm-messageLabel') }}</label>
                    <textarea 
                        id="message" 
                        class="form-textarea" 
                        v-model="message" 
                        :placeholder="$t('ContactForm-messagePlaceholder')" 
                        required
                    ></textarea>
                </div>
                <button type="submit" class="form-submit-button">{{ $t('ContactForm-submitButton') }}</button>
            </form>

            <!-- Display success or error message -->
            <div v-if="formMessage" class="form-message">{{ formMessage }}</div>
        </div>
    </div>
</template>

<script setup>
import { ref, computed } from 'vue';
import { useAuthStore } from '@/stores/auth';

const authStore = useAuthStore();
const user = computed(() => authStore.userDetail);
const email = ref(user.value?.email || '');
const message = ref('');
const formMessage = ref('');

async function submitContactForm() {
    try {
        const emailContent = {
            to: 'hsbotracker@gmail.com',
            subject: 'Support Ticket',
            text: `From: ${email.value}\n\nMessage:\n${message.value}`
        };

        const response = await fetch('http://localhost:3500/api/mail/send', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(emailContent),
        });

        if (!response.ok) {
            throw new Error('Failed to send the email. Please try again later.');
        }

        formMessage.value = 'Your message has been sent successfully!';
        email.value = '';
        message.value = '';
    } catch (error) {
        formMessage.value = 'An error occurred while sending your message. Please try again.';
        console.error(error);
    }
}
</script>

<style scoped>
.contact-page {
    min-height: 100vh;
    display: flex;
    justify-content: center;
    align-items: center;
    background: linear-gradient(135deg, #f1e4cc 0%, #e6cc99 50%, #f1e4cc 100%);
}

/* Dark mode background */
.dark-mode.contact-page {
    background: linear-gradient(135deg, #1e1e1e 0%, #141414 50%, #1e1e1e 100%);
}

.contact-form-container {
    width: 100%;
    max-width: 700px;
    padding: 30px;
    background: rgba(255, 255, 255, 0.9);
    border-radius: 15px;
    box-shadow: 0 8px 15px rgba(0, 0, 0, 0.3);
    backdrop-filter: blur(10px);
    transition: transform 0.3s ease;
}

.contact-form-title {
    font-size: 32px;
    font-weight: 700;
    text-align: center;
    margin-bottom: 15px;
    color: #1f1f1f;
}

.contact-form-description {
    font-size: 18px;
    text-align: center;
    margin-bottom: 25px;
    color: #555;
}

.contact-form {
    display: flex;
    flex-direction: column;
    gap: 20px;
}

.form-group {
    width: 100%;
}

.form-label {
    font-size: 16px;
    font-weight: 500;
    margin-bottom: 8px;
    color: #333;
}

.dark-mode .form-label {
    color: #ddd;
}

.form-input,
.form-textarea {
    width: 100%;
    border: 1px solid #d0d0d0;
    border-radius: 8px;
    padding: 12px;
    font-size: 16px;
    box-sizing: border-box;
    background-color: #fff;
}

.form-input:focus,
.form-textarea:focus {
    border-color: #009579;
    box-shadow: 0 0 5px rgba(0, 149, 121, 0.5);
    outline: none;
}

.form-textarea {
    height: 180px;
    resize: vertical;
}

.form-submit-button {
    background-color: #00543d;
    color: #fff;
    border: none;
    padding: 14px 25px;
    border-radius: 8px;
    cursor: pointer;
    font-size: 18px;
    font-weight: 600;
    transition: background-color 0.3s, transform 0.2s;
}

.form-submit-button:hover {
    background-color: #00412f;
    transform: scale(1.02);
}

.form-message {
    margin-top: 20px;
    font-size: 16px;
    color: #009579;
    text-align: center;
}

/* Dark mode styles */
.dark-mode .contact-form-container {
    background: #2e2e2e;
    color: #bbb;
    box-shadow: 0 0 5px rgba(255, 235, 235, 0.4);
}

.dark-mode .contact-form-title {
    color: #e69543;
}

.dark-mode .contact-form-description {
    color: #bbb;
}

.dark-mode .form-input,
.dark-mode .form-textarea {
    background-color: #333;
    color: #bbb;
    border: 1px solid #555;
}

.dark-mode .form-submit-button {
    background-color: #333;
    color: #bbb;
    box-shadow: 0 0 5px rgba(255, 235, 235, 0.4);
}

.dark-mode .form-submit-button:hover {
    background-color: #292929;
}

.dark-mode .form-message {
    color: #e69543;
}
</style>
