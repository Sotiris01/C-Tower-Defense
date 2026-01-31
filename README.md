# 🚀 Planetary Defense Command

A **C++ Tower Defense Game** designed for learning programming through game development.

![C++](https://img.shields.io/badge/C++-17-blue?logo=cplusplus)
![Raylib](https://img.shields.io/badge/Raylib-5.0-green)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)

---

## 📖 About

**Planetary Defense Command** is an educational project that teaches C++ programming concepts through building a real, playable tower defense game. Students complete exercises in the `src/student/` folder, and their code directly affects the game!

### Learning Topics:
- 📦 **Variables & Data Types** - Economy system (money, health, scores)
- 🔀 **If/Else Conditions** - Shop logic and game decisions  
- 🔢 **Math & Operators** - Targeting calculations and damage
- 🔄 **Loops** - Radar scanning and wave management
- 🎯 **Functions** - Upgrade system and game mechanics

---

## 🎮 Game Features

- **Grid-based Tower Defense** - Build towers on a 10x10 grid
- **Enemy Waves** - Defend your base against incoming enemies
- **Upgrade System** - Level up your towers for more power
- **Real-time Feedback** - See your code working in the game!
- **Visual Panels** - RADAR, STATUS, and COMMANDER displays

---

## ⚡ Quick Start

### First Time Setup

1. **Download** the `SETUP_GAME.bat` file
2. **Run** it in a terminal (or double-click)
3. The script will automatically install:
   - Git (if missing)
   - MinGW-w64 / g++ compiler (if missing)
   - Raylib 5.0 graphics library
   - Clone the project
   - Open in VS Code

### Building & Running

```batch
.\build_and_run.bat
```

This compiles your code and launches the game!

---

## 📁 Project Structure

```
├── src/
│   ├── student/          # 📝 YOUR CODE GOES HERE
│   │   ├── 01_Economy.cpp
│   │   ├── 02_Shop.cpp
│   │   ├── 03_Targeting.cpp
│   │   ├── 04_Radar.cpp
│   │   └── 05_Upgrades.cpp
│   └── engine/           # 🔒 Game engine (don't modify)
│       └── main.cpp
├── scripts/
│   ├── SETUP_GAME.bat    # First-time setup
│   ├── SAVE_PROGRESS.bat # Save your work to cloud
│   └── GET_NEW_LEVEL.bat # Get new content from teacher
└── build_and_run.bat     # Compile and play!
```

---

## 🎯 How to Learn

1. **Start with `01_Economy.cpp`** - Read the instructions in the file
2. **Complete the TODO sections** - Fill in the missing code
3. **Run `build_and_run.bat`** - See your changes in action!
4. **Move to the next file** - Progress through all 5 modules

Each file has detailed comments explaining:
- What you need to do
- Examples and hints
- The C++ concepts being taught

---

## 🎮 Game Controls

| Key | Action |
|-----|--------|
| **Left Click** | Build tower / Select tower |
| **ESC** | Deselect tower |
| **R** | Restart game |
| **U** | Auto-upgrade all towers |
| **SPACE** | Pause game |

---

## 📚 Student Exercises

| File | Topic | Concepts |
|------|-------|----------|
| `01_Economy.cpp` | Economy System | Variables, Constants, Data Types |
| `02_Shop.cpp` | Shop Logic | If/Else, Conditions, Comparisons |
| `03_Targeting.cpp` | Targeting System | Math, Functions, Distance |
| `04_Radar.cpp` | Radar & Scanning | Loops, Arrays, Counting |
| `05_Upgrades.cpp` | Upgrade System | All Combined! |

---

## 🛠️ Requirements

- **Windows 10/11**
- **VS Code** (recommended)
- Git, g++, Raylib (auto-installed by setup script)

---

## 📄 License

Educational project. Feel free to use for learning purposes.

---

*Built with ❤️ for learning C++ through game development*
