#!/bin/bash

# Script to kill any process using port 8008

# Define the port
PORT=8008

# Find the Process IDs (PIDs) using the specified port
PIDS=$(lsof -ti :$PORT)

if [ -z "$PIDS" ]; then
    echo "No processes are using port $PORT."
    exit 0
fi

echo "Found the following process(es) using port $PORT: $PIDS"

# Iterate over each PID and attempt to terminate it
for PID in $PIDS; do
    # Check if the process exists
    if kill -0 $PID 2>/dev/null; then
        echo "Attempting to terminate process ID $PID..."
        
        # First, try to terminate gracefully
        kill $PID
        sleep 2

        # Check if the process is still running
        if kill -0 $PID 2>/dev/null; then
            echo "Process ID $PID did not terminate gracefully. Forcefully killing..."
            kill -9 $PID
        else
            echo "Process ID $PID terminated successfully."
        fi
    else
        echo "Process ID $PID is not running."
    fi
done

echo "All processes using port $PORT have been handled."

#pio home --no-open
#pio home #To launch in browser
