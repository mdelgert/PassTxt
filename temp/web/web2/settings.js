
// Define the base URL for API requests
const BASE_URL = window.location.hostname === "localhost" ? "http://192.168.50.145" : "";

// Function to load settings from /get-json
function loadSettings() {
  const url = `${BASE_URL}/get-json`;

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
    });
}

// Function to save settings to /save-json
function saveSettings() {
  const textarea = document.getElementById("settings-textarea");
  const settings = textarea.value;

  try {
    // Ensure the textarea content is valid JSON
    const parsed = JSON.parse(settings);

    const url = `${BASE_URL}/save-json`;

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
          alert("Settings saved successfully!");
        } else {
          throw new Error(data.message);
        }
      })
      .catch((error) => {
        console.error("Failed to save settings:", error);
        alert("Failed to save settings. Please try again.");
      });
  } catch (error) {
    console.error("Invalid JSON format:", error);
    alert("Invalid JSON. Please ensure your input is correctly formatted.");
  }
}

// Event listeners
document.addEventListener("DOMContentLoaded", () => {
  loadSettings();

  const submitButton = document.getElementById("submit-button");
  submitButton.addEventListener("click", saveSettings);
});
