console.log("settings.js loaded");

import { httpGet, httpPost, showMessage } from "./global.js";

// Load settings from the server
async function loadSettings() {
  try {
    console.log("Fetching settings from /settings/get...");
    const response = await httpGet("/settings/get");
    const { data } = response; // Extract the data object from the response
    console.log("Received settings:", data);

    // Populate form fields
    document.getElementById("device_name").value = data.device_name || "";
    document.getElementById("wifi_network").value = data.wifi_ssid || "";
    document.getElementById("wifi_password").value = data.wifi_password || "";
    document.getElementById("mqtt_server").value = data.mqtt_server || "";
    document.getElementById("mqtt_port").value = data.mqtt_port || "";
    document.getElementById("mqtt_username").value = data.mqtt_username || "";
    document.getElementById("mqtt_password").value = data.mqtt_password || "";
    //document.getElementById("api_key").value = data.api_key || "";

    //showMessage("Settings loaded successfully!", "success");
  } catch (error) {
    showMessage("Failed to load settings.", "error");
    console.error("Error loading settings:", error);
  }
}

// Save settings to the server
async function saveSettings() {
  const deviceName = document.getElementById("device_name").value.trim();
  const wifiSsid = document.getElementById("wifi_network").value.trim();
  const wifiPassword = document.getElementById("wifi_password").value.trim();
  const mqttServer = document.getElementById("mqtt_server").value.trim();
  const mqttPort = document.getElementById("mqtt_port").value.trim();
  const mqttUsername = document.getElementById("mqtt_username").value.trim();
  const mqttPassword = document.getElementById("mqtt_password").value.trim();
  //const apiKey = document.getElementById("api_key").value.trim();

  // Validate mandatory fields (example: Wi-Fi credentials)
  if (!deviceName || !wifiSsid || !wifiPassword) {
    showMessage("Please fill in all mandatory fields.", "error");
    console.error("Validation failed: Missing required fields.");
    return;
  }

  try {
    console.log("Saving settings to /settings/set...");
    const body = {
      device_name: deviceName,
      wifi_ssid: wifiSsid,
      wifi_password: wifiPassword,
      mqtt_server: mqttServer,
      mqtt_port: mqttPort,
      mqtt_username: mqttUsername,
      mqtt_password: mqttPassword,
      //api_key: apiKey,
    };
    console.log("Payload being sent:", body);

    await httpPost("/settings/set", body);
    showMessage("Settings saved successfully!", "success");
  } catch (error) {
    showMessage("Failed to save settings.", "error");
    console.error("Error saving settings:", error);
  }
}

// Attach event listeners
document.addEventListener("DOMContentLoaded", () => {
  // Load existing settings on page load
  console.log("Page loaded. Initializing settings load...");
  loadSettings();

  // Attach save button event
  const saveButton = document.querySelector(".settings-form button");
  if (saveButton) {
    saveButton.addEventListener("click", saveSettings);
    console.log("Save button event listener attached.");
  } else {
    console.error("Save button not found.");
  }
});
