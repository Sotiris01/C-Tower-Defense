#!/bin/bash

# ═══════════════════════════════════════════════════════════════════════
#  PLANETARY DEFENSE COMMAND - Setup Script (Linux)
# ═══════════════════════════════════════════════════════════════════════
#  Run this script in terminal to download the game!
#  It will automatically install Git and g++ if missing!
# ═══════════════════════════════════════════════════════════════════════

echo ""
echo "  ╔═══════════════════════════════════════════════════════════════╗"
echo "  ║     🚀 PLANETARY DEFENSE COMMAND - SETUP                      ║"
echo "  ║     Welcome!                                                  ║"
echo "  ╚═══════════════════════════════════════════════════════════════╝"
echo ""

# Repository URL
REPO_URL="https://github.com/Sotiris01/C-Tower-Defense.git"
FOLDER_NAME="C-Tower-Defense"
INSTALL_PATH="$HOME/Documents/$FOLDER_NAME"

# Detect package manager
detect_package_manager() {
    if command -v apt &> /dev/null; then
        PKG_MANAGER="apt"
        PKG_INSTALL="sudo apt install -y"
        PKG_UPDATE="sudo apt update"
    elif command -v dnf &> /dev/null; then
        PKG_MANAGER="dnf"
        PKG_INSTALL="sudo dnf install -y"
        PKG_UPDATE="sudo dnf check-update"
    elif command -v pacman &> /dev/null; then
        PKG_MANAGER="pacman"
        PKG_INSTALL="sudo pacman -S --noconfirm"
        PKG_UPDATE="sudo pacman -Sy"
    elif command -v zypper &> /dev/null; then
        PKG_MANAGER="zypper"
        PKG_INSTALL="sudo zypper install -y"
        PKG_UPDATE="sudo zypper refresh"
    else
        echo "  ❌ ERROR: Could not detect package manager!"
        echo "     Please install git and g++ manually."
        exit 1
    fi
    echo "  Detected package manager: $PKG_MANAGER"
}

# ─────────────────────────────────────────────────────────────────────
# STEP 1: Check and install Git
# ─────────────────────────────────────────────────────────────────────
echo "[1/6] Checking for Git..."
if ! command -v git &> /dev/null; then
    echo "      Git not found. Installing..."
    detect_package_manager
    $PKG_UPDATE
    $PKG_INSTALL git
    
    if ! command -v git &> /dev/null; then
        echo ""
        echo "  ❌ ERROR: Failed to install Git!"
        echo "     Please install manually: sudo apt install git"
        echo ""
        read -p "Press Enter to exit..."
        exit 1
    fi
    echo "      ✓ Git installed successfully!"
else
    echo "      ✓ Git found!"
fi

# ─────────────────────────────────────────────────────────────────────
# STEP 2: Check and install g++ (C++ compiler)
# ─────────────────────────────────────────────────────────────────────
echo ""
echo "[2/6] Checking for g++ (C++ compiler)..."
if ! command -v g++ &> /dev/null; then
    echo "      g++ not found. Installing build-essential..."
    
    if [ -z "$PKG_MANAGER" ]; then
        detect_package_manager
    fi
    
    case $PKG_MANAGER in
        apt)
            $PKG_INSTALL build-essential
            ;;
        dnf)
            $PKG_INSTALL gcc-c++ make
            ;;
        pacman)
            $PKG_INSTALL base-devel
            ;;
        zypper)
            $PKG_INSTALL gcc-c++ make
            ;;
    esac
    
    if ! command -v g++ &> /dev/null; then
        echo ""
        echo "  ❌ ERROR: Failed to install g++!"
        echo "     Please install manually: sudo apt install build-essential"
        echo ""
        read -p "Press Enter to exit..."
        exit 1
    fi
    echo "      ✓ g++ installed successfully!"
else
    echo "      ✓ g++ found!"
fi

# ─────────────────────────────────────────────────────────────────────
# STEP 3: Check and install Raylib dependencies
# ─────────────────────────────────────────────────────────────────────
echo ""
echo "[3/6] Checking Raylib dependencies..."

if [ -z "$PKG_MANAGER" ]; then
    detect_package_manager
fi

case $PKG_MANAGER in
    apt)
        echo "      Installing Raylib dependencies..."
        $PKG_INSTALL libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev
        ;;
    dnf)
        $PKG_INSTALL alsa-lib-devel mesa-libGL-devel libX11-devel libXrandr-devel libXi-devel libXcursor-devel libXinerama-devel
        ;;
    pacman)
        $PKG_INSTALL alsa-lib mesa libx11 libxrandr libxi libxcursor libxinerama
        ;;
    zypper)
        $PKG_INSTALL alsa-devel Mesa-libGL-devel libX11-devel libXrandr-devel libXi-devel libXcursor-devel libXinerama-devel
        ;;
esac
echo "      ✓ Dependencies installed!"

# ─────────────────────────────────────────────────────────────────────
# STEP 4: Choose folder
# ─────────────────────────────────────────────────────────────────────
echo ""
echo "[4/6] Setting up installation folder..."
echo ""
echo "     The game will be downloaded to:"
echo "     $INSTALL_PATH"
echo ""

# Create Documents folder if it doesn't exist
mkdir -p "$HOME/Documents"

# Check if folder already exists
if [ -d "$INSTALL_PATH" ]; then
    echo "  ⚠ WARNING: Folder already exists!"
    echo ""
    read -p "     Do you want to delete and re-download? (y/n): " OVERWRITE
    if [ "$OVERWRITE" = "y" ] || [ "$OVERWRITE" = "Y" ]; then
        echo "     Deleting old folder..."
        rm -rf "$INSTALL_PATH"
    else
        echo ""
        echo "     OK! Opening existing project..."
        # Skip to VS Code
        SKIP_CLONE=true
    fi
fi

# ─────────────────────────────────────────────────────────────────────
# STEP 5: Clone the repository
# ─────────────────────────────────────────────────────────────────────
if [ "$SKIP_CLONE" != "true" ]; then
    echo ""
    echo "[5/6] Downloading the game from GitHub..."
    echo "     (This may take a moment)"
    echo ""

    cd "$HOME/Documents"
    git clone "$REPO_URL"

    if [ $? -ne 0 ]; then
        echo ""
        echo "  ❌ ERROR: Download failed!"
        echo "     Check your internet connection and try again."
        echo ""
        read -p "Press Enter to exit..."
        exit 1
    fi

    echo ""
    echo "      ✓ Game downloaded successfully!"
fi

# ─────────────────────────────────────────────────────────────────────
# STEP 5.5: Install Raylib (download and setup)
# ─────────────────────────────────────────────────────────────────────
echo ""
echo "[5.5/6] Setting up Raylib..."

RAYLIB_HEADER="$INSTALL_PATH/src/engine/raylib/include/raylib.h"
RAYLIB_TARGET="$INSTALL_PATH/src/engine/raylib"

if [ -f "$RAYLIB_HEADER" ]; then
    echo "      ✓ Raylib found!"
else
    echo "      Raylib not found. Downloading..."
    
    TEMP_DIR="/tmp/raylib_setup"
    mkdir -p "$TEMP_DIR"
    
    # Download Raylib source
    RAYLIB_URL="https://github.com/raysan5/raylib/releases/download/5.0/raylib-5.0_linux_amd64.tar.gz"
    
    echo "      Downloading Raylib 5.0..."
    wget -q --show-progress -O "$TEMP_DIR/raylib.tar.gz" "$RAYLIB_URL"
    
    if [ ! -f "$TEMP_DIR/raylib.tar.gz" ]; then
        echo ""
        echo "  ❌ ERROR: Failed to download Raylib!"
        echo "     Download manually from: https://github.com/raysan5/raylib/releases"
        echo ""
        read -p "Press Enter to exit..."
        exit 1
    fi
    
    echo "      Extracting Raylib..."
    tar -xzf "$TEMP_DIR/raylib.tar.gz" -C "$TEMP_DIR"
    
    # Create target directories
    mkdir -p "$RAYLIB_TARGET/include"
    mkdir -p "$RAYLIB_TARGET/lib"
    
    # Find and copy files
    RAYLIB_EXTRACTED=$(find "$TEMP_DIR" -maxdepth 1 -type d -name "raylib*" | head -1)
    if [ -d "$RAYLIB_EXTRACTED" ]; then
        cp -r "$RAYLIB_EXTRACTED/include/"* "$RAYLIB_TARGET/include/" 2>/dev/null || true
        cp -r "$RAYLIB_EXTRACTED/lib/"* "$RAYLIB_TARGET/lib/" 2>/dev/null || true
    fi
    
    # Cleanup
    rm -rf "$TEMP_DIR"
    
    if [ -f "$RAYLIB_HEADER" ]; then
        echo "      ✓ Raylib installed successfully!"
    else
        echo "      ⚠ Raylib setup may need manual configuration"
    fi
fi

# ─────────────────────────────────────────────────────────────────────
# STEP 6: Open in VS Code
# ─────────────────────────────────────────────────────────────────────
echo ""
echo "[6/6] Opening in VS Code..."

# Check if VS Code is installed
if ! command -v code &> /dev/null; then
    echo ""
    echo "  ⚠ VS Code not found."
    echo "     Open the folder manually:"
    echo "     $INSTALL_PATH"
    echo ""
    
    # Try to open file manager
    if command -v xdg-open &> /dev/null; then
        xdg-open "$INSTALL_PATH"
    elif command -v nautilus &> /dev/null; then
        nautilus "$INSTALL_PATH"
    fi
else
    code "$INSTALL_PATH"
fi

echo ""
echo "  ╔═══════════════════════════════════════════════════════════════╗"
echo "  ║  ✅ DONE! The game is ready!                                  ║"
echo "  ╠═══════════════════════════════════════════════════════════════╣"
echo "  ║                                                               ║"
echo "  ║  📁 Folder: $FOLDER_NAME                                      ║"
echo "  ║                                                               ║"
echo "  ║  🎯 NEXT STEPS:                                               ║"
echo "  ║                                                               ║"
echo "  ║  1. Go to folder: src/student/                                ║"
echo "  ║                                                               ║"
echo "  ║  2. Start with file: 01_Economy.cpp                           ║"
echo "  ║                                                               ║"
echo "  ║  3. Read the instructions and fill in the code!               ║"
echo "  ║                                                               ║"
echo "  ║  4. Run: ./build_and_run.sh to see the game                   ║"
echo "  ║                                                               ║"
echo "  ╚═══════════════════════════════════════════════════════════════╝"
echo ""
echo "  Good luck! 🎮"
echo ""
read -p "Press Enter to exit..."
