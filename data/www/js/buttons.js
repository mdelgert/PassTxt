//buttons.js

//import "./global.js";
import {BASE_URL} from './config.js';

const endPoint = BASE_URL;

let allButtons = []; // Store all buttons globally

// Fetch and Render Buttons
export async function fetchButtons() {
  try {
    const response = await fetch(`${endPoint}/buttons`);
    if (!response.ok) throw new Error("Failed to fetch buttons");
    const data = await response.json();
    allButtons = data.buttons; // Store data globally
    populateCategoryDropdown(allButtons);
    renderButtons(allButtons);
  } catch (error) {
    console.error("Error fetching buttons:", error);
    const buttonContainer = document.getElementById("buttonContainer");
    if (buttonContainer) {
      buttonContainer.innerHTML = "<p>Error loading buttons.</p>";
    }
  }
}

// Populate Dropdown with Unique Categories
function populateCategoryDropdown(buttons) {
  const categorySet = new Set(buttons.map(button => button.category));
  const categoryFilter = document.getElementById("categoryFilter");

  if (!categoryFilter) return;

  // Clear existing options except "All"
  categoryFilter.innerHTML = '<option value="all">All</option>';

  // Add categories dynamically
  categorySet.forEach(category => {
    const option = document.createElement("option");
    option.value = category;
    option.textContent = category;
    categoryFilter.appendChild(option);
  });

  // Attach event listener for filtering
  categoryFilter.addEventListener("change", () => {
    const selectedCategory = categoryFilter.value;
    const filteredButtons = selectedCategory === "all" ? allButtons : allButtons.filter(button => button.category === selectedCategory);
    renderButtons(filteredButtons);
  });
}

// Render Buttons
function renderButtons(buttons) {
  const container = document.getElementById("buttonContainer");
  if (!container) {
    console.error("Error: #buttonContainer not found in the DOM.");
    return;
  }
  container.innerHTML = ""; // Clear previous content

  buttons.forEach(button => {
    const btn = document.createElement("button");
    btn.className = "button tooltip";
    btn.textContent = button.name;

    // Apply inline styles only if provided
    if (button.style?.color) btn.style.color = button.style.color;
    if (button.style?.fontSize) btn.style.fontSize = button.style.fontSize;
    if (button.style?.backgroundColor) {
      btn.style.setProperty("--dynamic-bg-color", button.style.backgroundColor);
    }
    if (button.style?.borderWidth && button.style?.borderColor) {
      btn.style.border = `${button.style.borderWidth} solid ${button.style.borderColor}`;
    }
    if (button.style?.borderRadius) btn.style.borderRadius = button.style.borderRadius;
    if (button.style?.padding) btn.style.padding = button.style.padding;
    if (button.style?.width) btn.style.width = button.style.width;

    // Attach click event to send the command
    btn.onclick = () => sendCommand(button.command);

    container.appendChild(btn);
  });
}

// Send Command to Server
export async function sendCommand(command) {
  if (navigator.vibrate) {
    navigator.vibrate(50);
  }

  try {
    const response = await fetch(`${endPoint}/command/set`, {
      method: "POST",
      headers: {
        "Content-Type": "text/plain",
      },
      body: command,
    });

    if (response.ok) {
      console.log(`Command sent: ${command}`);
    } else {
      alert("Failed to send command");
    }
  } catch (error) {
    console.error("Error sending command:", error);
    alert("Error sending command");
  }
}

// Ensure `fetchButtons` runs **after the DOM is loaded**
document.addEventListener("DOMContentLoaded", () => {
  fetchButtons();
});
