// aes.js
console.log('aes.js loaded');

import { BASE_URL, httpGet, showMessage } from './global.js';

window.AesHelper = {
    // Constants matching the C# implementation
    KEY_SIZE: 32, // 256 bits = 32 bytes
    BLOCK_SIZE: 16, // 128 bits = 16 bytes
    SALT_SIZE: 16, // 16 bytes for the salt
    //ITERATIONS: 10000, // PBKDF2 iterations
    ITERATIONS: 1000, // PBKDF2 iterations

    /**
     * Encrypts the given plaintext using the specified password.
     * Returns a Base64 string combining the salt and the ciphertext.
     */
    encrypt: async function (plainText, password) {
        const enc = new TextEncoder();

        // Generate a random salt
        const salt = crypto.getRandomValues(new Uint8Array(this.SALT_SIZE));

        // Encode password as UTF-8
        const passwordBytes = enc.encode(password);

        // Derive the key and IV using PBKDF2
        const keyMaterial = await crypto.subtle.importKey(
            "raw",
            passwordBytes,
            "PBKDF2",
            false,
            ["deriveBits"]
        );

        const derivedBits = await crypto.subtle.deriveBits(
            {
                name: "PBKDF2",
                salt: salt,
                iterations: this.ITERATIONS,
                hash: "SHA-256",
            },
            keyMaterial,
            (this.KEY_SIZE + this.BLOCK_SIZE) * 8 // Derive key (256 bits) + IV (128 bits)
        );

        const keyArray = new Uint8Array(derivedBits);
        const key = keyArray.slice(0, this.KEY_SIZE); // First 32 bytes for the key
        const iv = keyArray.slice(this.KEY_SIZE); // Last 16 bytes for the IV

        // Encrypt the plaintext
        const encryptedData = await crypto.subtle.encrypt(
            {
                name: "AES-CBC",
                iv: iv,
            },
            await crypto.subtle.importKey(
                "raw",
                key,
                { name: "AES-CBC" },
                false,
                ["encrypt"]
            ),
            enc.encode(plainText)
        );

        // Combine salt and ciphertext
        const combined = new Uint8Array(salt.length + encryptedData.byteLength);
        combined.set(salt); // Prepend the salt
        combined.set(new Uint8Array(encryptedData), salt.length); // Append the ciphertext

        // Convert to Base64
        return btoa(String.fromCharCode.apply(null, combined));
    },

    /**
     * Decrypts the given Base64-encoded ciphertext using the specified password.
     * Expects the input to contain the salt and the ciphertext.
     */
    decrypt: async function (encryptedText, password) {
        const enc = new TextEncoder();
        const dec = new TextDecoder();

        // Decode Base64 input
        const combined = new Uint8Array(
            atob(encryptedText).split("").map((c) => c.charCodeAt(0))
        );

        if (combined.length < this.SALT_SIZE) {
            throw new Error("Invalid encrypted data. Too short to contain salt and ciphertext.");
        }

        // Extract the salt
        const salt = combined.slice(0, this.SALT_SIZE);

        // Extract the ciphertext
        const cipherText = combined.slice(this.SALT_SIZE);

        // Encode password as UTF-8
        const passwordBytes = enc.encode(password);

        // Derive the key and IV using PBKDF2
        const keyMaterial = await crypto.subtle.importKey(
            "raw",
            passwordBytes,
            "PBKDF2",
            false,
            ["deriveBits"]
        );

        const derivedBits = await crypto.subtle.deriveBits(
            {
                name: "PBKDF2",
                salt: salt,
                iterations: this.ITERATIONS,
                hash: "SHA-256",
            },
            keyMaterial,
            (this.KEY_SIZE + this.BLOCK_SIZE) * 8 // Derive key (256 bits) + IV (128 bits)
        );

        const keyArray = new Uint8Array(derivedBits);
        const key = keyArray.slice(0, this.KEY_SIZE); // First 32 bytes for the key
        const iv = keyArray.slice(this.KEY_SIZE); // Last 16 bytes for the IV

        // Decrypt the ciphertext
        const decryptedData = await crypto.subtle.decrypt(
            {
                name: "AES-CBC",
                iv: iv,
            },
            await crypto.subtle.importKey(
                "raw",
                key,
                { name: "AES-CBC" },
                false,
                ["decrypt"]
            ),
            cipherText
        );

        return dec.decode(decryptedData);
    },
};


window.addEventListener('DOMContentLoaded', () => {
    const encryptBtn = document.getElementById('encrypt-btn');
    const decryptBtn = document.getElementById('decrypt-btn');
    const textArea = document.getElementById('aes-textarea');
    const passwordInput = document.getElementById('aes-password');
    const messageBox = document.getElementById('message-box');

    encryptBtn.addEventListener('click', async () => {
        const text = textArea.value;
        const password = passwordInput.value;

        if (!text || !password) {
            showMessage("Please provide both text and password.", "success");
            return;
        }

        try {
            const encryptedText = await window.AesHelper.encrypt(text, password);
            textArea.value = encryptedText;
            showMessage("Text encrypted successfully!", "success");
        } catch (err) {
            console.error('Encryption error:', err);
            showMessage("Encryption failed. Check the console for details.", "error");
        }
    });

    decryptBtn.addEventListener('click', async () => {
        const text = textArea.value;
        const password = passwordInput.value;

        if (!text || !password) {
            showMessage("Please provide both text and password.", "success");
            return;
        }

        try {
            const decryptedText = await window.AesHelper.decrypt(text, password);
            textArea.value = decryptedText;
            showMessage("Text decrypted successfully!", "success");
        } catch (err) {
            console.error('Decryption error:', err);
            showMessage("Decryption failed. Check the console for details.", "error");
        }
    });
});