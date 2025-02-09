// src/routes/index.js

import { createWebHistory, createRouter } from "vue-router";
import { useAuthStore } from "@/stores/auth";

const MapView = () => import("../views/map_view/MapView.vue");
const LoginView = () => import("../views/auth_view/LoginView.vue");
const RegisterView = () => import("../views/auth_view/RegisterView.vue");
const SettingsView = () => import("../views/accountsettings_view/SettingsView.vue");
const TrackerView = () => import("../views/tracker_view/TrackerView.vue");
const ForgotPasswordView = () => import("../views/auth_view/ForgotPasswordView.vue");
const RouteView = () => import("../views/route_view/RouteView.vue");
const ContactFormView = () => import("../components/main/ContactForm.vue");
const PrivacyPolicyView = () => import("../views/PrivacyPolicy.vue");
const Nutzungbedingungen = () => import("../views/Nutzungbedingungen.vue");
const Uber = () => import("../views/UberUns.vue");
const HomeView = () => import("../views/HomeView.vue"); // **ADDED**

const routes = [
  { path: "/", name: "home", component: HomeView }, // **UPDATED**
  { path: "/map", component: MapView, name: "main", meta: { requiresAuth: true } },
  { path: "/route", component: RouteView, name: "route", meta: { requiresAuth: true } },
  { path: "/account", component: SettingsView, name: "account", meta: { requiresAuth: true } },
  { path: "/login", component: LoginView, name: "login", meta: { requiresGuest: true, hideComponent: true } },
  { path: "/register", component: RegisterView, name: "register", meta: { requiresGuest: true, hideComponent: true } },
  { path: "/reset", component: ForgotPasswordView, name: "reset", meta: { requiresGuest: true, hideComponent: true } },
  { path: "/trackers", component: TrackerView, name: "trackers", meta: { requiresAuth: true } },
  { path: "/contact", component: ContactFormView, name: "contact", meta: { requiresAuth: true } },
  { path: "/privacy", component: PrivacyPolicyView, name: "privacy" },
  { path: "/terms", component: Nutzungbedingungen, name: "terms" },
  { path: "/uber", component: Uber, name: "uber" },
];

const router = createRouter({
  history: createWebHistory(),
  routes,
});

// **UPDATED Navigation Guard**
router.beforeEach(async (to, from, next) => {
  const authStore = useAuthStore();

  // Attempt to restore user authentication
  if (!authStore.authReady) {
    await authStore.attempt();
  }

  // **HANDLE NAVIGATION TO HOME ('/')**
  if (to.name === "home") {
    if (authStore.isAuthenticated) {
      return next({ name: "main" }); // Redirect to /map
    } else {
      return next({ name: "login" }); // Redirect to /login
    }
  }

  // **CHECK IF THE ROUTE REQUIRES AUTHENTICATION**
  if (to.meta.requiresAuth && !authStore.isAuthenticated) {
    // Redirect to the login page
    return next({ name: "login", query: { redirect: to.fullPath } });
  }
  // **CHECK IF THE ROUTE IS FOR GUESTS ONLY**
  else if (to.meta.requiresGuest && authStore.isAuthenticated) {
    // Redirect to the main page
    return next({ name: "main" });
  } else {
    return next();
  }
});

export default router;
