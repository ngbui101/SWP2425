<script setup>
import { ref } from 'vue'
import Main from './views/MainView.vue'
import LoginView from './views/LoginView.vue';
import RegisterView from './views/RegisterView.vue';
</script>
<template>
    <div class="container">
      <RouterView />
    </div>
  </template>