console.log('device.js loaded');

import { httpGet, showMessage } from './global.js';

window.addEventListener('DOMContentLoaded', async () => {
  try {
    const data = await httpGet('/device/get');
    const tableBody = document.getElementById('device-info');
    tableBody.innerHTML = ''; // Clear existing rows

    // Loop over each key-value pair in the JSON
    for (const [key, value] of Object.entries(data)) {
      const row = document.createElement('tr');

      const keyCell = document.createElement('td');
      keyCell.textContent = key;

      const valueCell = document.createElement('td');
      valueCell.textContent = value;

      row.appendChild(keyCell);
      row.appendChild(valueCell);
      tableBody.appendChild(row);
    }
    console.log("Device info loaded successfully!");
  } catch (error) {
    console.error('Error fetching device info:', error);
    showMessage('Error fetching device info.', 'error');
  }
});
