<script setup>
import NavBar from './components/main/NavBar.vue';
import LogoHeader from './components/main/LogoHeader.vue';
</script>
<template>
    
      <LogoHeader v-if="!$route.meta.hideComponent" />
      <NavBar v-if="!$route.meta.hideComponent" />
      <RouterView />
    
  </template>


<style>

</style>