# C++ Tower Defense: "Planetary Defense Command"

## 1. Ταυτότητα Μαθήματος
*   **Μαθητής:** Νικόλας (14 ετών, Java background)
*   **Στόχος:** Μετάβαση από Java σε C++ μέσω Game Development.
*   **Project:** Tower Defense Game (Grid-based, Minimalist).
*   **Πλατφόρμα:** VS Code (Windows).
*   **Τρέχουσα Φάση:** Setup & Basics.

## 2. Σκοπός & Ύλη (Dec 2025)
Βασισμένο στα αρχεία PDF:
1.  **Βασικές Έννοιες** (`01- ΒασικέςΈννοιες Dec 2025.pdf`): 
    *   Δομή προγράμματος (`main`, `#include`).
    *   Μεταβλητές (`int`, `double`, `string`, `bool`) & `const`.
    *   Είσοδος/Έξοδος (`cin`, `cout`).
    *   Τελεστές (`+`, `-`, `*`, `/`, `%`).
2.  **If / Else** (`02- If Dec 2025.pdf`): 
    *   Συνθήκες, Τελεστές σύγκρισης (`>`, `==`, `!=`).
    *   Λογικοί τελεστές (`&&`, `||`, `!`).
    *   Nested Ifs.
3.  **Loops** (`03- Βρόχοι_loops_ Dec 2025.pdf`): 
    *   `while`, `for`, `do-while`.
    *   Break/Continue.

## 3. Δομή Μαθημάτων (Curriculum)
Το μάθημα ακολουθεί το σενάριο "Chief Defense Programmer" (βλ. `TOWER_DEFENSE_CURRICULUM.md`).

*   **Μάθημα 1: Economy System (Variables)**
    *   Στόχος: Δήλωση μεταβλητών για χρήματα, ζωή βάσης, όνομα παίκτη.
    *   Αρχείο: `01_Economy.cpp`
*   **Μάθημα 2: Shop Logic (If/Else)**
    *   Στόχος: Έλεγχος αν επαρκούν τα χρήματα για αγορά πύργου.
    *   Αρχείο: `02_Shop.cpp`
*   **Μάθημα 3: Targeting System (Math & Logic)**
    *   Στόχος: Υπολογισμός απόστασης (range) και ζημιάς.
    *   Αρχείο: `03_Targeting.cpp`
*   **Μάθημα 4: Radar & Waves (Loops)**
    *   Στόχος: Έλεγχος όλων των εχθρών (σάρωση) για εντοπισμό απειλών.
    *   Αρχείο: `04_Radar.cpp`

## 4. Οργάνωση Αρχείων (Workspace)
*   `TOWER_DEFENSE_CURRICULUM.md`: Αναλυτικό πλάνο σεναρίου.
*   `GEMINI.md`: Context file για το μάθημα.
*   `src/`: Ο κώδικας του παιχνιδιού.
    *   `engine/`: (Hidden/Locked) Κώδικας γραφικών (Raylib ή Console mockup) και main loop.
    *   `student/`: Τα αρχεία που επεξεργάζεται ο Νικόλας.
*   `scripts/`: 
    *   `build_and_run.bat`: Script για αυτόματο compile & run ώστε να κρυφτεί η πολυπλοκότητα.

## 5. Επόμενα Βήματα
1.  Δημιουργία φακέλων (`src/student`, `src/engine`, `scripts`).
2.  Δημιουργία του `01_Economy.cpp` (σκελετός).
3.  Δημιουργία του `build_and_run.bat`.
