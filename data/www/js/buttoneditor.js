// buttoneditor.js

import { BASE_URL } from './config.js';

const endPoint = BASE_URL;
const token = "test";

let items = [];
let editingItemId = null;
let categories = [];
let previousCategoryId = null;

const fetchCategories = async () => {
  try {
    const response = await fetch(`${endPoint}/categories`, {
      headers: { Authorization: `Bearer ${token}` }
    });
    if (response.ok) {
      const data = await response.json();
      categories = data.categories || [];
      return categories;
    } else {
      throw new Error("Failed to fetch categories");
    }
  } catch (err) {
    console.error("Error fetching categories:", err);
    showModalError("Failed to load categories. Please try again.");
    return [];
  }
};

const fetchButtons = async () => {
  try {
    const response = await fetch(`${endPoint}/buttons`);
    if (response.ok) {
      const data = await response.json();
      items = data.buttons || [];
      renderList();
    } else {
      throw new Error("Failed to fetch buttons");
    }
  } catch (err) {
    console.error("Error fetching buttons:", err);
    showModalError("Failed to load buttons. Please try again.");
  }
};

function getCategoryNameById(id) {
  const category = categories.find(cat => cat.id === id);
  return category ? category.name : "Unknown";
}

function populateCategoryFilter(categoryFilter, buttons) {
  categoryFilter.innerHTML = '<option value="all">All</option>';
  const categorySet = new Set(buttons.map(button => button.categoryId));
  categorySet.forEach(categoryId => {
    const categoryName = getCategoryNameById(categoryId);
    const option = document.createElement("option");
    option.value = categoryId;
    option.textContent = categoryName;
    categoryFilter.appendChild(option);
  });

  categoryFilter.addEventListener("change", () => {
    const selectedCategoryId = categoryFilter.value;
    const filteredButtons = selectedCategoryId === "all" ? items : items.filter(button => button.categoryId === parseInt(selectedCategoryId));
    renderList(filteredButtons);
  });
}

function populateEditCategory(editCategory, newCategoryBox) {
  editCategory.innerHTML = '<option value="" disabled selected>Select category</option>'; // Placeholder for dropdown
  categories.forEach(cat => {
    const option = document.createElement("option");
    option.value = cat.id;
    option.textContent = cat.name;
    editCategory.appendChild(option);
  });

  const divider = document.createElement("option");
  divider.disabled = true;
  divider.textContent = "────────────";
  editCategory.appendChild(divider);

  const addNew = document.createElement("option");
  addNew.value = "add-new";
  addNew.textContent = "➕ Add new category…";
  editCategory.appendChild(addNew);

  editCategory.addEventListener("change", () => {
    if (editCategory.value === "add-new") {
      previousCategoryId = editCategory.value;
      newCategoryBox.style.display = "block";
    } else {
      previousCategoryId = editCategory.value;
      newCategoryBox.style.display = "none";
      clearNewCategoryError();
    }
  });
}

function showModalError(message) {
  const modalError = document.getElementById("modalError");
  modalError.textContent = message;
  modalError.style.display = "block";
}

function clearModalError() {
  const modalError = document.getElementById("modalError");
  modalError.textContent = "";
  modalError.style.display = "none";
}

function showNewCategoryError(message) {
  const newCategoryError = document.getElementById("newCategoryError");
  newCategoryError.textContent = message;
  newCategoryError.style.display = "block";
}

function clearNewCategoryError() {
  const newCategoryError = document.getElementById("newCategoryError");
  newCategoryError.textContent = "";
  newCategoryError.style.display = "none";
}

async function saveNewCategory(editCategory, newCategoryBox, newCategoryNameInput) {
  const name = newCategoryNameInput.value.trim();
  if (!name) {
    showNewCategoryError("Enter a category name");
    return;
  }

  const duplicate = categories.some(cat => cat.name.toLowerCase() === name.toLowerCase());
  if (duplicate) {
    showNewCategoryError("A category with this name already exists.");
    return;
  }

  try {
    const res = await fetch(`${endPoint}/categories`, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        "Authorization": `Bearer ${token}`
      },
      body: JSON.stringify({ categories: [{ name }] })
    });

    if (!res.ok) {
      throw new Error("Failed to save category");
    }

    const data = await res.json();
    const newCat = data.categories?.[0];
    if (!newCat) throw new Error("No category returned");

    await fetchCategories();
    populateEditCategory(editCategory, newCategoryBox);
    editCategory.value = newCat.id;

    newCategoryBox.style.display = "none";
    newCategoryNameInput.value = "";
    clearNewCategoryError();
  } catch (err) {
    showNewCategoryError("Failed to save category. Please try again.");
    console.error(err);
  }
}

function cancelNewCategory(editCategory, newCategoryBox, newCategoryNameInput) {
  newCategoryBox.style.display = "none";
  newCategoryNameInput.value = "";
  editCategory.value = previousCategoryId && previousCategoryId !== "add-new" ? previousCategoryId : categories[0]?.id || "";
  clearNewCategoryError();
}

function renderList(filteredItems = items) {
  const itemList = document.getElementById("itemList");
  if (!itemList) return console.error("itemList not found");
  itemList.innerHTML = "";

  filteredItems.forEach((item) => {
    const li = document.createElement("li");
    li.style.cursor = "pointer";
    li.style.padding = "8px";
    li.innerHTML = `
      <span>${item.name}</span>
      <div class="actions" style="display: inline-block; margin-left: 10px;">
        <button class="edit-btn" data-id="${item.id}" aria-label="Edit ${item.name}">Edit</button>
        <button class="delete-btn" data-id="${item.id}" aria-label="Delete ${item.name}">Delete</button>
      </div>
    `;

    li.addEventListener("mouseover", () => li.style.backgroundColor = "#222222");
    li.addEventListener("mouseout", () => li.style.backgroundColor = "#1e1e1e");
    li.addEventListener("click", (e) => {
      if (e.target.tagName !== "BUTTON") openModal(item.id);
    });
    li.setAttribute("tabindex", "0");
    li.addEventListener("keypress", (e) => {
      if (e.key === "Enter") openModal(item.id);
    });

    itemList.appendChild(li);
  });

  document.querySelectorAll(".edit-btn").forEach(btn => {
    btn.addEventListener("click", (e) => {
      e.stopPropagation();
      openModal(parseInt(btn.dataset.id));
    });
  });
  document.querySelectorAll(".delete-btn").forEach(btn => {
    btn.addEventListener("click", (e) => {
      e.stopPropagation();
      deleteItem(parseInt(btn.dataset.id));
    });
  });
}

function closeModal() {
  document.getElementById("editModal").style.display = "none";
  document.getElementById("new-category-box").style.display = "none";
  clearModalError();
  clearNewCategoryError();
}

async function deleteItem(id) {
  //First confirm deletion
  if (!confirm("Are you sure you want to delete this button?")) return;
  try {
    const response = await fetch(`${endPoint}/buttons?id=${encodeURIComponent(id)}`, { method: "DELETE" });
    if (!response.ok) {
      throw new Error("Failed to delete button");
    }
    await fetchButtons();
  } catch (err) {
    console.error("Error deleting button:", err);
    showModalError("Failed to delete button. Please try again.");
  }
}

// Function to toggle fields based on editDeviceAction value
function toggleFields() {
  const editDeviceAction = document.getElementById('editDeviceAction');
  const commandFields = document.getElementById('commandFields');
  const passwordFields = document.getElementById('passwordFields');
  
  const value = editDeviceAction.value;
  if (value === '1') {
    passwordFields.classList.remove('hidden');
    commandFields.classList.add('hidden');
  } else if (value === '2') {
    passwordFields.classList.add('hidden');
    commandFields.classList.remove('hidden');
  }
}

function openModalForAdd() {
  editingItemId = null;
  document.getElementById("editName").value = "";
  document.getElementById("editCategory").value = categories[0]?.id || "";
  previousCategoryId = categories[0]?.id || "";
  document.getElementById("editDeviceAction").value = "1";
  document.getElementById("editPasswordAction").value = "1";
  document.getElementById("editCommand").value = "";
  document.getElementById("editUserName").value = "";
  document.getElementById("editUserPassword").value = "";
  document.getElementById("editNotes").value = "";
  document.getElementById("modalTitle").textContent = "Add New Button";
  document.getElementById("editModal").style.display = "flex";
  document.getElementById("new-category-box").style.display = "none";
  clearModalError();
  clearNewCategoryError();
  toggleFields(); // Set initial visibility
}

function openModal(id) {
  editingItemId = id;
  const item = items.find((i) => i.id === id);
  document.getElementById("editName").value = item.name;
  document.getElementById("editCategory").value = item.categoryId;
  previousCategoryId = item.categoryId;
  document.getElementById("editDeviceAction").value = item.deviceAction;
  document.getElementById("editPasswordAction").value = item.passwordAction;
  document.getElementById("editCommand").value = item.command || "";
  document.getElementById("editUserName").value = item.userName || "";
  document.getElementById("editUserPassword").value = item.userPassword || "";
  document.getElementById("editNotes").value = item.notes || "";
  document.getElementById("modalTitle").textContent = "Edit Button";
  document.getElementById("editModal").style.display = "flex";
  document.getElementById("new-category-box").style.display = "none";
  clearModalError();
  clearNewCategoryError();
  toggleFields(); // Set initial visibility based on loaded value
}

async function saveChanges() {
  const name = document.getElementById("editName").value;
  const categoryId = document.getElementById("editCategory").value;
  const deviceAction = document.getElementById("editDeviceAction").value;
  const passwordAction = document.getElementById("editPasswordAction").value;
  const command = document.getElementById("editCommand").value;
  const userName = document.getElementById("editUserName").value;
  const userPassword = document.getElementById("editUserPassword").value;
  const notes = document.getElementById("editNotes").value;

  if (!name) {
    showModalError("Button label is required.");
    return;
  }
  
  // if (!command) {
  //   showModalError("Command is required.");
  //   return;
  // }

  if (deviceAction === "2" && !command) {
    showModalError("Command is required for Command action.");
    return;
  }
  
  // if (deviceAction === "1" && !userPassword) {
  //   showModalError("Password is required for action.");
  //   return;
  // }
  
  if (categoryId === "add-new") {
    showModalError("Please save or select a valid category.");
    return;
  }
  
  const button = { 
    id: editingItemId || null,
    name,
    categoryId: parseInt(categoryId),
    deviceAction,
    passwordAction,
    command,
    userName,
    userPassword, 
    notes
  };

  try {
    const response = await fetch(`${endPoint}/buttons`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ buttons: [button] }),
    });

    if (!response.ok) {
      throw new Error("Failed to save button");
    }
    
    await fetchButtons();
    
    closeModal();

    } catch (err) {
      showModalError("Failed to save button. Please try again.");
      console.error("Error saving button:", err);
    }
}

async function init() {
  const categoryFilter = document.getElementById("categoryFilter");
  const editCategory = document.getElementById("editCategory");
  const newCategoryBox = document.getElementById("new-category-box");
  const newCategoryNameInput = document.getElementById("new-category-name");
  const saveNewCategoryBtn = document.getElementById("save-new-category");
  const cancelNewCategoryBtn = document.getElementById("cancel-new-category");
  const editDeviceAction = document.getElementById("editDeviceAction");
  const addButton = document.getElementById("addButton"); // New reference

  await fetchCategories();
  populateEditCategory(editCategory, newCategoryBox);
  await fetchButtons();
  populateCategoryFilter(categoryFilter, items);
 
  //This throws errors attaching event listeners
  //document.querySelector("button[onclick='openModalForAdd()']").addEventListener("click", openModalForAdd);
  addButton.addEventListener("click", openModalForAdd); // Updated line

  document.getElementById("saveChanges").addEventListener("click", saveChanges);
  document.getElementById("closeModal").addEventListener("click", closeModal);
  document.getElementById("closeModalFooter").addEventListener("click", closeModal);
  
  saveNewCategoryBtn.addEventListener("click", () => saveNewCategory(editCategory, newCategoryBox, newCategoryNameInput));
  cancelNewCategoryBtn.addEventListener("click", () => cancelNewCategory(editCategory, newCategoryBox, newCategoryNameInput));

  // Add event listener for dynamic toggling
  editDeviceAction.addEventListener('change', toggleFields);
}

export { init };