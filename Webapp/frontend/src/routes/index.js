import { createWebHistory, createRouter } from 'vue-router';
import { useAuthStore } from '@/stores/auth';


const MapView = () => import('../views/map_view/MapView.vue');
const LoginView = () => import('../views/auth_view/LoginView.vue');
const RegisterView = () => import('../views/auth_view/RegisterView.vue');
const SettingsView = () => import('../views/accountsettings_view/SettingsView.vue');
const TrackerView = () => import('../views/tracker_view/TrackerView.vue'); // Neue Seite importieren
const ForgotPasswordView = () => import('../views/auth_view/ForgotPasswordView.vue');
const RouteView = () => import('../views/route_view/RouteView.vue');
const ContactFormView = () => import('../components/main/ContactForm.vue');
const PrivacyPolicyView = () => import('../views/PrivacyPolicy.vue'); // Neue Seite privacy Policy

// const ForbiddenPage = () => import('../components/ForbiddenPage.vue'); // Falls benötigt

const routes = [
  { path: '/', redirect: { name: 'login' } }, // Default to login if no route is specified
  { path: '/map', component: MapView, name: 'main', meta: { requiresAuth: true } },
  { path: '/route', component: RouteView, name: 'route', meta: { requiresAuth: true } },
  { path: '/account', component: SettingsView, name: 'account', meta: { requiresAuth: true } },
  { path: '/login', component: LoginView, name: 'login', meta: { requiresGuest: true, hideComponent: true } },
  { path: '/register', component: RegisterView, name: 'register', meta: { requiresGuest: true, hideComponent: true } },
  { path: '/reset', component: ForgotPasswordView, name: 'reset', meta: { requiresGuest: true, hideComponent: true } },
  { path: '/trackers', component: TrackerView, name: 'trackers', meta: { requiresAuth: true } },
  { path: '/contact', component: ContactFormView, name: 'contact', meta: { requiresAuth: true } },
  { path: '/privacy', component: PrivacyPolicyView, name: 'privacy' },
];



const router = createRouter({
    history: createWebHistory(),
    routes
});

// Global navigation guard for authentication and guest access
router.beforeResolve(async (to, from, next) => {
    const authStore = useAuthStore();
  
    // Überprüfen, ob die Route Authentifizierung erfordert und ob der Benutzer nicht authentifiziert ist
    if (to.meta.requiresAuth && !authStore.isAuthenticated) {
      // Umleitung zur Login-Seite
      return next({ name: 'login', query: { redirect: to.fullPath } });
    } 
    // Überprüfen, ob die Route nur für Gäste ist und ob der Benutzer bereits authentifiziert ist
    else if (to.meta.requiresGuest && authStore.isAuthenticated) {
      // Umleitung zur Hauptseite
      return next({ name: 'main' });
    } 
    else {
      return next();
    }
}); 
  
export default router;
