@echo off
chcp 65001 >nul
setlocal EnableDelayedExpansion

echo.
echo  ╔═══════════════════════════════════════════════════════════════╗
echo  ║     💾 SAVE PROGRESS - Αποθήκευση Προόδου                      ║
echo  ╚═══════════════════════════════════════════════════════════════╝
echo.

:: Check if we're in a git repository
git rev-parse --git-dir >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo  ❌ ΣΦΑΛΜΑ: Δεν βρίσκεσαι σε git repository!
    echo     Τρέξε αυτό το script μέσα από τον φάκελο του παιχνιδιού.
    echo.
    pause
    exit /b 1
)

:: Check if student branch exists locally, if not create it
echo [1/4] Έλεγχος branch...
git show-ref --verify --quiet refs/heads/student-nikolas
if %ERRORLEVEL% neq 0 (
    echo       Δημιουργία branch student-nikolas...
    git checkout -b student-nikolas
    if %ERRORLEVEL% neq 0 (
        echo  ❌ ΣΦΑΛΜΑ: Αποτυχία δημιουργίας branch!
        pause
        exit /b 1
    )
) else (
    :: Switch to student branch if not already on it
    for /f "tokens=*" %%a in ('git rev-parse --abbrev-ref HEAD') do set CURRENT_BRANCH=%%a
    if not "!CURRENT_BRANCH!"=="student-nikolas" (
        echo       Αλλαγή σε branch student-nikolas...
        git checkout student-nikolas
    )
)
echo       ✓ Branch: student-nikolas

:: Add all changes
echo.
echo [2/4] Προσθήκη αλλαγών...
git add .
echo       ✓ Αλλαγές προστέθηκαν!

:: Commit with timestamp
echo.
echo [3/4] Δημιουργία commit...
set "ts=%DATE% %TIME:~0,8%"
git commit -m "Progress: %ts% - Nikolas" >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo       ℹ Δεν υπάρχουν νέες αλλαγές για αποθήκευση.
) else (
    echo       ✓ Commit δημιουργήθηκε!
)

:: Push to student branch
echo.
echo [4/4] Ανέβασμα στο cloud...
git push -u origin student-nikolas 2>nul
if %ERRORLEVEL% neq 0 (
    echo  ⚠ Πρώτη φορά; Δοκιμή με force push...
    git push -u origin student-nikolas --force
)

if %ERRORLEVEL% neq 0 (
    echo.
    echo  ❌ ΣΦΑΛΜΑ: Αποτυχία ανεβάσματος!
    echo     Έλεγξε τη σύνδεση internet.
    echo.
) else (
    echo       ✓ Ανέβηκε επιτυχώς!
    echo.
    echo  ╔═══════════════════════════════════════════════════════════════╗
    echo  ║  ✅ Η πρόοδός σου αποθηκεύτηκε στο cloud!                     ║
    echo  ╚═══════════════════════════════════════════════════════════════╝
)
echo.
pause
