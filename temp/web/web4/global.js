console.log('global.js loaded');

// Dev logic: if host is "localhost", use the given IP as base, else use relative path
const BASE_URL = window.location.hostname === "localhost" 
  ? "http://demo.local" 
  : "";                      

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

// Export variables and functions for use in other scripts
export { BASE_URL, showMessage };
