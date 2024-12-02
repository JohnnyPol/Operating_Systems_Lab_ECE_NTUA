#!/bin/bash

# Start the ./riddle executable in the background
./riddle &
# Get the PID of the last background command (process)
pid=$!

# Check if PID is found
if [ -n "$pid" ]; then
  echo "Started riddle process with PID: $pid"
  
  # Wait before sending SIGCONT
  sleep 2
  
  echo "Sending SIGCONT to process with PID: $pid"
  kill -SIGCONT "$pid"  # Send the SIGCONT signal to resume the process
else
  echo "Failed to start the riddle process."
fi
