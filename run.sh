#!/usr/bin/env bash
set -e

# Default to local linux target
BINARY_NAME="raygui_test"

# Resolve where the executable sits based on your CMakePresets.json binaryDir structure
TARGET_EXE="build/$BINARY_NAME"

if [ -f "$TARGET_EXE" ]; then
    echo "🚀 Running $TARGET_EXE..."
    ./"$TARGET_EXE"
else
    echo "❌ Error: Executable not found at $TARGET_EXE"
    echo "💡 Please run './build.sh' first to compile it."
    exit 1
fi
