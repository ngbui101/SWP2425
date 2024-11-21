<template>
  <nav :class="['navbar', (user.settings?.template ?? 'default') === 'dark' ? 'dark-mode' : '']">
    <div class="nav-container">
      <!-- Centered Navigation Items -->
      <div class="nav-center">
        <ul class="nav-list">

        <li class="nav-item">
            <a href="#" @click.prevent="goToHome">Map</a>
          </li>
          <li class="nav-item">
            <a href="#" @click.prevent="goToRoutes">Route</a>
          </li>
          <li class="nav-item">
            <a ref="tour2" href="#" @click.prevent="goToTrackers">Tracker</a>
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
import { tour3 } from '@/routes/tourRefs.js';
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
const goToRoutes = () => {
  router.push({ name: 'route' }); // Replace with your home route
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
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.3);
  font-family: 'Poppins', sans-serif;
  border-bottom: 3px solid #C19A6B;


  backdrop-filter: blur(5px);
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
  padding-left: 10;
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
  list-style-type: none;
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
  margin-right: auto;
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




/* Hover Effects */
.nav-item a:hover,
.nav-item button:hover,
.help-link:hover {
  transform: scale(1.1);

}



/* Dark Mode for Navbar */
.navbar.dark-mode {
  background: linear-gradient(135deg, #444 0%, #2e2e2e 50%, #444 100%);
  border-bottom: 3px solid #ddd;
  /* Darker background for the navbar */
}

.dark-mode .nav-item a,
.dark-mode .nav-item button,
.dark-mode .help-link {
  color: #bbb;
  /* Lighter text color for dark mode */
}

.dark-mode .logout-button {
  color: #E69543;
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

  /* Reduce margin between items */
  .nav-item {
    margin: 0 5px;
  }

  /* Smaller font and padding for mobile */
  .nav-item a,
  .nav-item button,
  .help-link,
  .logout-text {
    padding: 5px 8px;
    font-size: 14px;
  }

  /* Adjust logout button and hide icon */



  /* Keep only text in logout button on mobile */
  .logout-button {
    padding: 5px 8px;
    font-size: 14px;
  }
}

/* Larger screens */
@media (min-width: 769px) {
  .nav-item {
    margin: 0 20px;
  }


}


</style>
