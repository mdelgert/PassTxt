// actions.js
console.log('actions.js loaded');

import { httpGet, httpPost, showMessage } from './global.js';

// Function to load actions from /actions/get
async function loadActions() {
  try {
    const response = await httpGet('/actions/get'); // Fetch the actions data
    const { data } = response; // Extract the 'data' field from the response

    if (!data || typeof data !== "object") {
      throw new Error("Invalid data format in response");
    }

    // Populate the textarea with the JSON data
    const textarea = document.getElementById("actions-textarea");
    textarea.value = JSON.stringify(data, null, 2); // Format JSON nicely
    console.log("Actions loaded successfully!");
    //showMessage("Actions loaded successfully!", "success");
  } catch (error) {
    console.error("Failed to load actions:", error);
    showMessage("Failed to load actions.", "error");
  }
}

// Function to save actions to /actions/save
async function saveActions() {
  const textarea = document.getElementById("actions-textarea");
  const actions = textarea.value;

  try {
    // Ensure the textarea content is valid JSON
    const parsed = JSON.parse(actions);

    const data = await httpPost('/actions/set', parsed);
    if (data.status === "success") {
      console.log("Actions saved successfully!");
      showMessage("Actions saved successfully!", "success");
    } else {
      throw new Error(data.message);
    }
  } catch (error) {
    console.error("Failed to save actions:", error);
    showMessage(error.message || "Failed to save actions.", "error");
  }
}

// Event listeners
document.addEventListener("DOMContentLoaded", () => {
  loadActions();

  const submitButton = document.getElementById("submit-button");
  submitButton.addEventListener("click", saveActions);
});
