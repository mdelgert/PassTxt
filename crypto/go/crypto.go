/*
package main

import (
	"crypto/aes"
	"crypto/cipher"
	"crypto/rand"
	"crypto/sha256"
	"encoding/base64"
	"fmt"
	"os"
	"strings"
)

const blockSize = 16 // AES block size

func encrypt(plainText, password string) (string, error) {
	key := sha256.Sum256([]byte(password)) // SHA-256 key derivation

	// Generate random IV
	iv := make([]byte, blockSize)
	if _, err := rand.Read(iv); err != nil {
		return "", fmt.Errorf("failed to generate IV: %v", err)
	}

	// Pad plaintext with PKCS7
	padLen := blockSize - (len(plainText) % blockSize)
	padText := make([]byte, len(plainText)+padLen)
	copy(padText, plainText)
	for i := len(plainText); i < len(padText); i++ {
		padText[i] = byte(padLen)
	}

	// Encrypt
	block, err := aes.NewCipher(key[:])
	if err != nil {
		return "", fmt.Errorf("failed to create cipher: %v", err)
	}
	cipherText := make([]byte, len(padText))
	mode := cipher.NewCBCEncrypter(block, iv)
	mode.CryptBlocks(cipherText, padText)

	// Prepend IV and encode
	finalOutput := append(iv, cipherText...)
	return base64.StdEncoding.EncodeToString(finalOutput), nil
}

func decrypt(encryptedBase64, password string) (string, error) {
	key := sha256.Sum256([]byte(password))

	// Decode Base64
	encryptedBytes, err := base64.StdEncoding.DecodeString(encryptedBase64)
	if err != nil {
		return "", fmt.Errorf("base64 decode failed: %v", err)
	}
	if len(encryptedBytes) < blockSize {
		return "", fmt.Errorf("invalid encrypted data: too short")
	}

	// Extract IV and ciphertext
	iv := encryptedBytes[:blockSize]
	cipherText := encryptedBytes[blockSize:]

	// Decrypt
	block, err := aes.NewCipher(key[:])
	if err != nil {
		return "", fmt.Errorf("failed to create cipher: %v", err)
	}
	plainText := make([]byte, len(cipherText))
	mode := cipher.NewCBCDecrypter(block, iv)
	mode.CryptBlocks(plainText, cipherText)

	// Remove PKCS7 padding
	padLen := int(plainText[len(plainText)-1])
	if padLen > blockSize || padLen <= 0 {
		return "", fmt.Errorf("invalid padding value: %d", padLen)
	}
	for i := len(plainText) - padLen; i < len(plainText); i++ {
		if plainText[i] != byte(padLen) {
			return "", fmt.Errorf("invalid PKCS7 padding")
		}
	}
	return string(plainText[:len(plainText)-padLen]), nil
}

func main() {
	if len(os.Args) < 4 {
		fmt.Printf("Usage: %s <enc|dec> <password> <text>\n", os.Args[0])
		os.Exit(1)
	}

	command := strings.ToLower(os.Args[1])
	password := os.Args[2]
	text := os.Args[3]

	switch command {
	case "enc":
		encrypted, err := encrypt(text, password)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error: %v\n", err)
			os.Exit(1)
		}
		fmt.Println("Encrypted:", encrypted)
	case "dec":
		decrypted, err := decrypt(text, password)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error: %v\n", err)
			os.Exit(1)
		}
		fmt.Println("Decrypted:", decrypted)
	default:
		fmt.Println("Invalid command. Use 'enc' or 'dec'.")
		os.Exit(1)
	}
}
*/