import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react';
export default defineConfig({ plugins: [react()], base: './', build: { outDir: '../firmware/main-controller/data', emptyOutDir: true } });
