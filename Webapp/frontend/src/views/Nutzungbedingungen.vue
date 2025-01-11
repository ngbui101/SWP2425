<template>
    <div class="terms-of-service">
      <h1>{{ $t("TermsOfService-title") }}</h1>
      <p>{{ $t("TermsOfService-effective_date") }}</p>
  
      <section>
        <h2>{{ $t("TermsOfService-intro-title") }}</h2>
        <p>{{ $t("TermsOfService-intro-text") }}</p>
      </section>
  
      <section>
        <h2>{{ $t("TermsOfService-usage_title") }}</h2>
        <p>{{ $t("TermsOfService-usage_text") }}</p>
        <ul>
          <li>{{ $t("TermsOfService-usage_tracker") }}</li>
          <li>{{ $t("TermsOfService-usage_account") }}</li>
          <li>{{ $t("TermsOfService-usage_restrictions") }}</li>
        </ul>
      </section>
  
      <section>
        <h2>{{ $t("TermsOfService-responsibility_title") }}</h2>
        <p>{{ $t("TermsOfService-responsibility_text") }}</p>
      </section>
  
      <section>
        <h2>{{ $t("TermsOfService-changes_title") }}</h2>
        <p>{{ $t("TermsOfService-changes_text") }}</p>
      </section>
  
      <section>
        <h2>{{ $t("TermsOfService-contact_title") }}</h2>
        <p>
          {{ $t("TermsOfService-contact_text") }}
          <strong>{{ $t("TermsOfService-contact_email") }}</strong>
        </p>
      </section>
    </div>
  </template>
  
  <script setup>
  // Optionales JavaScript hier hinzufügen
  </script>
  
  <style scoped>
  .terms-of-service {
    padding: 20px;
    font-family: Arial, sans-serif;
    line-height: 1.6;
    color: #333;
  }
  
  .terms-of-service h1 {
    font-size: 2em;
    margin-bottom: 20px;
  }
  
  .terms-of-service h2 {
    margin-top: 20px;
    font-size: 1.5em;
  }
  
  .terms-of-service ul {
    margin: 10px 0;
    padding-left: 20px;
    list-style: disc;
  }
  
  .terms-of-service p {
    margin: 10px 0;
  }
  </style>
  