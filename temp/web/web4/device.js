//device.js
console.log('device.js loaded');

import { BASE_URL } from './global.js';

window.addEventListener('DOMContentLoaded', () => {
 fetch(`${BASE_URL}/device/get`)
   .then(response => response.json())
   .then(data => {
     const tableBody = document.getElementById('device-info');
     tableBody.innerHTML = ''; // clear existing rows

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
   })
   .catch(error => {
     console.error('Error fetching device info:', error);
   });
});