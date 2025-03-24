// files.js
const endPoint = window.location.hostname === "localhost" ? `http://demo1.local` : "";
let currentPath = "/";
let currentFile = "";

function showNotification(message, type = "info") {
    const statusMessage = document.getElementById("statusMessage");
    statusMessage.textContent = message;
    statusMessage.style.color = type === "error" ? "#ff5555" : "#ccc";
}

function breadcrumbPath(path) {
    const breadcrumbs = document.getElementById("breadcrumbs");
    breadcrumbs.innerHTML = path;
}

async function refreshFiles() {
    try {
        const response = await fetch(`${endPoint}/filemanager?path=${encodeURIComponent(currentPath)}`);
        const data = await response.json();

        if (data.status !== "success") {
            showNotification(`Error fetching directory contents: ${data.message}`, "error");
            return;
        }

        const fileList = document.getElementById("fileList");
        fileList.innerHTML = "";

        //updateBreadcrumbs(currentPath);
        breadcrumbPath(currentPath);

        if (currentPath !== '/') {
            const backButton = document.createElement('li');
            backButton.textContent = '.. (Back)';
            backButton.onclick = goBack;
            fileList.appendChild(backButton);
        }

        data.data.folders.forEach(folder => {
            const li = document.createElement("li");
            li.classList.add("folder");
            li.innerHTML = `
        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="currentColor" class="icon">
            <path d="M19.5 21a3 3 0 0 0 3-3v-4.5a3 3 0 0 0-3-3h-15a3 3 0 0 0-3 3V18a3 3 0 0 0 3 3h15ZM1.5 10.146V6a3 3 0 0 1 3-3h5.379a2.25 2.25 0 0 1 1.59.659l2.122 2.121c.14.141.331.22.53.22H19.5a3 3 0 0 1 3 3v1.146A4.483 4.483 0 0 0 19.5 9h-15a4.483 4.483 0 0 0-3 1.146Z" />
        </svg>
        <span>${folder}</span>`;
            li.onclick = () => openFolder(folder);
            fileList.appendChild(li);
        });

        data.data.files.forEach(file => {
            const li = document.createElement("li");
            li.classList.add("file");
            li.innerHTML = `
        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="currentColor" class="icon">
            <path fill-rule="evenodd" d="M5.625 1.5c-1.036 0-1.875.84-1.875 1.875v17.25c0 1.035.84 1.875 1.875 1.875h12.75c1.035 0 1.875-.84 1.875-1.875V12.75A3.75 3.75 0 0 0 16.5 9h-1.875a1.875 1.875 0 0 1-1.875-1.875V5.25A3.75 3.75 0 0 0 9 1.5H5.625ZM7.5 15a.75.75 0 0 1 .75-.75h7.5a.75.75 0 0 1 0 1.5h-7.5A.75.75 0 0 1 7.5 15Zm.75 2.25a.75.75 0 0 0 0 1.5H12a.75.75 0 0 0 0-1.5H8.25Z" clip-rule="evenodd" />
            <path d="M12.971 1.816A5.23 5.23 0 0 1 14.25 5.25v1.875c0 .207.168.375.375.375H16.5a5.23 5.23 0 0 1 3.434 1.279 9.768 9.768 0 0 0-6.963-6.963Z" />
        </svg>
        <span>${file}</span>`;
            li.onclick = () => openFile(file);
            fileList.appendChild(li);
        });

    } catch (err) {
        showNotification("Error refreshing file list: " + err.message, "error");
    }
}

async function openFile(file) {
    try {
        const response = await fetch(`${endPoint}/file?filename=${encodeURIComponent(currentPath + file)}`);
        const content = await response.text();
        currentFile = file;
        document.getElementById('editor').value = content;
        showNotification(currentFile);
    } catch (err) {
        showNotification("Error opening file: " + err.message, "error");
    }
}

function openFolder(folder) {
    currentPath = `${currentPath}${folder}/`;
    refreshFiles();
}

function goBack() {
    currentPath = currentPath.substring(0, currentPath.lastIndexOf('/', currentPath.length - 2) + 1) || '/';
    refreshFiles();
}

async function saveFile() {
    if (!currentFile) {
        showNotification("No file selected.", "error");
        return;
    }
    const content = document.getElementById('editor').value;
    try {
        const response = await fetch(`${endPoint}/file?filename=${encodeURIComponent(currentPath + currentFile)}`, {
            method: 'POST',
            headers: { 'Content-Type': 'text/plain' },
            body: content
        });
        const result = await response.json();
        if (result.status === "success") {
            showNotification("File saved successfully!");
        } else {
            showNotification("Error saving file: " + result.message, "error");
        }
    } catch (err) {
        showNotification("Error saving file: " + err.message, "error");
    }
}

async function createFile() {
    const fileName = prompt("Enter new file name:");
    if (!fileName) return;
    try {
        const response = await fetch(`${endPoint}/file?filename=${encodeURIComponent(currentPath + fileName)}`, {
            method: 'POST',
            headers: { 'Content-Type': 'text/plain' },
            body: ' ' // Create an empty file
        });
        const result = await response.json();
        if (result.status === "success") {
            refreshFiles();
        } else {
            showNotification("Error creating file: " + result.message, "error");
        }
    } catch (err) {
        showNotification("Error creating file: " + err.message, "error");
    }
}

async function createFolder() {
    const folderName = prompt("Enter new folder name:");
    if (!folderName) return;
    try {
        const response = await fetch(`${endPoint}/folder?foldername=${encodeURIComponent(currentPath + folderName)}`, {
            method: 'POST'
        });
        const result = await response.json();
        if (result.status === "success") {
            refreshFiles();
        } else {
            showNotification("Error creating folder: " + result.message, "error");
        }
    } catch (err) {
        showNotification("Error creating folder: " + err.message, "error");
    }
}

async function deleteItem(isFolder) {
    
    const endpoint = isFolder ? '/folder' : '/file';
    const itemName = isFolder ? currentPath : currentFile;
    const confirmed = window.confirm(`Are you sure you want to delete ${itemName}?`);

    if (!confirmed) {
        return;
    }

    try {
        const response = await fetch(`${endPoint}${endpoint}?filename=${encodeURIComponent(currentPath + itemName)}`, {
            method: 'DELETE'
        });
        const result = await response.json();
        if (result.status === "success") {
            refreshFiles();
        } else {
            showNotification("Error deleting item: " + result.message, "error");
        }
    } catch (err) {
        showNotification("Error deleting item: " + err.message, "error");
    }
}

async function renameItem() {
    const oldName = prompt("Enter current name:");
    if (!oldName) return;
    const newName = prompt("Enter new name:");
    if (!newName) return;
    try {
        const response = await fetch(`${endPoint}/rename`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ oldname: currentPath + oldName, newname: currentPath + newName })
        });
        const result = await response.json();
        if (result.status === "success") {
            refreshFiles();
        } else {
            showNotification("Error renaming item: " + result.message, "error");
        }
    } catch (err) {
        showNotification("Error renaming item: " + err.message, "error");
    }
}

refreshFiles();

/*
function updateBreadcrumbs(path) {
    const breadcrumbs = document.getElementById("breadcrumbs");
    breadcrumbs.innerHTML = "";

    const segments = path.split("/").filter(Boolean);
    let accumulatedPath = "/";
    const root = document.createElement("span");
    root.textContent = "Root";
    root.onclick = () => navigateTo("/");
    breadcrumbs.appendChild(root);

    segments.forEach(segment => {
        accumulatedPath += `${segment}/`;
        const crumb = document.createElement("span");
        crumb.textContent = segment;
        crumb.onclick = () => navigateTo(accumulatedPath);
        breadcrumbs.appendChild(document.createTextNode(" / "));
        breadcrumbs.appendChild(crumb);
    });
}

function navigateTo(path) {
    currentPath = path;
    refreshFiles();
}


async function deleteFile() {

    alert(`${endPoint}/file?filename=${encodeURIComponent(currentPath + currentFile)}`);
    showNotification(`${endPoint}/file?filename=${encodeURIComponent(currentPath + currentFile)}`);

    const confirmed = window.confirm(`Are you sure you want to delete ${currentPath}${currentFile}?`);

    if (!confirmed) {
        return;
    }

    try {
        const response = await fetch(`${endPoint}/file?filename=${encodeURIComponent(currentPath + currentFile)}`, {
            method: 'DELETE'
        });
        const result = await response.json();
        if (result.status === "success") {
            refreshFiles();
        } else {
            //alert("Error deleting item: " + result.message);
            showNotification("Error deleting item: " + result.message, "error");
        }
    } catch (err) {
        //console.error("Error deleting item:", err);
        showNotification("Error deleting item: " + err.message, "error");
    }
}

async function deleteItem() {
    const itemName = prompt("Enter file or folder name to delete:");
    if (!itemName) return;
    const isFolder = confirm("Is this a folder?");
    const endpoint = isFolder ? '/folder' : '/file';
    try {
        const response = await fetch(`${endPoint}${endpoint}?filename=${encodeURIComponent(currentPath + itemName)}`, {
            method: 'DELETE'
        });
        const result = await response.json();
        if (result.status === "success") {
            refreshFiles();
        } else {
            alert("Error deleting item: " + result.message);
        }
    } catch (err) {
        console.error("Error deleting item:", err);
    }
}

async function renameItem() {
    const oldName = prompt("Enter current name:");
    if (!oldName) return;
    const newName = prompt("Enter new name:");
    if (!newName) return;
    try {
        const response = await fetch(`${endPoint}/rename`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ oldname: currentPath + oldName, newname: currentPath + newName })
        });
        const result = await response.json();
        if (result.status === "success") {
            refreshFiles();
        } else {
            alert("Error renaming item: " + result.message);
        }
    } catch (err) {
        console.error("Error renaming item:", err);
    }
}
*/

