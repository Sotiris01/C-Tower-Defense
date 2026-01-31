#!/bin/bash

# ═══════════════════════════════════════════════════════════════════════
#  💾 SAVE PROGRESS - Save your work to cloud (Linux)
# ═══════════════════════════════════════════════════════════════════════

echo ""
echo "  ╔═══════════════════════════════════════════════════════════════╗"
echo "  ║     💾 SAVE PROGRESS - Αποθήκευση Προόδου                      ║"
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

# Check if student branch exists locally, if not create it
echo "[1/4] Checking branch..."
if ! git show-ref --verify --quiet refs/heads/student-nikolas; then
    echo "      Creating branch student-nikolas..."
    git checkout -b student-nikolas
    if [ $? -ne 0 ]; then
        echo "  ❌ ERROR: Failed to create branch!"
        read -p "Press Enter to exit..."
        exit 1
    fi
else
    # Switch to student branch if not already on it
    CURRENT_BRANCH=$(git rev-parse --abbrev-ref HEAD)
    if [ "$CURRENT_BRANCH" != "student-nikolas" ]; then
        echo "      Switching to branch student-nikolas..."
        git checkout student-nikolas
    fi
fi
echo "      ✓ Branch: student-nikolas"

# Add all changes
echo ""
echo "[2/4] Adding changes..."
git add .
echo "      ✓ Changes added!"

# Commit with timestamp
echo ""
echo "[3/4] Creating commit..."
TIMESTAMP=$(date "+%Y-%m-%d %H:%M:%S")
git commit -m "Progress: $TIMESTAMP" > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "      ℹ No new changes to save."
else
    echo "      ✓ Commit created!"
fi

# Push to student branch
echo ""
echo "[4/4] Uploading to cloud..."
git push -u origin student-nikolas 2>/dev/null
if [ $? -ne 0 ]; then
    echo "  ⚠ First time? Trying force push..."
    git push -u origin student-nikolas --force
fi

if [ $? -ne 0 ]; then
    echo ""
    echo "  ❌ ERROR: Upload failed!"
    echo "     Check your internet connection."
    echo ""
else
    echo "      ✓ Uploaded successfully!"
    echo ""
    echo "  ╔═══════════════════════════════════════════════════════════════╗"
    echo "  ║  ✅ Your progress has been saved to cloud!                    ║"
    echo "  ╚═══════════════════════════════════════════════════════════════╝"
fi
echo ""
read -p "Press Enter to exit..."
