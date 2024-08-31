
import { createApp } from 'vue'
import App from './App.vue'
import router from "./routes"
import {createPinia } from 'pinia'
import {authentication} from './plugins/authentication'
import EN from './locale/en.json'
import DE from './locale/de.json'
import { createI18n } from 'vue-i18n'
import './global.css'
import 'bootstrap/dist/css/bootstrap.min.css';
import 'ionicons/dist/css/ionicons.min.css';

const i18n = createI18n({
    legacy: false,
    locale: 'DE',
    messages: {
        EN: EN,
        DE: DE,
    }
})


const app = createApp(App)
app.use(createPinia())
authentication.install().then(() => {
    app.use(router)
    app.use(i18n)
    app.mount('#app')
    console.log('App mounted successfully:', app);
})
