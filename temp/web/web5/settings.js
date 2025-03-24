console.log('settings.js loaded');

import { httpGet, httpPost, showMessage } from './global.js';

// Function to load settings from /settings/get
async function loadSettings() {
  try {
    const data = await httpGet('/settings/get');
    // Populate the textarea with the JSON data
    const textarea = document.getElementById("settings-textarea");
    textarea.value = JSON.stringify(data, null, 2); // Format JSON nicely
    console.log("Settings loaded successfully!");
  } catch (error) {
    console.error("Failed to load settings:", error);
    showMessage("Failed to load settings.", "error");
  }
}

// Function to save settings to /settings/save
async function saveSettings() {
  const textarea = document.getElementById("settings-textarea");
  const settings = textarea.value;

  try {
    // Ensure the textarea content is valid JSON
    const parsed = JSON.parse(settings);

    const data = await httpPost('/settings/save', parsed);
    if (data.status === "success") {
      console.log("Settings saved successfully!");
      showMessage("Settings saved successfully!", "success");
    } else {
      throw new Error(data.message);
    }
  } catch (error) {
    console.error("Failed to save settings:", error);
    showMessage(error.message || "Failed to save settings.", "error");
  }
}

// Event listeners
document.addEventListener("DOMContentLoaded", () => {
  loadSettings();

  const submitButton = document.getElementById("submit-button");
  submitButton.addEventListener("click", saveSettings);
});
