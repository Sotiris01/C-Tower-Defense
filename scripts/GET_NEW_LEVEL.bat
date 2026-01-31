@echo off
chcp 65001 >nul
setlocal EnableDelayedExpansion

echo.
echo  ╔═══════════════════════════════════════════════════════════════╗
echo  ║     📥 GET NEW LEVEL - Λήψη Νέου Επιπέδου                      ║
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

:: Save any uncommitted changes first
echo [1/4] Αποθήκευση τρεχουσών αλλαγών...
git stash push -m "auto-stash before update" >nul 2>&1
set STASHED=%ERRORLEVEL%
if %STASHED% equ 0 (
    echo       ✓ Οι αλλαγές σου αποθηκεύτηκαν προσωρινά!
) else (
    echo       ℹ Δεν υπάρχουν αλλαγές για αποθήκευση.
)

:: Ensure we are on the student branch
echo.
echo [2/4] Έλεγχος branch...
git show-ref --verify --quiet refs/heads/student-nikolas
if %ERRORLEVEL% neq 0 (
    echo       Δημιουργία branch student-nikolas...
    git checkout -b student-nikolas
) else (
    git checkout student-nikolas >nul 2>&1
)
echo       ✓ Branch: student-nikolas

:: Fetch latest from remote
echo.
echo [3/4] Σύνδεση με το server...
git fetch origin >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo  ❌ ΣΦΑΛΜΑ: Αποτυχία σύνδεσης!
    echo     Έλεγξε τη σύνδεση internet.
    pause
    exit /b 1
)
echo       ✓ Συνδέθηκε!

:: Pull changes from main (Teacher's updates)
echo.
echo [4/4] Λήψη νέου περιεχομένου από τον δάσκαλο...
git pull origin main --no-edit 2>&1
set PULL_RESULT=%ERRORLEVEL%

if %PULL_RESULT% neq 0 (
    echo.
    echo  ⚠ ΠΡΟΣΟΧΗ: Υπάρχει conflict!
    echo.
    echo     Μην ανησυχείς, οι αλλαγές σου είναι ασφαλείς.
    echo     Ζήτα βοήθεια από τον δάσκαλο.
    echo.
    pause
    exit /b 1
)

:: Restore stashed changes if any
if %STASHED% equ 0 (
    echo.
    echo       Επαναφορά των αλλαγών σου...
    git stash pop >nul 2>&1
    if %ERRORLEVEL% neq 0 (
        echo  ⚠ Μερικές αλλαγές χρειάζονται χειροκίνητη επαναφορά.
        echo     Τρέξε: git stash pop
    ) else (
        echo       ✓ Οι αλλαγές σου επαναφέρθηκαν!
    )
)

echo.
echo  ╔═══════════════════════════════════════════════════════════════╗
echo  ║  ✅ Νέο επίπεδο κατέβηκε επιτυχώς!                             ║
echo  ║                                                               ║
echo  ║  Τρέξε build_and_run.bat για να δεις τις αλλαγές!             ║
echo  ╚═══════════════════════════════════════════════════════════════╝
echo.
pause
