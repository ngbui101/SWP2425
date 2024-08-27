<template>
    <div class="container">
      <div class="registration form">
        <header>Signup</header>
        <form @submit.prevent="handleSignup">
          <input type="text" v-model="email" placeholder="Enter your email">
          <input type="password" v-model="password" placeholder="Create a password">
          <input type="password" v-model="confirmPassword" placeholder="Confirm your password">
          <input type="submit" class="button" value="Signup">
        </form>
        <div class="signup">
          <span class="signup">Already have an account?
            <label @click="goToLogin">Login</label>
          </span>
        </div>
      </div>
    </div>
  </template>
  
  <script>
  export default {
    data() {
      return {
        email: '',
        password: '',
        confirmPassword: ''
      };
    },
    methods: {
      handleSignup() {
        // Handle signup logic here
        if (this.password !== this.confirmPassword) {
          alert("Passwords do not match!");
          return;
        }
        console.log('Email:', this.email);
        console.log('Password:', this.password);
      },
      goToLogin() {
        this.$emit('showLogin');
      }
    }
  };
  </script>
  
  <style scoped>
  @import url('https://fonts.googleapis.com/css2?family=Poppins:wght@200;300;400;500;600;700&display=swap');
  
  * {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
    font-family: 'Poppins', sans-serif;
  }
  
  body {
    min-height: 100vh;
    width: 100%;
    background: #009579;
  }
  
  .container {
    position: absolute;
    top: 50%;
    left: 50%;
    transform: translate(-50%, -50%);
    max-width: 430px;
    width: 100%;
    background: #fff;
    border-radius: 7px;
    box-shadow: 0 5px 10px rgba(0, 0, 0, 0.3);
  }
  
  .container .form {
    padding: 2rem;
  }
  
  .form header {
    font-size: 2rem;
    font-weight: 500;
    text-align: center;
    margin-bottom: 1.5rem;
  }
  
  .form input {
    height: 60px;
    width: 100%;
    padding: 0 15px;
    font-size: 17px;
    margin-bottom: 1.3rem;
    border: 1px solid #ddd;
    border-radius: 6px;
    outline: none;
  }
  
  .form input:focus {
    box-shadow: 0 1px 0 rgba(0, 0, 0, 0.2);
  }
  
  .form a {
    font-size: 16px;
    color: #009579;
    text-decoration: none;
  }
  
  .form a:hover {
    text-decoration: underline;
  }
  
  .form input.button {
    color: #fff;
    background: #009579;
    font-size: 1.2rem;
    font-weight: 500;
    letter-spacing: 1px;
    margin-top: 1.7rem;
    cursor: pointer;
    transition: 0.4s;
  }
  
  .form input.button:hover {
    background: #006653;
  }
  
  .signup {
    font-size: 17px;
    text-align: center;
  }
  
  .signup label {
    color: #009579;
    cursor: pointer;
  }
  
  .signup label:hover {
    text-decoration: underline;
  }
  </style>
  