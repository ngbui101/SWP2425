/// <reference types="vite/client" />

interface ImportMetaEnv {
    readonly VITE_API_URI: string;
    // Add other environment variables here...
  }
  
  interface ImportMeta {
    readonly env: ImportMetaEnv;
  }
  