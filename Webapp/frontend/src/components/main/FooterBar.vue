<template>
    <div class="footer-wrapper">
      <div class="footer-basic">
        <footer>
          <div class="social">
            <a href="#"><i class="icon ion-social-instagram"></i></a>
            <a href="#"><i class="icon ion-social-snapchat"></i></a>
            <a href="#"><i class="icon ion-social-twitter"></i></a>
            <a href="#"><i class="icon ion-social-facebook"></i></a>
          </div>
          <ul class="list-inline">
            <li class="list-inline-item"><a href="#">Home</a></li>
            <li class="list-inline-item"><a href="#">Services</a></li>
            <li class="list-inline-item"><a href="#">About</a></li>
            <li class="list-inline-item"><a href="#">Terms</a></li>
            <li class="list-inline-item"><a href="#">Privacy Policy</a></li>
          </ul>
          <p class="copyright">Company Name © 2018</p>
        </footer>
      </div>
    </div>
  </template>
  
  <script>
  export default {
    name: "FooterBar",
  };
  </script>
  
  <style scoped>
  .footer-wrapper {
    display: flex;
    justify-content: center;
    align-items: center;
    margin-top: 2rem; /* Optional: add space above the footer */
  }
  
  .footer-basic {
    background-color: #fff;
    border-radius: 7px; /* Match the rounded corners of the login form */
    box-shadow: 0 5px 10px rgba(0, 0, 0, 0.3); /* Add shadow effect */
    padding: 2rem;
    max-width: 800px;
    width: 100%;
    text-align: center;
  }
  
  .footer-basic .social {
    text-align: center;
    margin-bottom: 20px; /* Space between icons and links */
  }
  
  .footer-basic .social > a {
    font-size: 24px;
    width: 40px;
    height: 40px;
    line-height: 40px;
    display: inline-block;
    text-align: center;
    border-radius: 50%;
    border: 1px solid #ccc;
    margin: 0 8px;
    color: inherit;
    opacity: 0.75;
  }
  
  .footer-basic .social > a:hover {
    opacity: 0.9;
  }
  
  .footer-basic ul {
    padding: 0;
    list-style: none;
    text-align: center;
    font-size: 18px;
    line-height: 1.6;
    margin-bottom: 20px; /* Space between links and copyright */
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
    display: inline-block; /* Ensure links are inline */
    margin: 0 10px; /* Add spacing between the links */
  }
  
  .footer-basic p {
    margin-top: 15px;
    font-size: 13px;
    text-align: center;
    margin-bottom: 0;
    color: #aaa;
  }
  </style>
  