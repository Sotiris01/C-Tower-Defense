#!/bin/bash

# ═══════════════════════════════════════════════════════════════════════
#  PLANETARY DEFENSE COMMAND - Build & Run Script (Linux)
# ═══════════════════════════════════════════════════════════════════════

echo "========================================"
echo "  PLANETARY DEFENSE BUILD SYSTEM"
echo "  (Raylib Graphics Edition - Linux)"
echo "========================================"
echo ""

# Check if Raylib is installed (local or system-wide)
echo "[1/3] Checking for Raylib..."

# Try to find raylib - prefer local project folder first
RAYLIB_FOUND=false
RAYLIB_INCLUDE=""
RAYLIB_LIB=""

# Check local project folder first
if [ -f "src/engine/raylib/include/raylib.h" ]; then
    RAYLIB_FOUND=true
    RAYLIB_INCLUDE="-I src/engine/raylib/include"
    if [ -f "src/engine/raylib/lib/libraylib.a" ]; then
        RAYLIB_LIB="-L src/engine/raylib/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11"
    else
        RAYLIB_LIB="-lraylib -lGL -lm -lpthread -ldl -lrt -lX11"
    fi
    echo "      ✓ Raylib found (project folder)"
# Check system installation with pkg-config
elif pkg-config --exists raylib 2>/dev/null; then
    RAYLIB_FOUND=true
    RAYLIB_INCLUDE=$(pkg-config --cflags raylib)
    RAYLIB_LIB=$(pkg-config --libs raylib)
    echo "      ✓ Raylib found (system via pkg-config)"
# Check common system paths
elif [ -f "/usr/include/raylib.h" ] || [ -f "/usr/local/include/raylib.h" ]; then
    RAYLIB_FOUND=true
    RAYLIB_INCLUDE="-I/usr/local/include"
    RAYLIB_LIB="-lraylib -lGL -lm -lpthread -ldl -lrt -lX11"
    echo "      ✓ Raylib found (system path)"
fi

if [ "$RAYLIB_FOUND" = false ]; then
    echo ""
    echo "[WARNING] Raylib not found!"
    echo ""
    echo "Install Raylib with one of these commands:"
    echo ""
    echo "  Ubuntu/Debian:  sudo apt install libraylib-dev"
    echo "  Fedora:         sudo dnf install raylib-devel"
    echo "  Arch:           sudo pacman -S raylib"
    echo ""
    echo "Or build from source:"
    echo "  git clone https://github.com/raysan5/raylib.git"
    echo "  cd raylib/src && make PLATFORM=PLATFORM_DESKTOP"
    echo "  sudo make install"
    echo ""
    read -p "Press Enter to exit..."
    exit 1
fi

echo "[2/3] Cleaning previous build..."
rm -f game 2>/dev/null

echo "[3/3] Compiling defense systems with Raylib..."
echo ""

# Compile with g++
g++ src/engine/main.cpp src/student/*.cpp -o game -O2 \
    -I src/engine \
    $RAYLIB_INCLUDE \
    $RAYLIB_LIB \
    2> build_errors.txt

if [ $? -ne 0 ]; then
    echo "========================================"
    echo "  [ERROR] COMPILATION FAILED!"
    echo "========================================"
    echo ""
    echo "Defense systems have critical errors."
    echo "Check the code for syntax mistakes."
    echo ""
    echo "--- Error Details ---"
    cat build_errors.txt
    echo ""
    rm -f build_errors.txt
    read -p "Press Enter to exit..."
    exit 1
fi

rm -f build_errors.txt
echo "[OK] Compilation successful!"
echo ""

echo "Launching Planetary Defense Command..."
echo ""
echo "========================================"
echo "  LAUNCHING GAME..."
echo "========================================"
echo ""

./game

echo ""
echo "Game closed. Thanks for playing!"
