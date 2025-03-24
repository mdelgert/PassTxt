const endPoint = window.location.hostname === "localhost" ? `http://demo1.local` : "";
let currentPath = '/';
let currentFile = null;

async function refreshFiles() {
    try {
        const folderResponse = await fetch(`${endPoint}/folders?path=${encodeURIComponent(currentPath)}`);
        const folderData = await folderResponse.json();

        const fileResponse = await fetch(`${endPoint}/files?path=${encodeURIComponent(currentPath)}`);
        const fileData = await fileResponse.json();

        if (folderData.status !== "success" || fileData.status !== "success") {
            console.error("Error fetching data:", folderData.message || fileData.message);
            return;
        }

        const fileList = document.getElementById('fileList');
        fileList.innerHTML = '';

        // Add back button if not in root
        if (currentPath !== '/') {
            const backButton = document.createElement('li');
            backButton.textContent = '.. (Back)';
            backButton.style.fontStyle = 'italic';
            backButton.onclick = () => goBack();
            fileList.appendChild(backButton);
        }

        // Add folders
        folderData.data.folders.forEach(folder => {
            const li = document.createElement('li');
            li.textContent = folder;
            li.style.fontWeight = 'bold';
            li.onclick = () => openFolder(folder);
            fileList.appendChild(li);
        });

        // Add files
        fileData.data.files.forEach(file => {
            const li = document.createElement('li');
            li.textContent = file;
            li.onclick = () => openFile(file);
            fileList.appendChild(li);
        });
    } catch (err) {
        console.error('Error refreshing file and folder list:', err);
    }
}

async function openFolder(folder) {
    currentPath = `${currentPath}/${folder}`.replace(/\/+/g, '/');
    refreshFiles();
}

function goBack() {
    currentPath = currentPath.substring(0, currentPath.lastIndexOf('/')) || '/';
    refreshFiles();
}

async function openFile(file) {
    try {
        const response = await fetch(`${endPoint}/file?filename=${encodeURIComponent(currentPath + '/' + file)}`);
        const content = await response.text();

        document.getElementById('editor').value = content;
        currentFile = file;
    } catch (err) {
        console.error('Error opening file:', err);
    }
}

async function saveFile() {
    if (!currentFile) {
        alert('No file selected.');
        return;
    }

    const content = document.getElementById('editor').value;
    try {
        const response = await fetch(`${endPoint}/file?filename=${encodeURIComponent(currentPath + '/' + currentFile)}`, {
            method: 'POST',
            headers: { 'Content-Type': 'text/plain' },
            body: content
        });

        const data = await response.json();
        if (data.status !== 'success') {
            alert('Failed to save file.');
        }
    } catch (err) {
        console.error('Error saving file:', err);
    }
}

async function newFile() {
    const fileName = prompt('Enter new file name:');
    if (!fileName) return;

    try {
        const response = await fetch(`${endPoint}/file?filename=${encodeURIComponent(currentPath + '/' + fileName)}`, {
            method: 'POST',
            headers: { 'Content-Type': 'text/plain' },
            body: ''
        });

        const data = await response.json();
        if (data.status === 'success') {
            refreshFiles();
        } else {
            alert('Failed to create file.');
        }
    } catch (err) {
        console.error('Error creating file:', err);
    }
}

async function createFolder() {
    const folderName = prompt('Enter new folder name:');
    if (!folderName) return;

    try {
        const response = await fetch(`${endPoint}/folder`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ foldername: `${currentPath}/${folderName}` })
        });

        const data = await response.json();
        if (data.status === 'success') {
            refreshFiles();
        } else {
            alert('Failed to create folder.');
        }
    } catch (err) {
        console.error('Error creating folder:', err);
    }
}

async function deleteItem() {
    if (!currentFile) {
        alert('No file or folder selected.');
        return;
    }

    if (!confirm(`Are you sure you want to delete ${currentFile}?`)) return;

    const isFolder = currentFile.endsWith('/');

    try {
        const response = await fetch(`${endPoint}/${isFolder ? 'folder' : 'file'}`, {
            method: 'DELETE',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ filename: `${currentPath}/${currentFile}` })
        });

        const data = await response.json();
        if (data.status === 'success') {
            currentFile = null;
            refreshFiles();
        } else {
            alert('Failed to delete.');
        }
    } catch (err) {
        console.error('Error deleting item:', err);
    }
}

async function renameItem() {
    if (!currentFile) {
        alert('No file or folder selected.');
        return;
    }

    const newName = prompt('Enter new name:', currentFile);
    if (!newName) return;

    try {
        const response = await fetch(`${endPoint}/rename`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ oldname: `${currentPath}/${currentFile}`, newname: `${currentPath}/${newName}` })
        });

        const data = await response.json();
        if (data.status === 'success') {
            currentFile = newName;
            refreshFiles();
        } else {
            alert('Failed to rename item.');
        }
    } catch (err) {
        console.error('Error renaming item:', err);
    }
}

refreshFiles();
