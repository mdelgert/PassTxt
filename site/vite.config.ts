import { defineConfig } from 'vite';

export default defineConfig(({ mode }) => ({
  base: mode === 'production' ? '/ImprovWiFiWeb/' : '/',
  build: {
    rollupOptions: {
      input: {
        main: 'index.html',
        about: 'about.html',
        blueserial: 'blueserial.html'
      },
    },
  },
}));
