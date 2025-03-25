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

// Helper function to get category name by ID
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
  editCategory.innerHTML = "";
  categories.forEach(cat => {
    const option = document.createElement("option");
    option.value = cat.id; // Use ID as the value
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
      clearNewCategoryError(); // Clear error when changing selection
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

  // Check for duplicate category name (case-insensitive)
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
        <button class="edit-btn" data-id="${item.id}">Edit</button>
        <button class="delete-btn" data-id="${item.id}">Delete</button>
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

function openModal(id) {
  editingItemId = id;
  const item = items.find((i) => i.id === id);
  document.getElementById("editName").value = item.name;
  document.getElementById("editCategory").value = item.categoryId;
  previousCategoryId = item.categoryId;
  document.getElementById("editAction").value = item.action;
  document.getElementById("editCommand").value = item.command || "";
  document.getElementById("editUserName").value = item.userName || "";
  document.getElementById("editPassword").value = item.password || "";
  document.getElementById("editNotes").value = item.notes || "";
  document.getElementById("modalTitle").textContent = "Edit Button";
  document.getElementById("editModal").style.display = "flex";
  document.getElementById("new-category-box").style.display = "none";
  clearModalError();
  clearNewCategoryError();
}

function openModalForAdd() {
  editingItemId = null;
  document.getElementById("editName").value = "";
  document.getElementById("editCategory").value = categories[0]?.id || "";
  previousCategoryId = categories[0]?.id || "";
  document.getElementById("editAction").value = "Command";
  document.getElementById("editCommand").value = "";
  document.getElementById("editUserName").value = "";
  document.getElementById("editPassword").value = "";
  document.getElementById("editNotes").value = "";
  document.getElementById("modalTitle").textContent = "Add New Button";
  document.getElementById("editModal").style.display = "flex";
  document.getElementById("new-category-box").style.display = "none";
  clearModalError();
  clearNewCategoryError();
}

function closeModal() {
  document.getElementById("editModal").style.display = "none";
  document.getElementById("new-category-box").style.display = "none";
  clearModalError();
  clearNewCategoryError();
}

async function saveChanges() {
  const name = document.getElementById("editName").value;
  const categoryId = document.getElementById("editCategory").value;
  const action = document.getElementById("editAction").value;
  const command = document.getElementById("editCommand").value;
  const userName = document.getElementById("editUserName").value;
  const password = document.getElementById("editPassword").value;
  const notes = document.getElementById("editNotes").value;

  if (!name) {
    showModalError("Button label is required.");
    return;
  }
  if (!command) {
    showModalError("Command is required.");
    return;
  }
  if (categoryId === "add-new") {
    showModalError("Please save or select a valid category.");
    return;
  }

  const button = { 
    id: editingItemId || null,
    name,
    categoryId: parseInt(categoryId),
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

async function deleteItem(id) {
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

async function init() {
  const categoryFilter = document.getElementById("categoryFilter");
  const editCategory = document.getElementById("editCategory");
  const newCategoryBox = document.getElementById("new-category-box");
  const newCategoryNameInput = document.getElementById("new-category-name");
  const saveNewCategoryBtn = document.getElementById("save-new-category");
  const cancelNewCategoryBtn = document.getElementById("cancel-new-category");

  await fetchCategories();
  populateEditCategory(editCategory, newCategoryBox);
  await fetchButtons();
  populateCategoryFilter(categoryFilter, items);

  document.querySelector("button[onclick='openModalForAdd()']").addEventListener("click", openModalForAdd);
  document.getElementById("saveChanges").addEventListener("click", saveChanges);
  document.getElementById("closeModal").addEventListener("click", closeModal);
  document.getElementById("closeModalFooter").addEventListener("click", closeModal);
  saveNewCategoryBtn.addEventListener("click", () => saveNewCategory(editCategory, newCategoryBox, newCategoryNameInput));
  cancelNewCategoryBtn.addEventListener("click", () => cancelNewCategory(editCategory, newCategoryBox, newCategoryNameInput));
}

export { init };