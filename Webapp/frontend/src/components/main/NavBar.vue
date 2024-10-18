<template>
  <nav :class="['navbar', (user.template ?? 'default') === 'dark' ? 'dark-mode' : '']">
    <div class="nav-container">
      <!-- Centered Navigation Items -->
      <div class="nav-center">
        <ul class="nav-list">
          <li class="nav-item">
            <a href="#" @click.prevent="goToHome">Map</a>
          </li>
          <li class="nav-item">
            <a ref="tour2" href="#" @click.prevent="goToTrackers">Trackers</a>
          </li>
          <li class="nav-item">
            <RouterLink to="account">Account</RouterLink>
          </li>

          <!-- Help Dropdown -->
          <li class="nav-item help-dropdown">
            <span class="help-link">Help</span>
            <ul class="dropdown-menu">
              <li @click.prevent="startTour">Tour</li>
              <li @click.prevent="contactSupport">Contact</li>
            </ul>
          </li>
        </ul>
      </div>

      <!-- Logout Button -->
      <div class="nav-right">
        <button class="logout-button" @click="logout">
          <span class="logout-text">Logout</span>
          <img src="/src/assets/logout-icon.png" alt="Logout" class="logout-icon" />
        </button>
      </div>
    </div>
  </nav>
</template>

<script setup>
import { useRouter } from 'vue-router';
import { computed, onMounted } from 'vue';

import { useAuthStore } from '@/stores/auth';
import { useTour } from '@/routes/TourController.js';  // Import useTour
import { tour2 } from '@/routes/tourRefs.js';
const router = useRouter();
const authStore = useAuthStore();
const user = computed(() => authStore.userDetail);
onMounted(async () => {
  await authStore.getUser();
});

// Navigation functions
const goToHome = () => {
  router.push({ name: 'main' }); // Replace with your home route
};

const goToTrackers = () => {
  router.push({ name: 'trackers' }); // Replace with your trackers route
};

// Start the tour when "Tour" is clicked
const startTour = () => {
  useTour(router);  // Start the tour without passing a DOM element here
};

const contactSupport = () => {
  router.push({ name: 'contact' }); // Replace with your contact route
};

// Logout function
const logout = async () => {
  try {
    await authStore.logout();
    router.replace({ name: 'login' }); // Redirect to the login page after logout
  } catch (err) {
    console.log(err.message);
  }
};
</script>

<style scoped>
/* Navbar Container */
.navbar {
  position: sticky;
  top: 0;
  z-index: 999;
  background-color: #00543D;
  padding: 10px 0px;
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
  font-family: 'Poppins', sans-serif;
}

/* Navbar Flexbox Container */
.nav-container {
  display: flex;
  justify-content: space-between;
  align-items: center;
  width: 100%;
  padding: 0 0px;
}

/* Centered Navigation Items */
.nav-center {
  display: flex;
  justify-content: center;
  flex-grow: 1;

}

/* Navbar List */
.nav-list {
  display: flex;
  justify-content: center;
  align-items: center;
  list-style: none;
  padding-left: 0;
  margin: 0;
}

/* Navbar Items */
.nav-item {
  margin: 0 20px;
  /* Add spacing between items */
}

/* Navbar Links */
.nav-item a,
.nav-item button,
.help-link {
  text-decoration: none;
  color: #ecf0f1;
  font-size: 18px;
  font-weight: 600;
  background: none;
  border: none;
  cursor: pointer;
  padding: 6px 12px;
  border-radius: 4px;
  transition: color 0.3s, text-decoration 0.3s;
  display: flex;
  align-items: center;
}

/* Help Dropdown */
.help-dropdown {
  position: relative;
}

.dropdown-menu {
  display: none;
  position: absolute;
  top: 100%;
  left: 0;
  background-color: #ffffff;
  padding: 8px 0;
  border-radius: 4px;
  box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
  z-index: 1000;
  border: 1px solid #00543D;

}

.help-dropdown:hover .dropdown-menu {
  display: block;
}

/* Dropdown Items */
.dropdown-menu li {
  padding: 4px 16px;
  font-size: 16px;
  color: #333;
  cursor: pointer;
  white-space: nowrap;
}

.dropdown-menu li:hover {
  background-color: #00543D;
  color: #ffffff;
}

/* Logout Section - Moved to the Right */
.nav-right {
  display: flex;
  align-items: center;
  justify-content: flex-end;
  margin-left: auto;
}

/* Logout Button */
.logout-button {
  display: flex;
  align-items: center;
  padding: 0;
  font-size: 18px;
  font-weight: 600;
  color: #ecf0f1;
  background: none;
  border: none;
  cursor: pointer;
  transition: color 0.3s;
}

/* Logout Text */
.logout-text {
  margin-right: 8px;
}

/* Logout Icon */
.logout-icon {
  width: 24px;
  height: 24px;
  padding-right: 10px;
}

/* Hover Effects */
.nav-item a:hover,
.nav-item button:hover,
.help-link:hover {
  text-decoration: underline;
  text-decoration-color: white;
}

/* Active/Focus Effects */
.nav-item a:focus,
.nav-item button:focus,
.help-link:focus {
  outline: none;
  color: #ffffff;
  text-decoration: underline;
  text-decoration-color: white;
}

/* Dark Mode for Navbar */
.navbar.dark-mode {
  background-color: #00261C;
  /* Darker background for the navbar */
}

.dark-mode .nav-item a,
.dark-mode .nav-item button,
.dark-mode .help-link {
  color: #bbb;
  /* Lighter text color for dark mode */
}

.dark-mode .logout-button {
  color: #bbb;
  /* Lighter text color for logout button */
}

.dark-mode .dropdown-menu {
  background-color: #333;
  /* Darker dropdown background */
  border-color: #444;
}

.dark-mode .dropdown-menu li {
  color: #bbb;
  /* Light text for dropdown items */
}

.dark-mode .dropdown-menu li:hover {
  background-color: #555;
  /* Darker hover color for dropdown */
  color: #fff;
}


/* Responsive Design for Mobile */
@media (max-width: 768px) {
  .navbar {
    padding: 10px;
  }

  .nav-item {
    margin: 0 10px;
  }

  .nav-item a,
  .nav-item button {
    padding: 6px 10px;
    font-size: 14px;
  }

  .logout-icon {
    width: 20px;
    height: 20px;
  }

  .logout-text {
    font-size: 14px;
  }
}



/* Add more spacing between nav items on larger screens */
@media (min-width: 769px) {
  .nav-item {
    margin: 0 30px;
  }
}

@media (min-width: 1200px) {

  /* You can adjust the min-width value based on your definition of full screen */
  .nav-center {
    margin-left: 100px;
    /* Adjust this value to move the items right in full screen */
  }
}
</style>
