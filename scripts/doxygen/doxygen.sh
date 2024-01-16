#!/bin/bash

# https://gist.github.com/rain1024/98dd5e2c6c8c28f9ea9d
# https://askubuntu.com/questions/1291874/how-to-install-doxygen-on-ubuntu-20-04

# Define color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# Project information
PROJECT_NAME="LinuxDefender"
PROJECT_VERSION="1.0.0"

# Specify the paths
SOURCES="include/inotify include/err include/version sources/cli sources/telekinesis include/compiler sources/cjson include/logger sources/scan"
OUTPUT_DIR="documentation/doxygen"
STYLE="customdoxygen.css"
HEADER="customheader.html"
FOOTER="customfooter.html"
ICON="icon.png"
PROJECT_BRIEF="Linux Defender revolutionizes security for Linux servers by taking an advanced approach focusing on 0day threats"

# Step 1: Generate Doxygen configuration file
echo -e "${GREEN}[*] Generating Doxygen configuration file...${NC}"
mkdir -p $OUTPUT_DIR
cp scripts/doxygen/$STYLE scripts/doxygen/$ICON scripts/doxygen/$HEADER scripts/doxygen/$FOOTER "$OUTPUT_DIR/html/"
doxygen -g .doxyfile

# Step 2: Edit Doxygen configuration file
echo -e "${GREEN}[*] Editing Doxygen configuration file...${NC}"
sed -i "s|PROJECT_NAME.*=.*|PROJECT_NAME            = \"$PROJECT_NAME\"|" .doxyfile
sed -i "s|PROJECT_BRIEF.*=.*|PROJECT_BRIEF            = \"$PROJECT_BRIEF\"|" .doxyfile
sed -i "s|PROJECT_NUMBER.*=.*|PROJECT_NUMBER        = $PROJECT_VERSION|" .doxyfile
sed -i "s|OUTPUT_DIRECTORY.*=.*|OUTPUT_DIRECTORY    = $OUTPUT_DIR|" .doxyfile
sed -i "s|INPUT.*=.*|INPUT                          = $SOURCES |" .doxyfile
sed -i "s|HTML_EXTRA.*=.*|HTML_EXTRA_STYLESHEET     = $OUTPUT_DIR/html/$STYLE |" .doxyfile
sed -i "s|HTML_HEADER.*=.*|HTML_HEADER              = $OUTPUT_DIR/html/$HEADER|" .doxyfile
sed -i "s|HTML_FOOTER.*=.*|HTML_FOOTER              = $OUTPUT_DIR/html/$FOOTER|" .doxyfile

# Step 3: Generate documentation
echo -e "${GREEN}[*] Generating documentation...${NC}"
doxygen .doxyfile

# Step 4: Generate PDF
echo -e "${GREEN}[*] Generating PDF...${NC}"
cd $OUTPUT_DIR/latex
make

if [ $? -eq 0 ]; then
  echo -e "${GREEN}Documentation generated successfully!${NC}"
else
  echo -e "${RED}Error: Documentation generation failed.${NC}"
fi
