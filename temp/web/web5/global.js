console.log('global.js loaded');

// Dev logic: if host is "localhost", use the given IP as base, else use relative path
const BASE_URL = window.location.hostname === "localhost" ? "http://demo.local" : "";

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

// Show message in the custom box
function showMessage(message, type) {
  const msgBox = document.getElementById("message-box");

  msgBox.className = "message-box";
  msgBox.classList.add(type);
  msgBox.textContent = message;
  msgBox.classList.remove("hidden");

  setTimeout(() => {
    msgBox.classList.add("hidden");
  }, 3000);
}

// Export variables and functions for use in other scripts
export { BASE_URL, httpGet, httpPost, showMessage };