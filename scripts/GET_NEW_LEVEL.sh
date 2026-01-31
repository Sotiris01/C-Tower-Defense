#!/bin/bash

# ═══════════════════════════════════════════════════════════════════════
#  📥 GET NEW LEVEL - Download new content from teacher (Linux)
# ═══════════════════════════════════════════════════════════════════════

echo ""
echo "  ╔═══════════════════════════════════════════════════════════════╗"
echo "  ║     📥 GET NEW LEVEL - Λήψη Νέου Επιπέδου                      ║"
echo "  ╚═══════════════════════════════════════════════════════════════╝"
echo ""

# Check if we're in a git repository
if ! git rev-parse --git-dir > /dev/null 2>&1; then
    echo "  ❌ ERROR: Not in a git repository!"
    echo "     Run this script from the game folder."
    echo ""
    read -p "Press Enter to exit..."
    exit 1
fi

# Save any uncommitted changes first
echo "[1/4] Saving current changes..."
STASH_OUTPUT=$(git stash push -m "auto-stash before update" 2>&1)
if echo "$STASH_OUTPUT" | grep -q "Saved working directory"; then
    STASHED=true
    echo "      ✓ Your changes saved temporarily!"
else
    STASHED=false
    echo "      ℹ No changes to save."
fi

# Ensure we are on the student branch
echo ""
echo "[2/4] Checking branch..."
if ! git show-ref --verify --quiet refs/heads/student-nikolas; then
    echo "      Creating branch student-nikolas..."
    git checkout -b student-nikolas
else
    git checkout student-nikolas > /dev/null 2>&1
fi
echo "      ✓ Branch: student-nikolas"

# Fetch latest from remote
echo ""
echo "[3/4] Connecting to server..."
git fetch origin > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "  ❌ ERROR: Connection failed!"
    echo "     Check your internet connection."
    read -p "Press Enter to exit..."
    exit 1
fi
echo "      ✓ Connected!"

# Pull changes from main (Teacher's updates)
echo ""
echo "[4/4] Downloading new content from teacher..."
git pull origin main --no-edit 2>&1
PULL_RESULT=$?

if [ $PULL_RESULT -ne 0 ]; then
    echo ""
    echo "  ⚠ WARNING: There's a conflict!"
    echo ""
    echo "     Don't worry, your changes are safe."
    echo "     Ask your teacher for help."
    echo ""
    read -p "Press Enter to exit..."
    exit 1
fi

# Restore stashed changes if any
if [ "$STASHED" = true ]; then
    echo ""
    echo "      Restoring your changes..."
    git stash pop > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "  ⚠ Some changes need manual restore."
        echo "     Run: git stash pop"
    else
        echo "      ✓ Your changes restored!"
    fi
fi

echo ""
echo "  ╔═══════════════════════════════════════════════════════════════╗"
echo "  ║  ✅ New level downloaded successfully!                        ║"
echo "  ║                                                               ║"
echo "  ║  Run ./build_and_run.sh to see the changes!                   ║"
echo "  ╚═══════════════════════════════════════════════════════════════╝"
echo ""
read -p "Press Enter to exit..."
