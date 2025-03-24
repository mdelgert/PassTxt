console.log('index.js loaded');

import { BASE_URL, httpGet, showMessage } from './global.js';

// Function to reboot the device
async function rebootDevice() {
  try {
    await httpGet('/device/reboot'); // Call the centralized GET function
    console.log("Device reboot initiated successfully!");
    showMessage("Device reboot initiated successfully!", "success");
  } catch (error) {
    console.error("Failed to reboot the device:", error);
    showMessage("Failed to reboot the device.", "error");
  }
}

// Event listeners
document.addEventListener("DOMContentLoaded", () => {
  const rebootButton = document.getElementById("rebootButton");
  rebootButton.addEventListener("click", rebootDevice);
});
