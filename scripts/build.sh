#!/bin/bash

# Cores
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

if [ -f ".gitmodules" ]; then
    echo -e "[${GREEN}*${NC}] Updating submodules..."
    git submodule update --init --recursive || { echo -e "[${RED}*${NC}] Error updating submodules"; exit 1; }
else
    echo "No submodules found."
fi

if [ -d "build" ]; then
    echo "The 'build' directory already exists."
fi

mkdir -p build || { echo -e "[${RED}*${NC}] Error creating the 'build' directory"; exit 1; }
cd build || { echo -e "[${RED}*${NC}] Error entering the 'build' directory"; exit 1; }

echo -e "[${GREEN}*${NC}] Configuring the project with CMake..."
cmake .. || { echo -e "[${RED}*${NC}] Error running CMake"; exit 1; }

echo -e "[${GREEN}*${NC}] Compiling the project with make..."
make || { echo -e "[${RED}*${NC}] Error running Make"; exit 1; }

echo -e "[${GREEN}*${NC}] Done! The project has been built successfully."
exit 0
