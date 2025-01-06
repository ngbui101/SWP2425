<template>
  <div class="privacy-policy">
    <h1>{{ $t("PrivacyPolicy-title") }}</h1>
    <p>{{ $t("PrivacyPolicy-effective_date") }}</p>

    <section>
      <h2>{{ $t("PrivacyPolicy-intro-title") }}</h2>
      <p>{{ $t("PrivacyPolicy-intro-text") }}</p>
    </section>

    <section>
      <h2>{{ $t("PrivacyPolicy-info_title") }}</h2>
      <p>{{ $t("PrivacyPolicy-info_text") }}</p>
      <ul>
        <li>{{ $t("PrivacyPolicy-info_personal") }}</li>
        <li>{{ $t("PrivacyPolicy-info_usage") }}</li>
      </ul>
    </section>

    <section>
      <h2>{{ $t("PrivacyPolicy-usage_title") }}</h2>
      <p>{{ $t("PrivacyPolicy-usage_text") }}</p>
      <ul>
        <li>{{ $t("PrivacyPolicy-usage_improve") }}</li>
        <li>{{ $t("PrivacyPolicy-usage_security") }}</li>
        <li>{{ $t("PrivacyPolicy-usage_communication") }}</li>
      </ul>
    </section>

    <section>
      <h2>{{ $t("PrivacyPolicy-protection_title") }}</h2>
      <p>{{ $t("PrivacyPolicy-protection_text") }}</p>
    </section>

    <section>
      <h2>{{ $t("PrivacyPolicy-rights_title") }}</h2>
      <p>{{ $t("PrivacyPolicy-rights_text") }}</p>
      <ul>
        <li>{{ $t("PrivacyPolicy-rights_access") }}</li>
        <li>{{ $t("PrivacyPolicy-rights_correction") }}</li>
        <li>{{ $t("PrivacyPolicy-rights_withdraw") }}</li>
      </ul>
    </section>

    <section>
      <h2>{{ $t("PrivacyPolicy-contact_title") }}</h2>
      <p>
        {{ $t("PrivacyPolicy-contact_text") }}
        <strong>{{ $t("PrivacyPolicy-contact_email") }}</strong>
      </p>
    </section>

    <p>{{ $t("PrivacyPolicy-thank_you") }}</p>
  </div>
</template>
  <script setup>

//if needed 
  </script>
  
  <style scoped>
  .privacy-policy {
    padding: 20px;
    font-family: Arial, sans-serif;
    line-height: 1.6;
    color: #333;
  }
  
  .privacy-policy h1 {
    font-size: 2em;
    margin-bottom: 20px;
  }
  
  .privacy-policy h2 {
    margin-top: 20px;
    font-size: 1.5em;
  }
  
  .privacy-policy ul {
    margin: 10px 0;
    padding-left: 20px;
    list-style: disc;
  }
  
  .privacy-policy p {
    margin: 10px 0;
  }
  </style>
  