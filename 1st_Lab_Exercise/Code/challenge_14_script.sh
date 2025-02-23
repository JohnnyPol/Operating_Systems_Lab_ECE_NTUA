#!/bin/bash

# Check if the script is run as root
if [ "$(id -u)" -ne 0 ]; then
    echo "This script requires root privileges. Please run as root or with sudo."
    exit 1
fi

# Backup the current value of ns_last_pid
current_pid=$(cat /proc/sys/kernel/ns_last_pid)
echo "Current ns_last_pid: $current_pid"

# Set ns_last_pid to 32766 to ensure that the next PID is 32767
echo 32766 > /proc/sys/kernel/ns_last_pid

# Run the executable
./riddle &

# Wait for a moment to ensure that the process starts
sleep 1

# Get the PID of the newly created process
new_pid=$!

# Verify the PID of the running process
if [ "$new_pid" -eq 32767 ]; then
    echo "Successfully assigned PID 32767 to ./riddle."
else
    echo "The PID of ./riddle is $new_pid, not 32767."
fi

# Restore the original value of ns_last_pid
echo $current_pid > /proc/sys/kernel/ns_last_pid
