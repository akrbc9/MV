#!/bin/bash

# Exit on error
set -e

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check for required commands
if ! command_exists cmake; then
    echo "Error: cmake is not installed"
    exit 1
fi

if ! command_exists make; then
    echo "Error: make is not installed"
    exit 1
fi

if ! command_exists python3; then
    echo "Error: python3 is not installed"
    exit 1
fi

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

echo "Building project..."
if [ ! -d "build" ]; then
    mkdir build
fi
cd build
rm -rf *
cmake ..
make

# Verify the executable was created
if [ ! -f "bin/parameter_sweep" ]; then
    echo "Error: Failed to build parameter_sweep executable"
    exit 1
fi
cd ..

echo "Running parameter sweep analysis..."
cd parameter_sweep
python3 python/analyze_sweep.py
cd ..

echo "Done! Results are in parameter_sweep/results/" 