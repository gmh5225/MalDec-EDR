#!/bin/bash

# Colors
GREEN='\033[0;32m'
NC='\033[0m' # No color

# Directories to be formatted
directories=("sources")

# Run cppcheck for each directory
for dir in "${directories[@]}"; do
  echo -e "[${GREEN}*${NC}] Checking files in ${dir}..."
  find "$dir" -type f \( -name "*.c" -o -name "*.h" \) -print0 | xargs -0 cppcheck
done

echo -e "${GREEN}Checking completed.${NC}"