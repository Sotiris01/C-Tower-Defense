#!/bin/bash

# ═══════════════════════════════════════════════════════════════════════
#  PLANETARY DEFENSE COMMAND - Build & Run Script (Linux)
# ═══════════════════════════════════════════════════════════════════════

echo "========================================"
echo "  PLANETARY DEFENSE BUILD SYSTEM"
echo "  (Raylib Graphics Edition - Linux)"
echo "========================================"
echo ""

# Check if Raylib is installed
if [ ! -f "src/engine/raylib/include/raylib.h" ]; then
    echo "[WARNING] Raylib not found!"
    echo ""
    echo "Run the setup script first:"
    echo "  ./scripts/SETUP_GAME.sh"
    echo ""
    echo "Or install Raylib manually:"
    echo "  sudo apt install libraylib-dev"
    echo ""
    read -p "Press Enter to exit..."
    exit 1
fi

echo "[1/3] Cleaning previous build..."
rm -f game 2>/dev/null

echo "[2/3] Compiling defense systems with Raylib..."
echo ""

# Compile with g++
g++ src/engine/main.cpp src/student/*.cpp -o game -O2 \
    -I src/engine \
    -I src/engine/raylib/include \
    -L src/engine/raylib/lib \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 \
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

echo "[3/3] Launching Planetary Defense Command..."
echo ""
echo "========================================"
echo "  LAUNCHING GAME..."
echo "========================================"
echo ""

./game

echo ""
echo "Game closed. Thanks for playing!"
