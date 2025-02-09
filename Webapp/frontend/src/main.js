// src/main.js

import { createApp } from 'vue'
import App from './App.vue'
import router from "./routes"
import { createPinia } from 'pinia'
import { authentication } from './plugins/authentication'
import EN from './locale/en.json'
import DE from './locale/de.json'
import { createI18n } from 'vue-i18n'
import './global.css'
import VueGuidedTour from 'vue-guided-tour'
import 'vue-guided-tour/style.css'
import 'shepherd.js/dist/css/shepherd.css'
import axios from 'axios'
import { useAuthStore } from '@/stores/auth'

const i18n = createI18n({
    legacy: false,
    locale: 'DE',
    messages: {
        EN: EN,
        DE: DE,
    }
})

const app = createApp(App)
const pinia = createPinia()
app.use(pinia)

authentication.install().then(() => {
    app.use(router)
    app.use(i18n)
    app.use(VueGuidedTour)
    app.mount('#app')
    console.log('App mounted successfully:', app);
    
    // Access the authStore with the Pinia instance
    const authStore = useAuthStore()
    
    // Set up Axios response interceptor
    axios.interceptors.response.use(
      response => response, // Pass through successful responses
      async (error) => {
        const originalRequest = error.config

        // Check if the error is due to an expired access token
        if (error.response && error.response.status === 401 && !originalRequest._retry) {
          originalRequest._retry = true // Prevent infinite loops

          try {
            // Attempt to refresh the access token
            const refreshResponse = await axios.post('http://localhost:3500/api/auth/refresh', {}, { withCredentials: true })
            const newAccessToken = refreshResponse.data.access_token

            // Update the access token in the store and localStorage
            authStore.accessToken = newAccessToken
            localStorage.setItem('accessToken', newAccessToken)
            console.log('Access Token refreshed:', newAccessToken)

            // Update the Authorization header and retry the original request
            originalRequest.headers['Authorization'] = `Bearer ${newAccessToken}`
            return axios(originalRequest)
          } catch (refreshError) {
            console.error('Token refresh failed:', refreshError)
            // If refreshing fails, log out the user and redirect to login
            await authStore.logout()
            router.push({ name: 'login' })
            return Promise.reject(refreshError)
          }
        }

        return Promise.reject(error) // Reject other errors
      }
    )
})
