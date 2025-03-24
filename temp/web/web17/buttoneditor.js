// buttoneditor.js
console.log('buttoneditor.js loaded');

const endPoint = window.location.hostname === "localhost" ? "http://demo1.local" : "";

let items = [];
let editingItemId = null;

const fetchButtons = async () => {
    try {
        const response = await fetch(`${endPoint}/buttons`, { method: "GET" });
        if (response.ok) {
            const data = await response.json();
            items = data.buttons || [];
            renderList();
        } else {
            console.error("Failed to fetch buttons");
        }
    } catch (err) {
        console.error("Error fetching buttons:", err);
    }
};

const renderList = () => {
    const itemList = document.getElementById("itemList");
    itemList.innerHTML = "";
    items.forEach((item) => {
        const li = document.createElement("li");
        li.innerHTML = `
      <span>${item.name}</span>
      <div class="actions">
        <button onclick="openModal(${item.id})">Edit</button>
        <button onclick="deleteItem(${item.id})">Delete</button>
      </div>
    `;
        itemList.appendChild(li);
    });
};

const openModal = (id) => {
    editingItemId = id;
    const item = items.find((i) => i.id === id);
    document.getElementById("editName").value = item.name;
    document.getElementById("editCategory").value = item.category;
    document.getElementById("editAction").value = item.action;
    document.getElementById("editCommand").value = item.command;
    document.getElementById("editNotes").value = item.notes || "";
    document.getElementById("modalTitle").textContent = "Edit Button";
    document.getElementById("editModal").style.display = "flex";
};

const openModalForAdd = () => {
    editingItemId = null;
    document.getElementById("editName").value = "";
    document.getElementById("editCategory").value = "General";
    document.getElementById("editAction").value = "Type";
    document.getElementById("editCommand").value = "";
    document.getElementById("editNotes").value = "";
    document.getElementById("modalTitle").textContent = "Add New Button";
    document.getElementById("editModal").style.display = "flex";
};

const closeModal = () => {
    document.getElementById("editModal").style.display = "none";
};

const saveChanges = async () => {
    const name = document.getElementById("editName").value;
    const category = document.getElementById("editCategory").value;
    const action = document.getElementById("editAction").value;
    const command = document.getElementById("editCommand").value;
    const notes = document.getElementById("editNotes").value;
    if (!name || !command) return;

    const button = { id: editingItemId || Date.now(), name, category, action, command, notes };

    try {
        const response = await fetch(`${endPoint}/buttons`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ buttons: [button] }),
        });

        if (response.ok) {
            await fetchButtons(); // Refresh the list
            closeModal();
        } else {
            console.error("Failed to save button");
        }
    } catch (err) {
        console.error("Error saving button:", err);
    }
};

/*
const deleteItem = async (id) => {
  try {
    const response = await fetch(`${endPoint}/buttons?id=${id}`, { method: "DELETE" });
    if (response.ok) {
      await fetchButtons(); // Refresh the list
    } else {
      console.error("Failed to delete button");
    }
  } catch (err) {
    console.error("Error deleting button:", err);
  }
};
*/

const deleteItem = async (id) => {
    try {
        const response = await fetch(`${endPoint}/buttons?id=${encodeURIComponent(id)}`, { method: "DELETE" });
        if (response.ok) {
            await fetchButtons(); // Refresh the list
        } else {
            console.error("Failed to delete button");
        }
    } catch (err) {
        console.error("Error deleting button:", err);
    }
};

fetchButtons();