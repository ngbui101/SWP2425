<script setup>
import NavBar from './components/main/NavBar.vue';
import LogoHeader from './components/main/LogoHeader.vue';
import FooterBar from './components/main/FooterBar.vue';
import 'shepherd.js/dist/css/shepherd.css';  // Import Shepherd CSS
</script>

<template>
  <LogoHeader v-if="!$route.meta.hideComponent" />
  <NavBar v-if="!$route.meta.hideComponent" />

  <RouterView />

  <FooterBar />
</template>

<style>
/* Your existing styles */


/* Custom styles for Shepherd tour */
.shepherd-element {
  background-color: #3ff5e6 !important;
  /* Same background as navbar */
  color: #ffffff !important;
  /* White font */
  font-family: 'Poppins', sans-serif !important;
  /* Poppins font */
  border-radius: 8px;
  /* Optional rounded corners */
  padding: 15px !important;
  /* Custom padding */
  border: 1px solid #ffffff !important;
  margin-top: 12px;
}

.shepherd-title {
  font-size: 20px;
  font-weight: bold;
  color: #ffffff !important;
}

.shepherd-text {
  font-size: 16px;
  color: #000000 !important;
}

/* Custom styles for Shepherd buttons */
.shepherd-button {
  background-color: #ffffff !important;
  /* White button */
  color: #000000 !important;
  /* Navbar color text */
  font-family: 'Poppins', sans-serif !important;
  border-radius: 4px;
  padding: 8px 12px;
}

.shepherd-button:hover {
  background-color: #99eed6 !important;
  /* Darker shade on hover */
}

/* Customize modal overlay */
.shepherd-modal-overlay-container {
  background-color: rgba(0, 0, 0, 0.7) !important;
  /* Darker modal overlay */
}

/* Custom highlight for the Shepherd tour */
.shepherd-highlight {
  box-shadow: 0 0 15px 5px rgba(255, 255, 255, 0.8) !important;
  /* Yellow glow */
  background-color: rgba(255, 255, 255, 0.5) !important;
  /* Semi-transparent yellow */

  /* Rounded corners */
  z-index: 10000 !important;
  /* Ensure it's on top of other elements */
}
</style>
