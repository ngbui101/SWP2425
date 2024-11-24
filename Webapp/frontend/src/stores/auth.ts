import { defineStore } from "pinia";
import { useApi, useApiPrivate } from "../composables/useApi";

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
      mode: string[];  // Array of strings to store selected modes, e.g., ['RT', 'LT']
    };
    timestampFilters: {
      mode: string[];  // Array of strings to store selected modes, e.g., ['RT', 'LT']
      validPosition: boolean;
    };
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
        async attempt() {
          try {
            await this.refresh();
            await this.getUser();
          } catch (error) {
            console.error("Error in attempt:", error);
          }
        },
    
        async login(payload: LoginData) {
          try {
            const { data } = await useApi().post(`http://localhost:3500/api/auth/login`, payload);
            this.accessToken = data.access_token;
            await this.getUser();
            return data;
          } catch (error: any) {
            throw new Error(`Login failed: ${error.message}`);
          }
        },
    
        async register(payload: RegisterData) {
          try {
            
            const { data } = await useApi().post('http://localhost:3500/api/auth/register', payload);
            return data;
          } catch (error: any) {
            if (error.response) {
              console.error('Server response:', error.response.data); // Detailed server error
            }
            throw new Error(`Registration failed: ${error.message}`);
          }
        },
        async getUser() {
          try {
              const { data } = await useApiPrivate().get(`http://localhost:3500/api/auth/user`);

              this.user = data;
              
              return data;
          } catch (error: any) {
              throw new Error(`Failed to fetch user: ${error.message}`);
          }
      },
      
      
          async logout() {
            try {
              const { data } = await useApiPrivate().post(`http://localhost:3500/api/auth/logout`);
              this.accessToken = "";
              this.user = {} as User;
              return data;
            } catch (error: any) {
              throw new Error(`Logout failed: ${error.message}`);
            }
          },
      
          async refresh() {
            try {
              const { data } = await useApi().post(`http://localhost:3500/api/auth/refresh`);
              this.accessToken = data.access_token;
              return data;
            } catch (error: any) {
              throw new Error(`Failed to refresh token: ${error.message}`);
            }
          },

          async fetchAllUsers() {
            try {
              const { data } = await useApiPrivate().get("/api/users/", {
                headers: {
                  Authorization: `Bearer ${this.accessToken}`,
                },
              });
              this.users = data;
              return data;
            } catch (error: any) {
              throw new Error(`Failed to fetch users: ${error.message}`);
            }
          },
          async fetchUserSettings() {
            try {
              const { data } = await useApiPrivate().get(`/api/settings`);
              this.user.settings = data;
              return data;
            } catch (error: any) {
              throw new Error(`Failed to fetch settings: ${error.message}`);
            }
          },
          async updateUserSettings(updatedSettings: Partial<Settings>) {
            try {
              const { data } = await useApiPrivate().put(`/api/settings`, updatedSettings);
              this.user.settings = { ...this.user.settings, ...updatedSettings }; // Update locally
              return data;
            } catch (error: any) {
              throw new Error(`Failed to update settings: ${error.message}`);
            }
          },
          async updatePassword(currentPassword: string, newPassword: string, confirmPassword: string) {
            try {
              const { data } = await useApiPrivate().put(`http://localhost:3500/api/users/${this.user._id}`, {
                currentPassword,
                newPassword,
                confirmPassword
              });
          
              return data; // Return the response data if needed
            } catch (error: any) {
              throw new Error(`Failed to update password: ${error.message}`);
            }
          },
          async updatePhoneNumber(newPhoneNumber: string) {
            try {
              const { data } = await useApiPrivate().put(`http://localhost:3500/api/users/${this.user._id}`, {
                number: newPhoneNumber
              });
              return data;
            } catch (error: any) {
              throw new Error(`Failed to update phone number: ${error.message}`);
            }
          },          
          
          async updateEmail(newEmail: string) {
            try {
              const { data } = await useApiPrivate().put(`http://localhost:3500/api/users/${this.user._id}`, {
                email: newEmail
              });
              this.user.email = newEmail; // Update the email in the store
              return data;
            } catch (error: any) {
              throw new Error(`Failed to update email: ${error.message}`);
            }
          },
          async updateSettings(settings) {
            try {
              const { data } = await useApiPrivate().put('http://localhost:3500/api/settings', settings); // No userId in the URL
              this.user.settings = data; // Update the user's settings in the store with the response data
              
            } catch (error) {
              
              throw new Error(`Failed to update settings: ${error.message}`);
            }
          },
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
                    ? (frequencySeconds ? frequencySeconds * 1000 : 5000) // Use frequencySeconds for real-time mode, default to 5000ms if not provided
                    : (frequencyHours ? frequencyHours * 60 * 60 * 1000 : 3600000), // Use frequencyHours for long-time mode, default to 1 hour
            };
        
            try {
                const { data } = await useApiPrivate().put(`http://localhost:3500/api/mode/${trackerId}`, updateData);
        
                return data;
            } catch (error: any) {
                console.error('Failed to update tracker mode:', error);
                throw new Error(`Failed to update tracker mode: ${error.message}`);
            }
        }
        
        
          
          
          
         
          
          
          

        },
    });