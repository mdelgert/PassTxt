//buttoneditor.js

import {BASE_URL} from './config.js';

const endPoint = BASE_URL;
const token = "test";

let items = [];
let editingItemId = null;
let categories = [];
let previousCategory = null; // Store the previously selected category

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
      console.error("Failed to fetch categories");
      return [];
    }
  } catch (err) {
    console.error("Error fetching categories:", err);
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
      console.error("Failed to fetch buttons");
    }
  } catch (err) {
    console.error("Error fetching buttons:", err);
  }
};

function populateCategoryFilter(categoryFilter, buttons) {
  categoryFilter.innerHTML = '<option value="all">All</option>';
  const categorySet = new Set(buttons.map(button => button.category));
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

function populateEditCategory(editCategory, newCategoryBox) {
  editCategory.innerHTML = "";
  categories.forEach(cat => {
    const option = document.createElement("option");
    option.value = cat.name;
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
      previousCategory = editCategory.value; // Store the current selection before switching
      newCategoryBox.style.display = "block";
    } else {
      previousCategory = editCategory.value; // Update the previous selection
      newCategoryBox.style.display = "none";
    }
  });
}

async function saveNewCategory(editCategory, newCategoryBox, newCategoryNameInput) {
  const name = newCategoryNameInput.value.trim();
  if (!name) return alert("Enter a category name");

  try {
    const res = await fetch(`${endPoint}/categories`, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        "Authorization": `Bearer ${token}`
      },
      body: JSON.stringify({ categories: [{ name }] })
    });

    const data = await res.json();
    const newCat = data.categories?.[0];
    if (!newCat) throw new Error("No category returned");

    await fetchCategories();
    populateEditCategory(editCategory, newCategoryBox);
    editCategory.value = newCat.name;

    newCategoryBox.style.display = "none";
    newCategoryNameInput.value = "";
  } catch (err) {
    alert("Failed to save category");
    console.error(err);
  }
}

function cancelNewCategory(editCategory, newCategoryBox, newCategoryNameInput) {
  newCategoryBox.style.display = "none";
  newCategoryNameInput.value = "";
  editCategory.value = previousCategory && previousCategory !== "add-new" ? previousCategory : categories[0]?.name || ""; // Reset to previous or first category
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
  document.getElementById("editCategory").value = item.category;
  previousCategory = item.category; // Store the initial category
  document.getElementById("editAction").value = item.action;
  document.getElementById("editCommand").value = item.command || "";
  document.getElementById("editUserName").value = item.userName || "";
  document.getElementById("editPassword").value = item.password || "";
  document.getElementById("editNotes").value = item.notes || "";
  document.getElementById("modalTitle").textContent = "Edit Button";
  document.getElementById("editModal").style.display = "flex";
  document.getElementById("new-category-box").style.display = "none";
}

function openModalForAdd() {
  editingItemId = null;
  document.getElementById("editName").value = "";
  document.getElementById("editCategory").value = categories[0]?.name || "";
  previousCategory = categories[0]?.name || ""; // Store the initial category
  document.getElementById("editAction").value = "Command";
  document.getElementById("editCommand").value = "";
  document.getElementById("editUserName").value = "";
  document.getElementById("editPassword").value = "";
  document.getElementById("editNotes").value = "";
  document.getElementById("modalTitle").textContent = "Add New Button";
  document.getElementById("editModal").style.display = "flex";
  document.getElementById("new-category-box").style.display = "none";
}

function closeModal() {
  document.getElementById("editModal").style.display = "none";
  document.getElementById("new-category-box").style.display = "none";
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
  if (category === "add-new") return alert("Please save or select a valid category");
  
  const button = { 
    //id: editingItemId || Date.now(),
    id: editingItemId || null,
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
  //if (!confirm("Are you sure you want to delete this item?")) return;
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