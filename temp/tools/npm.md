# Download and install nvm:
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.1/install.sh | bash

# Download and install Node.js (need to close and open terminal for next command)
nvm install 22

# Verify the Node.js version:
node -v # Should print "v22.12.0".
nvm current # Should print "v22.12.0".

# Verify npm version:
npm -v # Should print "10.9.0".
