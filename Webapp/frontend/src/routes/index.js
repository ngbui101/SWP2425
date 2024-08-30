import { createWebHistory, createRouter } from 'vue-router';
import { useAuthStore } from '@/stores/auth';

const MainView = () => import('../views/MainView.vue');
const LoginView = () => import('../views/LoginView.vue');
const RegisterView = () => import('../views/RegisterView.vue');
// const ForbiddenPage = () => import('../components/ForbiddenPage.vue'); // Falls benötigt

const routes = [
    { path: '/', component: MainView, name: 'main', meta: { requiresAuth: true } }, // Geschützte Route
    { path: '/login', component: LoginView, name: 'login', meta: { requiresGuest: true } },
    { path: '/register', component: RegisterView, name: 'register', meta: { requiresGuest: true } },
    // { path: '/unauthorized', name: 'unauthorized', component: ForbiddenPage, meta: { hideComponent:true}} // Falls benötigt
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
