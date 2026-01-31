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
# STEP 3: Check and install Raylib (system-wide)
# ─────────────────────────────────────────────────────────────────────
echo ""
echo "[3/6] Installing Raylib..."

if [ -z "$PKG_MANAGER" ]; then
    detect_package_manager
fi

# Install Raylib based on package manager
case $PKG_MANAGER in
    apt)
        echo "      Installing Raylib and dependencies for Debian/Ubuntu..."
        $PKG_INSTALL libraylib-dev libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev
        ;;
    dnf)
        echo "      Installing Raylib and dependencies for Fedora..."
        $PKG_INSTALL raylib-devel alsa-lib-devel mesa-libGL-devel libX11-devel libXrandr-devel libXi-devel libXcursor-devel libXinerama-devel
        ;;
    pacman)
        echo "      Installing Raylib and dependencies for Arch..."
        $PKG_INSTALL raylib alsa-lib mesa libx11 libxrandr libxi libxcursor libxinerama
        ;;
    zypper)
        echo "      Installing Raylib and dependencies for openSUSE..."
        $PKG_INSTALL raylib-devel alsa-devel Mesa-libGL-devel libX11-devel libXrandr-devel libXi-devel libXcursor-devel libXinerama-devel
        ;;
esac

# Verify Raylib installation
if pkg-config --exists raylib 2>/dev/null; then
    echo "      ✓ Raylib installed successfully (system)!"
elif [ -f "/usr/include/raylib.h" ] || [ -f "/usr/local/include/raylib.h" ]; then
    echo "      ✓ Raylib found in system paths!"
else
    echo "      ⚠ Raylib package not available. Building from source..."
    
    # Build Raylib from source
    TEMP_DIR="/tmp/raylib_build"
    rm -rf "$TEMP_DIR"
    mkdir -p "$TEMP_DIR"
    cd "$TEMP_DIR"
    
    echo "      Downloading Raylib source..."
    git clone --depth 1 --branch 5.0 https://github.com/raysan5/raylib.git
    
    if [ -d "$TEMP_DIR/raylib" ]; then
        cd "$TEMP_DIR/raylib/src"
        echo "      Compiling Raylib (this may take a minute)..."
        make PLATFORM=PLATFORM_DESKTOP -j$(nproc)
        
        if [ $? -eq 0 ]; then
            echo "      Installing Raylib to system..."
            sudo make install
            echo "      ✓ Raylib built and installed successfully!"
        else
            echo "  ❌ ERROR: Failed to build Raylib!"
            echo "     Try installing manually."
            read -p "Press Enter to continue anyway..."
        fi
    fi
    
    # Cleanup
    rm -rf "$TEMP_DIR"
    cd "$HOME/Documents"
fi

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
# STEP 5.5: Copy Raylib to engine folder
# ─────────────────────────────────────────────────────────────────────
echo ""
echo "[5.5/6] Setting up Raylib in engine folder..."

RAYLIB_TARGET="$INSTALL_PATH/src/engine/raylib"

# Create target directories
mkdir -p "$RAYLIB_TARGET/include"
mkdir -p "$RAYLIB_TARGET/lib"

# Copy Raylib headers and libs from system to project
echo "      Copying Raylib files to project..."

# Find and copy headers
if [ -f "/usr/include/raylib.h" ]; then
    cp /usr/include/raylib.h "$RAYLIB_TARGET/include/"
    cp /usr/include/raymath.h "$RAYLIB_TARGET/include/" 2>/dev/null || true
    cp /usr/include/rlgl.h "$RAYLIB_TARGET/include/" 2>/dev/null || true
    echo "      ✓ Headers copied from /usr/include/"
elif [ -f "/usr/local/include/raylib.h" ]; then
    cp /usr/local/include/raylib.h "$RAYLIB_TARGET/include/"
    cp /usr/local/include/raymath.h "$RAYLIB_TARGET/include/" 2>/dev/null || true
    cp /usr/local/include/rlgl.h "$RAYLIB_TARGET/include/" 2>/dev/null || true
    echo "      ✓ Headers copied from /usr/local/include/"
else
    echo "      ⚠ Could not find raylib headers in system paths"
fi

# Find and copy library
if [ -f "/usr/lib/libraylib.a" ]; then
    cp /usr/lib/libraylib.a "$RAYLIB_TARGET/lib/"
    echo "      ✓ Library copied from /usr/lib/"
elif [ -f "/usr/local/lib/libraylib.a" ]; then
    cp /usr/local/lib/libraylib.a "$RAYLIB_TARGET/lib/"
    echo "      ✓ Library copied from /usr/local/lib/"
elif [ -f "/usr/lib/x86_64-linux-gnu/libraylib.a" ]; then
    cp /usr/lib/x86_64-linux-gnu/libraylib.a "$RAYLIB_TARGET/lib/"
    echo "      ✓ Library copied from /usr/lib/x86_64-linux-gnu/"
else
    # Try to find it anywhere
    RAYLIB_LIB=$(find /usr -name "libraylib.a" 2>/dev/null | head -1)
    if [ -n "$RAYLIB_LIB" ]; then
        cp "$RAYLIB_LIB" "$RAYLIB_TARGET/lib/"
        echo "      ✓ Library copied from $RAYLIB_LIB"
    else
        echo "      ⚠ Could not find libraylib.a - will use system library"
    fi
fi

# Verify
if [ -f "$RAYLIB_TARGET/include/raylib.h" ]; then
    echo "      ✓ Raylib setup complete!"
else
    echo "      ℹ Using system Raylib (headers not copied)"
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
