<script setup>
import NavBar from './components/main/NavBar.vue';
import LogoHeader from './components/main/LogoHeader.vue';
import FooterBar from './components/main/FooterBar.vue';
</script>
<template>
    
      <LogoHeader v-if="!$route.meta.hideComponent" />
      <NavBar v-if="!$route.meta.hideComponent" />
      <div class="boxi"><RouterView />
      </div>
      <FooterBar />
  </template>


<style>
.boxi {
  background: rgb(0,84,61);
  background: linear-gradient(0deg, rgba(0,84,61,1) 0%, rgba(255,255,255,1) 70%);
}
</style>