import { defineConfig } from 'vite';
import { viteStaticCopy } from 'vite-plugin-static-copy';
import fs from 'fs';
import path from 'path';

export default defineConfig(({ mode }) => {
  const isGitHubPages = process.env.GITHUB_REPOSITORY;
  const basePath = isGitHubPages ? `/${process.env.GITHUB_REPOSITORY.split('/')[1]}/` : '/';

  const firmwarePath = path.resolve(__dirname, 'firmware');
  const firmwareExists = fs.existsSync(firmwarePath);

  const plugins = [];

  if (firmwareExists) {
    plugins.push(
      viteStaticCopy({
        targets: [
          {
            src: 'firmware/**/*',
            dest: 'firmware',
          },
        ],
      })
    );
  } else {
    console.warn('Firmware folder not found. Skipping static copy.');
  }

  return {
    root: 'public',
    build: {
      outDir: '../dist',
      emptyOutDir: true,
    },
    base: basePath,
    plugins,
  };
});
