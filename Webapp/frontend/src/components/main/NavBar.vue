<template>
  <nav class="navbar">
    <ul class="nav-list">
      <div class="nav-center">
        <li class="nav-item">
          <a href="#" @click.prevent="goToHome">Map</a>
        </li>
        <li class="nav-item">
          <a href="#" @click.prevent="goToTrackers">Trackers</a>
        </li>
        <li class="nav-item">
          <RouterLink to="account">Account</RouterLink>
        </li>
      </div>
      <li class="nav-item logout-item">
        <button class="logout-button" @click="logout">
          <img src="/src/assets/logout-icon.png" alt="Logout" class="logout-icon" />
          <span class="logout-text">Logout</span>
        </button>
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
  top: 0; /* Adjust this if you need it to stick below something */
  z-index: 999;
  background-color: #00543D;
  padding: 10px 20px;
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
  font-family: 'Poppins', sans-serif; /* Ensure Poppins font is applied to all elements */
}

/* Navbar List */
.nav-list {
  display: flex;
  justify-content: space-between; /* Space between center items and logout button */
  align-items: center;
  list-style: none;
  margin: 0;
  padding: 0;
  font-family: inherit; /* Inherit the font from the navbar */
  width: 100%; /* Make the list take up the full width */
}

/* Centered Navbar Items */
.nav-center {
  display: flex;
  justify-content: center;
  align-items: center;
  flex-grow: 1; /* Take up remaining space */
}

/* Navbar Items */
.nav-item {
  margin: 0 20px;
}

/* Logout Item */
.logout-item {
  margin-left: auto; /* Push the Logout button to the end */
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
  position: relative;
}

/* Logout Icon */
.logout-icon {
  width: 24px; /* Adjust size as needed */
  height: 24px; /* Adjust size as needed */
}

/* Logout Text (Hidden by default) */
.logout-text {
  visibility: hidden;
  opacity: 0;
  transition: opacity 0.3s, visibility 0.3s;
  position: absolute;
  bottom: -25px; /* Adjust position as needed */
  left: 50%;
  transform: translateX(-50%);
  background-color: #333;
  color: #fff;
  padding: 2px 6px; /* Reduce padding to make it less thick */
  font-size: 12px; /* Smaller font size for a more compact look */
  border-radius: 3px; /* Slightly rounded corners */
  white-space: nowrap;
  line-height: 1.2; /* Adjust line height for a tighter fit */
}

/* Show Logout Text on Hover */
.logout-button:hover .logout-text {
  visibility: visible;
  opacity: 1;
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

  .logout-icon {
    width: 20px; /* Smaller size for mobile */
    height: 20px; /* Smaller size for mobile */
  }

  .logout-text {
    bottom: -25px; /* Adjust for smaller screens */
  }
  
}
</style>
