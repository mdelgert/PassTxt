// buttons.js
console.log('buttons.js loaded');

import { BASE_URL, httpGet, showMessage } from './global.js';

const endPoint = BASE_URL;

//const endPoint = window.location.hostname === "localhost" ? "http://demo1.local" : "";

    // Fetch and Render Buttons
    async function loadButtons() {
      try {
        const response = await fetch(`${endPoint}/file?filename=/buttons.json`);
        if (!response.ok) throw new Error("Failed to fetch buttons");
        const data = await response.json();
        renderButtons(data.buttons);
      } catch (error) {
        console.error(error);
        //document.getElementById("buttonContainer").innerHTML = "<p>Error loading buttons.</p>";
        showMessage("Error loading buttons", "error");
      }
    }

    // Render Buttons Dynamically
    function renderButtons(buttons) {
      const container = document.getElementById("buttonContainer");
      container.innerHTML = ""; // Clear previous content

      buttons.forEach(button => {
        const btn = document.createElement("button");
        btn.className = "button tooltip";
        btn.textContent = button.label;

        // Apply inline styles
        btn.style.color = button.style.color;
        btn.style.fontSize = button.style.fontSize;
        btn.style.backgroundColor = button.style.backgroundColor;
        btn.style.border = `${button.style.borderWidth} solid ${button.style.borderColor}`;
        btn.style.borderRadius = button.style.borderRadius;
        btn.style.padding = button.style.padding;
        btn.style.width = button.style.width;

        // Add tooltip if available
        if (button.tooltip) {
          btn.setAttribute("data-tooltip", button.tooltip);
        }

        // Attach click event to send the command
        btn.onclick = () => sendCommand(button.command);

        container.appendChild(btn);
      });
    }

    // Send Command to Server
    async function sendCommand(command) {
      try {
        const response = await fetch(`${endPoint}/command/set`, {
          method: "POST",
          headers: {
            "Content-Type": "text/plain",
          },
          body: command,
        });
        if (response.ok) {
          //alert(`Command sent: ${command}`);
          showMessage("Command sent successfully", "success");
        } else {
          //alert("Failed to send command");
          showMessage("Failed to send command", "error");
        }
      } catch (error) {
        console.error("Error sending command:", error);
        //alert("Error sending command");
        showMessage("Error sending command", "error");
      }
    }

    // Load buttons on page load
    loadButtons();