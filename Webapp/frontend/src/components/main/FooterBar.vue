<template>

  <div :class="['footer-wrapper', (user.settings?.template ?? 'default') === 'dark' ? 'dark-mode' : '']">
    <div class="footer-basic">
      <footer>
        <!-- Icons with functionnalities -->
        <div class="social">
          <a href="https://www.instagram.com" target="_blank" rel="noopener noreferrer">
            <i class="icon ion-social-instagram"></i>
          </a>
          <a href="https://www.snapchat.com" target="_blank" rel="noopener noreferrer">
            <i class="icon ion-social-snapchat"></i>
          </a>
          <a href="https://www.twitter.com" target="_blank" rel="noopener noreferrer">
            <i class="icon ion-social-twitter"></i>
          </a>
          <a href="https://www.facebook.com" target="_blank" rel="noopener noreferrer">
            <i class="icon ion-social-facebook"></i>
          </a>
        </div>
        <ul class="list-inline">
                  <!-- Contact formular -->
          <li class="list-inline-item">
            <router-link to="/contact">Contact us</router-link> 
          </li>
          <li class="list-inline-item"><a href="#">Services</a></li>
          <li class="list-inline-item"><a href="#">About</a></li>
          <li class="list-inline-item"><a href="#">Terms</a></li>
          <li class="list-inline-item">
            <router-link to="/privacy">Privacy Policy</router-link>
          </li>
        </ul>
        <p class="copyright">BO-Tracker © 2024</p>
      </footer>
    </div>
  </div>
</template>

<script setup>
import { computed, onMounted } from 'vue';
import { useAuthStore } from '@/stores/auth'; // Adjust the path as needed

const authStore = useAuthStore();
const user = computed(() => authStore.userDetail);

onMounted(async () => {
  await authStore.getUser();
});
</script>

<style scoped>
.footer-wrapper {
  display: flex;
  justify-content: center;
  align-items: center;

}

.social {
  color: #ffffff;
}

.list-inline-item {
  color: #ffffff;
}

.footer-basic {
  background-color: #00543D;
  padding: 2.2rem;
  width: 100%;
  text-align: center;
}

.dark-mode .footer-basic {
  background-color: #1e1e1e;
  /* Darker green for dark mode */
}

.footer-basic .social {
  text-align: center;
  /* Space between icons and links */
}

.footer-basic .social>a {
  font-size: 24px;
  width: 40px;
  height: 40px;
  line-height: 40px;
  display: inline-block;
  text-align: center;
  margin: 0 8px;
  color: inherit;
  opacity: 0.75;
}

.footer-basic .social>a:hover {
  opacity: 0.9;
}

.footer-basic ul {
  padding: 0;
  list-style: none;
  text-align: center;
  font-size: 18px;
  line-height: 1.6;
  margin-bottom: 20px;
  /* Space between links and copyright */
}

.footer-basic ul a {
  color: inherit;
  text-decoration: none;
  opacity: 0.8;
}

.footer-basic ul a:hover {
  opacity: 1;
}

.footer-basic ul .list-inline-item {
  display: inline-block;
  /* Ensure links are inline */
  margin: 0 10px;
  /* Add spacing between the links */
}

.footer-basic p {
  margin-top: 15px;
  font-size: 13px;
  text-align: center;
  color: #aaa;
}

.dark-mode .footer-basic p {
  color: #ddd;
  /* Lighter text in dark mode */
}
</style>
