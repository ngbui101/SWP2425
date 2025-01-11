<template>
    <div class="about-us">
      <h1>{{ $t("AboutUs-title") }}</h1>
  
      <section>
        <h2>{{ $t("AboutUs-welcome_title") }}</h2>
        <p>{{ $t("AboutUs-welcome_text") }}</p>
      </section>
  
      <section>
        <h2>{{ $t("AboutUs-products_title") }}</h2>
        <p>{{ $t("AboutUs-products_text") }}</p>
      </section>
  
      <section>
        <h2>{{ $t("AboutUs-services_title") }}</h2>
        <ul>
          <li>
            <strong>{{ $t("AboutUs-services_management_title") }}</strong>
            <p>{{ $t("AboutUs-services_management_text") }}</p>
          </li>
          <li>
            <strong>{{ $t("AboutUs-services_tracking_title") }}</strong>
            <p>{{ $t("AboutUs-services_tracking_text") }}</p>
          </li>
          <li>
            <strong>{{ $t("AboutUs-services_notifications_title") }}</strong>
            <p>{{ $t("AboutUs-services_notifications_text") }}</p>
          </li>
        </ul>
      </section>
  
      <section>
        <h2>{{ $t("AboutUs-team_title") }}</h2>
        <p>{{ $t("AboutUs-team_text") }}</p>
      </section>
  
      <section>
        <h2>{{ $t("AboutUs-why_us_title") }}</h2>
        <ul>
          <li>{{ $t("AboutUs-why_us_app") }}</li>
          <li>{{ $t("AboutUs-why_us_tracker") }}</li>
          <li>{{ $t("AboutUs-why_us_support") }}</li>
        </ul>
      </section>
  
      <section>
        <h2>{{ $t("AboutUs-contact_title") }}</h2>
        <p>
          {{ $t("AboutUs-contact_text") }}
          <strong>{{ $t("AboutUs-contact_email") }}</strong><br />
          {{ $t("AboutUs-contact_phone") }}
        </p>
      </section>
    </div>
  </template>
  
  <script setup>
  // Falls zusätzliche Logik benötigt wird
  </script>
  
  <style scoped>
  .about-us {
    padding: 20px;
    font-family: Arial, sans-serif;
    line-height: 1.6;
    color: #333;
  }
  
  .about-us h1 {
    font-size: 2em;
    margin-bottom: 20px;
  }
  
  .about-us h2 {
    margin-top: 20px;
    font-size: 1.5em;
  }
  
  .about-us ul {
    margin: 10px 0;
    padding-left: 20px;
    list-style: disc;
  }
  
  .about-us p {
    margin: 10px 0;
  }
  </style>
  