//buttoneditor.js

import {BASE_URL} from './config.js';

//const endPoint = window.location.hostname === "localhost" ? "http://demo1.local" : "";
const endPoint = BASE_URL;

let items = [];
let editingItemId = null;

const fetchButtons = async () => {
  try {
    const response = await fetch(`${endPoint}/buttons`);
    if (response.ok) {
      const data = await response.json();
      items = data.buttons || [];
      populateCategoryFilter(items);
      renderList();
    } else {
      console.error("Failed to fetch buttons");
    }
  } catch (err) {
    console.error("Error fetching buttons:", err);
  }
};

// Populate the category filter dropdown
function populateCategoryFilter(buttons) {
  const categorySet = new Set(buttons.map(button => button.category));
  const categoryFilter = document.getElementById("categoryFilter");

  categoryFilter.innerHTML = '<option value="all">All</option>';

  categorySet.forEach(category => {
    const option = document.createElement("option");
    option.value = category;
    option.textContent = category;
    categoryFilter.appendChild(option);
  });

  categoryFilter.addEventListener("change", () => {
    const selectedCategory = categoryFilter.value;
    const filteredButtons = selectedCategory === "all" ? items : items.filter(button => button.category === selectedCategory);
    renderList(filteredButtons);
  });
}

// Update renderList function to accept filtered items
function renderList(filteredItems = items) {
  const itemList = document.getElementById("itemList");
  if (!itemList) return console.error("itemList not found");
  itemList.innerHTML = "";

  filteredItems.forEach((item) => {
    const li = document.createElement("li");
    li.style.cursor = "pointer"; // Hand cursor
    li.style.padding = "8px"; // Optional: Basic padding for spacing
    //li.style.backgroundColor = "#fff"; // Optional: White background
    li.innerHTML = `
      <span>${item.name}</span>
      <div class="actions" style="display: inline-block; margin-left: 10px;">
        <button onclick="openModal(${item.id}); event.stopPropagation();">Edit</button>
        <button onclick="deleteItem(${item.id}); event.stopPropagation();">Delete</button>
      </div>
    `;

    // Hover effect for hand cursor and visual feedback
    li.addEventListener("mouseover", () => {
      li.style.backgroundColor = "#222222";
    });

    li.addEventListener("mouseout", () => {
      li.style.backgroundColor = "#1e1e1e";
    });

    // Make entire li clickable to open modal, except buttons
    li.addEventListener("click", (e) => {
      if (e.target.tagName !== "BUTTON") {
        openModal(item.id);
      }
    });

    // Accessibility: Make li focusable and keyboard-accessible
    li.setAttribute("tabindex", "0");
    li.addEventListener("keypress", (e) => {
      if (e.key === "Enter") {
        openModal(item.id);
      }
    });
    
    itemList.appendChild(li);
  });
}

function renderListOriginal(filteredItems = items) {
  const itemList = document.getElementById("itemList");
  itemList.innerHTML = "";

  filteredItems.forEach((item) => {
    const li = document.createElement("li");
    li.innerHTML = `
      <span>${item.name}</span>
      <div class="actions">
        <button onclick="sendCommand('${encodeURIComponent(item.command)}')">Run</button>
        <button onclick="openModal(${item.id})">Edit</button>
        <button onclick="deleteItem(${item.id})">Delete</button>
      </div>`;
      
    itemList.appendChild(li);
  });
}

function openModal(id) {
  editingItemId = id;
  const item = items.find((i) => i.id === id);
  document.getElementById("editName").value = item.name;
  document.getElementById("editCategory").value = item.category;
  document.getElementById("editAction").value = item.action;
  document.getElementById("editCommand").value = item.command || "";
  document.getElementById("editUserName").value = item.userName || "";
  document.getElementById("editPassword").value = item.password || "";
  document.getElementById("editNotes").value = item.notes || "";
  document.getElementById("modalTitle").textContent = "Edit Button";
  document.getElementById("editModal").style.display = "flex";
}

function openModalForAdd() {
  editingItemId = null;
  document.getElementById("editName").value = "";
  document.getElementById("editCategory").value = "General";
  document.getElementById("editAction").value = "Command";
  document.getElementById("editCommand").value = "";
  document.getElementById("editUserName").value = "";
  document.getElementById("editPassword").value = "";
  document.getElementById("editNotes").value = "";
  document.getElementById("modalTitle").textContent = "Add New Button";
  document.getElementById("editModal").style.display = "flex";
}

function closeModal() {
  document.getElementById("editModal").style.display = "none";
}

async function saveChanges() {
  const name = document.getElementById("editName").value;
  const category = document.getElementById("editCategory").value;
  const action = document.getElementById("editAction").value;
  const command = document.getElementById("editCommand").value;
  const userName = document.getElementById("editUserName").value;
  const password = document.getElementById("editPassword").value;
  const notes = document.getElementById("editNotes").value;
  if (!name || !command) return;

  const button = { 
    id: editingItemId || Date.now(), 
    name,
    category, 
    action,
    command,
    userName,
    password, 
    notes
  };

  try {
    const response = await fetch(`${endPoint}/buttons`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ buttons: [button] }),
    });
    
    if (response.ok) {
      await fetchButtons();
      closeModal();
    } else {
      console.error("Failed to save button");
    }
  } catch (err) {
    console.error("Error saving button:", err);
  }
}

async function deleteItem(id) {
  if (!confirm("Are you sure you want to delete this item?")) return;
  try {
    const response = await fetch(`${endPoint}/buttons?id=${encodeURIComponent(id)}`, { method: "DELETE" });
    if (response.ok) {
      await fetchButtons();
    } else {
      console.error("Failed to delete button");
    }
  } catch (err) {
    console.error("Error deleting button:", err);
  }
}

async function deleteItemOriginal(id) {
  try {
    const response = await fetch(`${endPoint}/buttons?id=${encodeURIComponent(id)}`, { method: "DELETE" });
    if (response.ok) {
      await fetchButtons();
    } else {
      console.error("Failed to delete button");
    }
  } catch (err) {
    console.error("Error deleting button:", err);
  }
}

async function sendCommand(command) {
  const decodedCommand = decodeURIComponent(command);

  if (navigator.vibrate) {
    navigator.vibrate(50);
  }

  try {
    const response = await fetch(`${endPoint}/command/set`, {
      method: "POST",
      headers: { "Content-Type": "text/plain" },
      body: decodedCommand,
    });

    if (response.ok) {
      console.log(`Command sent successfully: ${decodedCommand}`);
    } else {
      console.error(`Failed to send command. Status: ${response.status}`);
      alert("Failed to send command");
    }
  } catch (error) {
    console.error("Error sending command:", error);
    alert("Error sending command");
  }
}

// Expose functions globally so HTML `onclick` attributes work
window.openModalForAdd = openModalForAdd;
window.openModal = openModal;
window.closeModal = closeModal;
window.saveChanges = saveChanges;
window.deleteItem = deleteItem;
window.sendCommand = sendCommand;

// Initialize buttons on page load
fetchButtons();
