console.log("global.js loaded");

// Centralized device configuration
const DEV_DEVICE = "demo1.local"; // Change this to the desired development device, e.g., "192.168.4.1"

// Determine Base URL and WebSocket URL based on environment
const BASE_URL = window.location.hostname === "localhost" 
    ? `http://${DEV_DEVICE}` 
    : "";

const WS_URL = `ws://${window.location.hostname === "localhost" 
    ? DEV_DEVICE 
    : window.location.hostname}:8232`;

console.log(`BASE_URL: ${BASE_URL}`);
console.log(`WS_URL: ${WS_URL}`);

// Centralized function for HTTP GET requests
async function httpGet(endpoint) {
  const url = `${BASE_URL}${endpoint}`;
  try {
    const response = await fetch(url, {
      method: "GET",
      headers: {
        "Content-Type": "application/json",
      },
    });

    if (!response.ok) {
      throw new Error(`HTTP GET Error: ${response.status} ${response.statusText}`);
    }

    return await response.json();
  } catch (error) {
    console.error(`Error in GET ${url}:`, error);
    throw error; // Re-throw the error for the caller to handle
  }
}

// Centralized function for HTTP POST requests
async function httpPost(endpoint, body) {
  const url = `${BASE_URL}${endpoint}`;
  try {
    const response = await fetch(url, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(body),
    });

    if (!response.ok) {
      throw new Error(`HTTP POST Error: ${response.status} ${response.statusText}`);
    }

    return await response.json();
  } catch (error) {
    console.error(`Error in POST ${url}:`, error);
    throw error; // Re-throw the error for the caller to handle
  }
}

// Show a message in the custom message box
function showMessage(message, type) {
  const msgBox = document.getElementById("message-box");

  if (msgBox) {
    msgBox.className = "message-box"; // Reset classes
    msgBox.classList.add(type); // Add the appropriate type (e.g., "success", "error")
    msgBox.textContent = message;
    msgBox.classList.remove("hidden");

    setTimeout(() => {
      msgBox.classList.add("hidden");
    }, 500); // Hide after 500ms
  }
}

// Load Navbar and Footer dynamically
document.addEventListener("DOMContentLoaded", () => {
  // Load Navbar
  const navbarContainer = document.getElementById("navbar-container");
  if (navbarContainer) {
    fetch("/navbar.html")
      .then((response) => response.text())
      .then((data) => {
        navbarContainer.innerHTML = data;

        // Highlight the active page
        const currentPath = window.location.pathname;
        document.querySelectorAll(".nav-link").forEach((link) => {
          if (link.getAttribute("href") === currentPath) {
            link.setAttribute("aria-current", "page");
          } else {
            link.removeAttribute("aria-current");
          }
        });
      })
      .catch((error) => console.error("Error loading navbar:", error));
  }

  // Load Footer
  const footerContainer = document.getElementById("footer-container");
  if (footerContainer) {
    fetch("/footer.html")
      .then((response) => response.text())
      .then((data) => {
        footerContainer.innerHTML = data;
      })
      .catch((error) => console.error("Error loading footer:", error));
  }
});

// Export variables and functions for reuse
export { WS_URL, BASE_URL, httpGet, httpPost, showMessage };
