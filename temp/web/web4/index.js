//index.js
console.log('index.js loaded');

import { BASE_URL, showMessage } from './global.js';

// Function to reboot the device
function rebootDevice() {
  const url = `${BASE_URL}/device/reboot`;

  fetch(url, {
    method: "GET", // Changed to GET as per your requirement
  })
    .then((response) => {
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      console.log("Device reboot initiated successfully!");
      showMessage("Device reboot initiated successfully!", "success");
    })
    .catch((error) => {
      console.error("Failed to reboot the device:", error);
      showMessage("Failed to reboot the device.", "error");
    });
}

// Event listeners
document.addEventListener("DOMContentLoaded", () => {
  const rebootButton = document.getElementById("rebootButton");
  rebootButton.addEventListener("click", rebootDevice);
});
