#!/bin/bash

# Exit on error
set -e

echo "Setting up parameter sweep environment..."

# Create and activate virtual environment
echo "Creating Python virtual environment..."
python3 -m venv venv
source venv/bin/activate

# Install required packages
echo "Installing Python dependencies..."
pip install -r python/requirements.txt

# Build C++ project
echo "Building C++ project..."
mkdir -p build
cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
cd ..

# Make Python script executable
chmod +x parameter_sweep/python/parameter_sweep.py

# Run parameter sweep
echo "Running parameter sweep..."
python parameter_sweep/python/parameter_sweep.py

echo "Parameter sweep completed!"
echo "Results are in the 'results' directory"
echo "Plots are in the 'plots' directory"

# Deactivate virtual environment
deactivate 

