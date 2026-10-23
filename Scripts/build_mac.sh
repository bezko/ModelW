#!/bin/bash
set -e

# ==============================================================================
# ModelW Precision Dynamics - macOS Build Script (Apple Silicon AU & VST3)
# ==============================================================================

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_DIR="${SCRIPT_DIR}/.."
cd "${PROJECT_DIR}"

echo "==============================================================================="
echo " ModelW - Building macOS Apple Silicon AU & VST3 Plugins"
echo "==============================================================================="

# Verify CMake installation
if ! command -v cmake &> /dev/null; then
    echo "[ERROR] CMake not found. Please install CMake via Homebrew: brew install cmake"
    exit 1
fi

BUILD_DIR="build_mac"
mkdir -p "${BUILD_DIR}"

echo "Configuring project for Universal (arm64 + x86_64) Apple Silicon / Intel..."
cmake -B "${BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET="10.15"

echo "Compiling ModelW Release..."
cmake --build "${BUILD_DIR}" --config Release --parallel

echo ""
echo "==============================================================================="
echo " macOS Build Succeeded!"
echo " AudioUnit (AU):   ${PROJECT_DIR}/${BUILD_DIR}/ModelW_artefacts/Release/AU/ModelW.component"
echo " VST3 Plugin:      ${PROJECT_DIR}/${BUILD_DIR}/ModelW_artefacts/Release/VST3/ModelW.vst3"
echo " Standalone App:   ${PROJECT_DIR}/${BUILD_DIR}/ModelW_artefacts/Release/Standalone/ModelW.app"
echo "==============================================================================="
echo ""
echo "To install to your system audio plugin directories:"
echo "  cp -R \"${BUILD_DIR}/ModelW_artefacts/Release/AU/ModelW.component\" ~/Library/Audio/Plug-Ins/Components/"
echo "  cp -R \"${BUILD_DIR}/ModelW_artefacts/Release/VST3/ModelW.vst3\" ~/Library/Audio/Plug-Ins/VST3/"
echo "  killall -9 AudioComponentRegistrar  # Rescans AU plugins in Logic Pro / Ableton"
