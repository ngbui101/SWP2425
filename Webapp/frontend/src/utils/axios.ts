import axios from "axios"

axios.defaults.withCredentials = true;

export const axiosInstance = axios.create({
  baseURL: "http://localhost:5173/",
  withCredentials: true,
  headers: {
    'Content-Type': 'application/json',
  }
})

export const axiosPrivateInstance = axios.create({
  baseURL: "http://localhost:5173/",
  withCredentials: true,
  headers: {
    'Content-Type': 'application/json',
  }
})