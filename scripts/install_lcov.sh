#!/bin/bash
echo "This script installs LCOV tools to parent directory"
if [ -d "../lcov" ]; then
echo "You already have LCOV installed there!"
echo "Exiting..."
fi

if [ ! -d "../lcov" ]; then
cd ..
git clone https://github.com/linux-test-project/lcov
cd scripts
echo "DONE!"
fi