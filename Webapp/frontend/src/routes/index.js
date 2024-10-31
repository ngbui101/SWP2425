import { createWebHistory, createRouter } from 'vue-router';
import { useAuthStore } from '@/stores/auth';


const MapView = () => import('../views/map_view/MapView.vue');
const LoginView = () => import('../views/LoginView.vue');
const RegisterView = () => import('../views/RegisterView.vue');
const SettingsView = () => import('../views/SettingsView.vue');
const TrackerView = () => import('../views/tracker_view/TrackerView.vue'); // Neue Seite importieren
const ForgotPasswordView = () => import('../views/ForgotPasswordView.vue');
const RouteView = () => import('../views/route_view/RouteView.vue');
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
