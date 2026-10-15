#!/usr/bin/env bash
set -e

# $VCPKG_ROOT/vcpkg install 
# --x-install-root=deps 

echo "========================================="
echo " Building RayGUI Test"
echo "========================================="

# 2. Configure CMake directly (without a CMake JSON preset)
echo "-> Configuring CMake..."
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"

# 3. Build the binary
echo "-> Compiling target binary..."
cmake --build build

echo "✅ Build completed successfully!"
