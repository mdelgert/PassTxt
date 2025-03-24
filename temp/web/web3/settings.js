console.log('Settings.js loaded');

// Define the base URL for API requests
const BASE_URL = window.location.hostname === "localhost" ? "http://192.168.50.145" : "";

// Function to load settings from /get-json
function loadSettings() {
  const url = `${BASE_URL}/settings/get`;

  fetch(url)
    .then((response) => {
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      return response.json();
    })
    .then((data) => {
      // Populate the textarea with the JSON data
      const textarea = document.getElementById("settings-textarea");
      textarea.value = JSON.stringify(data, null, 2); // Format JSON nicely
    })
    .catch((error) => {
      console.error("Failed to load data from /get-json:", error);
      showMessage("", "error");
    });
}

// Function to save settings to /save-json
function saveSettings() {
  const textarea = document.getElementById("settings-textarea");
  const settings = textarea.value;

  try {
    // Ensure the textarea content is valid JSON
    const parsed = JSON.parse(settings);

    const url = `${BASE_URL}/settings/save`;

    fetch(url, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(parsed),
    })
      .then((response) => {
        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }
        return response.json();
      })
      .then((data) => {
        if (data.status === "success") {
          console.log("Settings saved successfully!");
          //alert("Settings saved successfully!");
          showMessage("Settings saved successfully!", "success");
        } else {
          throw new Error(data.message);
        }
      })
      .catch((error) => {
        console.error("Failed to save settings:", error);
        //alert("Failed to save settings. Please try again.");
        showMessage("Failed to save settings:", "error");
      });
  } catch (error) {
    console.error("Invalid JSON format:", error);
    //alert("Invalid JSON. Please ensure your input is correctly formatted.");
    showMessage("Invalid JSON format:", "error");
  }
}

// Event listeners
document.addEventListener("DOMContentLoaded", () => {
  loadSettings();

  const submitButton = document.getElementById("submit-button");
  submitButton.addEventListener("click", saveSettings);
});

// Show message in the custom box
function showMessage(message, type) {
  // Get the message box element
  const msgBox = document.getElementById("message-box");

  // Clear any existing classes (success / error), then add the appropriate one
  msgBox.className = "message-box";
  msgBox.classList.add(type);

  // Set the text content
  msgBox.textContent = message;

  // Make sure it's visible
  msgBox.classList.remove("hidden");

  // Optional: Hide after a certain delay (e.g., 3 seconds)
  setTimeout(() => {
    msgBox.classList.add("hidden");
  }, 3000);
}