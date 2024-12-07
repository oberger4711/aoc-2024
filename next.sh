#!/bin/bash

# Creates the directory for the next day as copy of the template directory.
# This script is from copilot.

# Find the highest numbered directory.
highest=0
for dir in [0-9][0-9]; do
  if [[ -d $dir ]]; then
    num=${dir#0} # Remove leading zero.
    if (( num > highest )); then
      highest=$num
    fi
  fi
done

# Calculate the next directory number.
next=$((highest + 1))
next_dir=$(printf "%02d" $next)

# Copy the template directory to the new directory.
cp -r template "$next_dir"

echo "Created directory $next_dir from template"