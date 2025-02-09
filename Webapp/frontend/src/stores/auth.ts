// src/stores/auth.ts

import { defineStore } from "pinia";
// **CHANGED**: Remove useApi and useApiPrivate imports
// import { useApi, useApiPrivate } from "../composables/useApi";
import axios from "axios"; // **CHANGED**: Import Axios directly
import { ref } from "vue";
import { useRouter } from "vue-router"; // **ADDED**: Import useRouter for redirection

export const currentView = ref("current");

export interface Settings {
  _id: string;
  template: string;
  language: string;
  notifications: {
    sms: {
      geofence: boolean;
      motionSensor: boolean;
      batteryLow: boolean;
    };
    email: {
      geofence: boolean;
      motionSensor: boolean;
      batteryLow: boolean;
    };
  };
  trackerFilters: {
    mode: string[]; // Array of strings to store selected modes, e.g., ['RT', 'LT']
  };
  timestampFilters: {
    mode: string[]; // Array of strings to store selected modes, e.g., ['RT', 'LT']
    validPosition: boolean;
  };
}

export interface User {
  _id: string;
  username: string;
  email: string;
  password: string;
  number: string;
  language: string;
  template: string;
  created_at: Date;
  updated_at: Date;
  __v: number;
  refresh_token: string;
  tracker: Array<{ _id: string }>;
  settings: Settings;
}

export interface State {
  user: User;
  accessToken: string;
  authReady: boolean;
  users: User[];
}

export interface LoginData {
  email: string;
  password: string;
}

export interface RegisterData {
  email: string;
  password: string;
  password_confirm: string;
}

export const useAuthStore = defineStore({
  id: "auth",
  state: (): State => ({
    user: { settings: {} } as User,
    accessToken: "",
    authReady: false,
    users: [],
  }),

  getters: {
    userDetail: (state: State) => state.user,
    isAuthenticated: (state: State) => !!state.accessToken,
    userList: (state: State) => state.users,
  },

  actions: {
    // Initialize the store on app start
    initializeStore() {
      const token = localStorage.getItem("accessToken");
      if (token) {
        this.accessToken = token;
        console.log("Access Token initialized from localStorage:", this.accessToken); // Debugging line
        this.getUser();
      } else {
        this.authReady = true; // No token to initialize
      }
    },

    // Attempt to authenticate the user
    async attempt() {
      try {
        const token = localStorage.getItem("accessToken");
        if (token) {
          this.accessToken = token;
          
          await this.getUser();
        }
      } catch (error) {
        console.error("Error in attempt:", error);
      } finally {
        this.authReady = true;
      }
    },

    // Login action
    async login(payload: LoginData) {
      try {
        // **CHANGED**: Use Axios directly with 'withCredentials: true' to include HTTP-only cookies
        const { data } = await axios.post(
          "http://localhost:3500/api/auth/login",
          payload,
          { withCredentials: true }
        );
        this.accessToken = data.access_token;
        // Speicherung in localStorage
        localStorage.setItem("accessToken", data.access_token); // Speichere das Token im localStorage
        await this.getUser();
        return data;
      } catch (error: any) {
        throw new Error(`Login failed: ${error.message}`);
      }
    },

    // Register action
    async register(payload: RegisterData) {
      try {
        // **CHANGED**: Use Axios directly
        const { data } = await axios.post(
          "http://localhost:3500/api/auth/register",
          payload
        );
        return data;
      } catch (error: any) {
        if (error.response) {
          console.error("Server response:", error.response.data); // Detailed server error
        }
        throw new Error(`Registration failed: ${error.message}`);
      }
    },

    // Fetch user details
    async getUser() {
      try {
        // **CHANGED**: Use Axios directly with Authorization header
        const { data } = await axios.get("http://localhost:3500/api/auth/user", {
          headers: { Authorization: `Bearer ${this.accessToken}` },
          withCredentials: true, // Include cookies if needed
        });
        this.user = data;
        
        return data;
      } catch (error: any) {
        throw new Error(`Failed to fetch user: ${error.message}`);
      }
    },

    // Logout action
    async logout() {
      try {
        // **CHANGED**: Use Axios directly with Authorization header and 'withCredentials: true'
        const { data } = await axios.post(
          "http://localhost:3500/api/auth/logout",
          {},
          {
            headers: { Authorization: `Bearer ${this.accessToken}` },
            withCredentials: true, // Include cookies if needed
          }
        );
        this.accessToken = "";
        this.user = {} as User;
        // Entferne Token
        localStorage.removeItem("accessToken"); // Entferne das Token aus dem localStorage
        return data;
      } catch (error: any) {
        throw new Error(`Logout failed: ${error.message}`);
      } finally {
        // **ADDED**: Redirect to login after logout
        const router = useRouter();
        router.replace({ name: "login" });
      }
    },

    // Refresh access token
    async refresh() {
      try {
        // **CHANGED**: Use Axios directly with 'withCredentials: true' to include HTTP-only cookies
        const { data } = await axios.post(
          "http://localhost:3500/api/auth/refresh",
          {},
          { withCredentials: true }
        );
        this.accessToken = data.access_token;
        localStorage.setItem("accessToken", data.access_token);
        return data;
      } catch (error: any) {
        throw new Error(`Failed to refresh token: ${error.message}`);
      }
    },

    // Fetch all users
    async fetchAllUsers() {
      try {
        // **CHANGED**: Use Axios directly with Authorization header
        const { data } = await axios.get("/api/users/", {
          headers: {
            Authorization: `Bearer ${this.accessToken}`,
          },
          withCredentials: true, // Include cookies if needed
        });
        this.users = data;
        return data;
      } catch (error: any) {
        throw new Error(`Failed to fetch users: ${error.message}`);
      }
    },

    // Fetch user settings
    async fetchUserSettings() {
      try {
        // **CHANGED**: Use Axios directly with Authorization header
        const { data } = await axios.get("/api/settings", {
          headers: {
            Authorization: `Bearer ${this.accessToken}`,
          },
          withCredentials: true, // Include cookies if needed
        });
        this.user.settings = data;
        return data;
      } catch (error: any) {
        throw new Error(`Failed to fetch settings: ${error.message}`);
      }
    },

    // Update user settings
    async updateUserSettings(updatedSettings: Partial<Settings>) {
      try {
        // **CHANGED**: Use Axios directly with Authorization header
        const { data } = await axios.put("/api/settings", updatedSettings, {
          headers: {
            Authorization: `Bearer ${this.accessToken}`,
          },
          withCredentials: true, // Include cookies if needed
        });
        this.user.settings = { ...this.user.settings, ...updatedSettings }; // Update locally
        return data;
      } catch (error: any) {
        throw new Error(`Failed to update settings: ${error.message}`);
      }
    },

    // Update password
    async updatePassword(
      currentPassword: string,
      newPassword: string,
      confirmPassword: string
    ) {
      try {
        // **CHANGED**: Use Axios directly with Authorization header
        const { data } = await axios.put(
          `http://localhost:3500/api/users/${this.user._id}`,
          {
            currentPassword,
            newPassword,
            confirmPassword,
          },
          {
            headers: {
              Authorization: `Bearer ${this.accessToken}`,
            },
            withCredentials: true, // Include cookies if needed
          }
        );

        return data; // Return the response data if needed
      } catch (error: any) {
        throw new Error(`Failed to update password: ${error.message}`);
      }
    },

    // Update phone number
    async updatePhoneNumber(newPhoneNumber: string) {
      try {
        // **CHANGED**: Use Axios directly with Authorization header
        const { data } = await axios.put(
          `http://localhost:3500/api/users/${this.user._id}`,
          {
            number: newPhoneNumber,
          },
          {
            headers: {
              Authorization: `Bearer ${this.accessToken}`,
            },
            withCredentials: true, // Include cookies if needed
          }
        );
        return data;
      } catch (error: any) {
        throw new Error(`Failed to update phone number: ${error.message}`);
      }
    },

    // Update email
    async updateEmail(newEmail: string) {
      try {
        // **CHANGED**: Use Axios directly with Authorization header
        const { data } = await axios.put(
          `http://localhost:3500/api/users/${this.user._id}`,
          {
            email: newEmail,
          },
          {
            headers: {
              Authorization: `Bearer ${this.accessToken}`,
            },
            withCredentials: true, // Include cookies if needed
          }
        );
        this.user.email = newEmail; // Update the email in the store
        return data;
      } catch (error: any) {
        throw new Error(`Failed to update email: ${error.message}`);
      }
    },

    // Update settings
    async updateSettings(settings: Partial<Settings>) {
      try {
        // **CHANGED**: Use Axios directly with Authorization header
        const { data } = await axios.put(
          "http://localhost:3500/api/settings",
          settings,
          {
            headers: {
              Authorization: `Bearer ${this.accessToken}`,
            },
            withCredentials: true, // Include cookies if needed
          }
        ); // No userId in the URL
        this.user.settings = data; // Update the user's settings in the store with the response data

      } catch (error: any) {
        throw new Error(`Failed to update settings: ${error.message}`);
      }
    },

    // Update tracker mode
    async updateTrackerMode(
      trackerId: string,
      isRealTime: boolean,
      frequencySeconds?: number,
      frequencyHours?: number,
      batteryMode?: boolean,
      temperatureMode?: boolean
    ) {
      const updateData = {
        GnssMode: isRealTime,
        CellInfosMode: !isRealTime,
        BatteryMode: batteryMode ?? true, // Default to true if not provided
        TemperatureMode: temperatureMode ?? true, // Default to true if not provided
        frequenz: isRealTime
          ? frequencySeconds
            ? frequencySeconds * 1000
            : 5000 // Use frequencySeconds for real-time mode, default to 5000ms if not provided
          : frequencyHours
          ? frequencyHours * 60 * 60 * 1000
          : 3600000, // Use frequencyHours for long-time mode, default to 1 hour
      };

      try {
        // **CHANGED**: Use Axios directly with Authorization header
        const { data } = await axios.put(
          `http://localhost:3500/api/mode/${trackerId}`,
          updateData,
          {
            headers: {
              Authorization: `Bearer ${this.accessToken}`,
            },
            withCredentials: true, // Include cookies if needed
          }
        );

        return data;
      } catch (error: any) {
        console.error("Failed to update tracker mode:", error);
        throw new Error(`Failed to update tracker mode: ${error.message}`);
      }
    },
  },
});
