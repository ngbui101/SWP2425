<template>
    <nav class="navbar">
      <ul class="nav-list">
        <li class="nav-item">
          <a href="#" @click.prevent="goToHome">Map</a>
        </li>
        <li class="nav-item">
          <a href="#" @click.prevent="goToTrackers">Trackers</a>
        </li>
        <li class="nav-item">
          <RouterLink to="account">Account</RouterLink>
        </li>
        
        <li class="nav-item">
          <button class="logout-button" @click="logout">Logout</button>
        </li>
      </ul>
    </nav>
  </template>
  
  <script setup>
  import { useRouter } from 'vue-router'
  import { useAuthStore } from '@/stores/auth'
  
  const router = useRouter()
  const authStore = useAuthStore()
  
  // Navigation functions
  const goToHome = () => {
    router.push({ name: 'main' }) // Replace with your home route
  }
  
  const goToAccount = () => {
    router.push({ name: 'account' }) // Replace with your account route
  }
  
  const goToTrackers = () => {
    router.push({ name: 'trackers' }) // Replace with your trackers route
  }
  
  // Logout function
  const logout = async () => {
    try {
      await authStore.logout()
      router.replace({ name: 'login' }) // Redirect to the login page after logout
    } catch (err) {
      console.log(err.message)
    }
  }
  </script>
  
  <style scoped>
  /* Navbar Container */
  .navbar {
    position: sticky;
    top: 91px;
    z-index: 999;
    background-color: #00543D;
    padding: 10px 20px;
    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
    font-family: 'Poppins', sans-serif; /* Ensure Poppins font is applied to all elements */
  }
  
  /* Navbar List */
  .nav-list {
    display: flex;
    justify-content: center;
    align-items: center;
    list-style: none;
    margin: 0;
    padding: 0;
    font-family: inherit; /* Inherit the font from the navbar */
  }
  
  /* Navbar Items */
  .nav-item {
    margin: 0 20px;
  }
  
  /* Navbar Links and Buttons */
  .nav-item a,
  .nav-item button {
    text-decoration: none;
    color: #ecf0f1;
    font-size: 16px;
    font-weight: 500;
    background: none;
    border: none;
    cursor: pointer;
    padding: 8px 16px;
    border-radius: 4px;
    transition: color 0.3s, text-decoration 0.3s;
    display: flex;
    align-items: center;
    font-family: inherit; /* Inherit font from the parent */
  }
  
  /* Hover Effects */
  .nav-item a:hover,
  .nav-item button:hover {
    text-decoration: underline;
    text-decoration-color: white; /* White underline */
  }
  
  /* Active/Focus Effects */
  .nav-item a:focus,
  .nav-item button:focus {
    outline: none;
    color: #ffffff;
    text-decoration: underline;
    text-decoration-color: white;
  }
  
  /* Responsive Design */
  @media (max-width: 768px) {
    .navbar {
      padding: 10px;
      
    }
  
    .nav-item {
      margin: 0 10px;
    }
  
    .nav-item a,
    .nav-item button {
      padding: 8px 12px;
      font-size: 14px;
    }
  }
  </style>
  