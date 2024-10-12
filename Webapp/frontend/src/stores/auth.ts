import { defineStore } from "pinia";
import { useApi, useApiPrivate } from "../composables/useApi";

export interface User {
    _id: string;
    username: string;
    email: string;
    password: string;
    number: string;
    language: string;
    created_at: Date;
    updated_at: Date;
    __v: number;
    refresh_token: string;
    tracker: Array<{ _id: string }>;
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
      user: {} as User,
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
            console.log('Payload being sent:', payload);
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
              console.log('Fetched user:', data); // Debugging line
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
              const { data } = await useApi().post(`/api/auth/refresh`);
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
          async updateLanguage(newLanguage: string) {
            try {
              const { data } = await useApiPrivate().put(`http://localhost:3500/api/users/${this.user._id}`, {
                language: newLanguage
              });
              this.user.language = newLanguage; // Update the email in the store
              return data;
            } catch (error: any) {
              throw new Error(`Failed to update email: ${error.message}`);
            }
          },

        },
    });