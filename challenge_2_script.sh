#!/bin/bash

# Get the PID of the riddle process (ignores the grep process)
pid=$(ps aux | grep '[r]iddle' | awk '{print $2}')

# Check if PID is found
if [ -n "$pid" ]; then
  echo "Sending SIGCONT to process with PID: $pid"
  kill -SIGCONT "$pid"
else
  echo "No riddle process found."
fi

