import { createWebHistory, createRouter } from 'vue-router';
import { useAuthStore } from '@/stores/auth';

const MainPage = () => import('../components/Main.vue');
const LoginPage = () => import('../components/LoginComponent.vue');
const RegisterPage = () => import('../components/RegisterComponent.vue');

const routes = [
    { path: '/', component: MainPage, name: 'main' },
    { path: '/login', component: LoginPage, name: 'login', meta: { requiresGuest: true } },
    { path: '/register', component: RegisterPage, name: 'register', meta: { requiresGuest: true } },
  //  { path: '/unauthorized', name: 'unauthorized', component: ForbiddenPage, meta: { hideComponent:true}}
  ];

  const router = createRouter({
    history: createWebHistory(),
    routes
  });

  // Global navigation guard for authentication and guest access
router.beforeResolve(async (to, from, next) => {
    const authStore = useAuthStore();
  
    if (to.meta.requiresAuth && !authStore.isAuthenticated) {
      return next({ name: 'login', query: { redirect: to.fullPath } });
    } else if (to.meta.requiresGuest && authStore.isAuthenticated) {
      return next({ name: 'main' });
    } else {
      return next();
    }
  });
  
  export default router;
  