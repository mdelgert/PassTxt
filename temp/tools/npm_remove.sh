#!/bin/bash

set -e

echo "Starting the uninstallation of NVM and Node.js..."

# Step 1: Remove NVM directory
if [ -d "$HOME/.nvm" ]; then
  echo "Removing NVM directory at $HOME/.nvm..."
  rm -rf "$HOME/.nvm"
else
  echo "NVM directory not found. Skipping..."
fi

# Step 2: Remove NVM configuration from shell profiles
remove_nvm_from_profile() {
  local profile=$1
  if [ -f "$profile" ]; then
    echo "Checking $profile for NVM configurations..."
    sed -i '/nvm.sh/d' "$profile"
    sed -i '/nvm/d' "$profile"
    echo "Removed NVM configurations from $profile."
  else
    echo "$profile not found. Skipping..."
  fi
}

remove_nvm_from_profile "$HOME/.bashrc"
remove_nvm_from_profile "$HOME/.zshrc"
remove_nvm_from_profile "$HOME/.profile"
remove_nvm_from_profile "$HOME/.bash_profile"

# Step 3: Remove Node.js binaries if globally installed
if command -v node >/dev/null 2>&1; then
  echo "Removing Node.js binary..."
  sudo rm -rf "$(which node)" "$(dirname "$(which node)")/node_modules"
else
  echo "Node.js binary not found. Skipping..."
fi

if command -v npm >/dev/null 2>&1; then
  echo "Removing npm binary..."
  sudo rm -rf "$(which npm)"
else
  echo "npm binary not found. Skipping..."
fi

# Step 4: Verify uninstallation
echo "Verifying uninstallation..."
if command -v nvm >/dev/null 2>&1; then
  echo "NVM is still installed. Uninstallation failed."
else
  echo "NVM successfully uninstalled."
fi

if command -v node >/dev/null 2>&1; then
  echo "Node.js is still installed. Uninstallation failed."
else
  echo "Node.js successfully uninstalled."
fi

if command -v npm >/dev/null 2>&1; then
  echo "npm is still installed. Uninstallation failed."
else
  echo "npm successfully uninstalled."
fi

echo "Uninstallation complete. Restart your terminal to apply changes."
