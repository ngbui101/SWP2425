
import { createApp } from 'vue'
import App from './App.vue'
import router from "./routes"
import {createPinia } from 'pinia'
import {authentication} from './plugins/authentication'


const app = createApp(App)
app.use(createPinia())
authentication.install().then(() => {
    app.use(router)
    app.mount('#app')
    console.log('App mounted successfully:', app);
})
