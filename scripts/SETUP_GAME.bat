@echo off
chcp 65001 >nul
setlocal EnableDelayedExpansion

:: ═══════════════════════════════════════════════════════════════════════
::  PLANETARY DEFENSE COMMAND - Setup Script για τον Νικόλα
:: ═══════════════════════════════════════════════════════════════════════
::  Τρέξε αυτό το script στο VS Code terminal για να κατεβάσεις το παιχνίδι!
::  Θα εγκαταστήσει αυτόματα Git και g++ αν δεν υπάρχουν!
:: ═══════════════════════════════════════════════════════════════════════

echo.
echo  ╔═══════════════════════════════════════════════════════════════╗
echo  ║     🚀 PLANETARY DEFENSE COMMAND - SETUP                      ║
echo  ║     Καλώς ήρθες Νικόλα!                                       ║
echo  ╚═══════════════════════════════════════════════════════════════╝
echo.

:: Repository URL
set REPO_URL=https://github.com/Sotiris01/C-Tower-Defense.git
set FOLDER_NAME=C-Tower-Defense

:: Temp folder for downloads
set TEMP_SETUP=%TEMP%\planetary_defense_setup
if not exist "%TEMP_SETUP%" mkdir "%TEMP_SETUP%"

:: ─────────────────────────────────────────────────────────────────────
:: ΒΗΜΑ 1: Έλεγχος και εγκατάσταση Git
:: ─────────────────────────────────────────────────────────────────────
echo [1/5] Έλεγχος για Git...
where git >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo       Git δεν βρέθηκε. Γίνεται αυτόματη εγκατάσταση...
    echo.
    
    :: Download Git installer
    echo       Κατέβασμα Git installer...
    set GIT_URL=https://github.com/git-for-windows/git/releases/download/v2.43.0.windows.1/Git-2.43.0-64-bit.exe
    set GIT_INSTALLER=%TEMP_SETUP%\git_installer.exe
    
    powershell -Command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri '%GIT_URL%' -OutFile '%GIT_INSTALLER%' -UseBasicParsing"
    
    if not exist "%GIT_INSTALLER%" (
        echo.
        echo  ❌ ΣΦΑΛΜΑ: Αποτυχία κατεβάσματος Git!
        echo     Κατέβασε χειροκίνητα από: https://git-scm.com/download/win
        echo.
        pause
        exit /b 1
    )
    
    echo       Εγκατάσταση Git (θα ανοίξει παράθυρο)...
    echo       ⚠ Πάτα NEXT σε όλα τα βήματα!
    echo.
    
    :: Run installer silently with default options
    "%GIT_INSTALLER%" /VERYSILENT /NORESTART /NOCANCEL /SP- /CLOSEAPPLICATIONS /RESTARTAPPLICATIONS /COMPONENTS="icons,ext\reg\shellhere,assoc,assoc_sh"
    
    :: Add Git to current session PATH
    set "PATH=%PATH%;C:\Program Files\Git\bin;C:\Program Files\Git\cmd"
    
    :: Verify installation
    where git >nul 2>&1
    if %ERRORLEVEL% neq 0 (
        echo.
        echo  ⚠ Το Git εγκαταστάθηκε αλλά χρειάζεται επανεκκίνηση terminal.
        echo     Κλείσε αυτό το παράθυρο και τρέξε ΞΑΝΑ το script.
        echo.
        pause
        exit /b 1
    )
    echo       ✓ Git εγκαταστάθηκε επιτυχώς!
) else (
    echo       ✓ Git βρέθηκε!
)

:: ─────────────────────────────────────────────────────────────────────
:: ΒΗΜΑ 2: Έλεγχος και εγκατάσταση g++ (MinGW-w64)
:: ─────────────────────────────────────────────────────────────────────
echo.
echo [2/5] Έλεγχος για g++ (C++ compiler)...
where g++ >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo       g++ δεν βρέθηκε. Γίνεται αυτόματη εγκατάσταση MinGW-w64...
    echo.
    
    :: Download MinGW-w64 (winlibs release - contains g++)
    echo       Κατέβασμα MinGW-w64 (~50MB, περίμενε)...
    set MINGW_URL=https://github.com/brechtsanders/winlibs_mingw/releases/download/13.2.0posix-17.0.6-11.0.1-ucrt-r5/winlibs-x86_64-posix-seh-gcc-13.2.0-mingw-w64ucrt-11.0.1-r5.zip
    set MINGW_ZIP=%TEMP_SETUP%\mingw.zip
    set MINGW_INSTALL=C:\mingw64
    
    powershell -Command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; $ProgressPreference = 'SilentlyContinue'; Invoke-WebRequest -Uri '%MINGW_URL%' -OutFile '%MINGW_ZIP%' -UseBasicParsing"
    
    if not exist "%MINGW_ZIP%" (
        echo.
        echo  ❌ ΣΦΑΛΜΑ: Αποτυχία κατεβάσματος MinGW!
        echo     Κατέβασε χειροκίνητα από: https://winlibs.com/
        echo.
        pause
        exit /b 1
    )
    
    echo       Αποσυμπίεση MinGW-w64...
    
    :: Extract to C:\mingw64
    powershell -Command "Expand-Archive -Path '%MINGW_ZIP%' -DestinationPath 'C:\' -Force"
    
    if not exist "C:\mingw64\bin\g++.exe" (
        echo.
        echo  ❌ ΣΦΑΛΜΑ: Αποτυχία αποσυμπίεσης MinGW!
        echo.
        pause
        exit /b 1
    )
    
    :: Add MinGW to system PATH permanently
    echo       Προσθήκη στο PATH...
    powershell -Command "[Environment]::SetEnvironmentVariable('Path', [Environment]::GetEnvironmentVariable('Path', 'User') + ';C:\mingw64\bin', 'User')"
    
    :: Add to current session PATH
    set "PATH=%PATH%;C:\mingw64\bin"
    
    :: Verify installation
    where g++ >nul 2>&1
    if %ERRORLEVEL% neq 0 (
        echo.
        echo  ⚠ Το MinGW εγκαταστάθηκε αλλά χρειάζεται επανεκκίνηση terminal.
        echo     Κλείσε αυτό το παράθυρο και τρέξε ΞΑΝΑ το script.
        echo.
        pause
        exit /b 1
    )
    echo       ✓ g++ εγκαταστάθηκε επιτυχώς!
) else (
    echo       ✓ g++ βρέθηκε!
)

:: ─────────────────────────────────────────────────────────────────────
:: ΒΗΜΑ 3: Επιλογή φακέλου
:: ─────────────────────────────────────────────────────────────────────
echo.
echo [3/6] Επιλογή φακέλου εγκατάστασης...
echo.
echo     Το παιχνίδι θα κατέβει στον φάκελο:
echo     %USERPROFILE%\Documents\%FOLDER_NAME%
echo.

set INSTALL_PATH=%USERPROFILE%\Documents\%FOLDER_NAME%

:: Έλεγχος αν υπάρχει ήδη ο φάκελος
if exist "%INSTALL_PATH%" (
    echo  ⚠ ΠΡΟΣΟΧΗ: Ο φάκελος υπάρχει ήδη!
    echo.
    set /p OVERWRITE="     Θέλεις να τον διαγράψεις και να κατεβάσεις ξανά; (Y/N): "
    if /i "!OVERWRITE!"=="Y" (
        echo     Διαγραφή παλιού φακέλου...
        rmdir /s /q "%INSTALL_PATH%"
    ) else (
        echo.
        echo     OK! Άνοιγμα του υπάρχοντος project...
        goto :OPEN_VSCODE
    )
)

:: ─────────────────────────────────────────────────────────────────────
:: ΒΗΜΑ 4: Clone του repository
:: ─────────────────────────────────────────────────────────────────────
echo.
echo [4/6] Κατέβασμα του παιχνιδιού από το GitHub...
echo     (Αυτό μπορεί να πάρει λίγο χρόνο)
echo.

cd /d "%USERPROFILE%\Documents"
git clone %REPO_URL%

if %ERRORLEVEL% neq 0 (
    echo.
    echo  ❌ ΣΦΑΛΜΑ: Αποτυχία κατεβάσματος!
    echo     Έλεγξε τη σύνδεση internet και δοκίμασε ξανά.
    echo.
    pause
    exit /b 1
)

echo.
echo       ✓ Το παιχνίδι κατέβηκε επιτυχώς!

:: ─────────────────────────────────────────────────────────────────────
:: ΒΗΜΑ 5: Εγκατάσταση Raylib
:: ─────────────────────────────────────────────────────────────────────
echo.
echo [5/6] Έλεγχος και εγκατάσταση Raylib...

set RAYLIB_HEADER=%INSTALL_PATH%\src\engine\raylib\include\raylib.h

if exist "%RAYLIB_HEADER%" (
    echo       ✓ Raylib βρέθηκε!
    goto :OPEN_VSCODE
)

echo       Raylib δεν βρέθηκε. Γίνεται αυτόματη εγκατάσταση...
echo.

:: Download Raylib
set RAYLIB_URL=https://github.com/raysan5/raylib/releases/download/5.0/raylib-5.0_win64_mingw-w64.zip
set RAYLIB_ZIP=%TEMP_SETUP%\raylib.zip
set RAYLIB_TARGET=%INSTALL_PATH%\src\engine\raylib

echo       Κατέβασμα Raylib 5.0...
powershell -Command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; $ProgressPreference = 'SilentlyContinue'; Invoke-WebRequest -Uri '%RAYLIB_URL%' -OutFile '%RAYLIB_ZIP%' -UseBasicParsing"

if not exist "%RAYLIB_ZIP%" (
    echo.
    echo  ❌ ΣΦΑΛΜΑ: Αποτυχία κατεβάσματος Raylib!
    echo     Κατέβασε χειροκίνητα από: https://github.com/raysan5/raylib/releases
    echo.
    pause
    exit /b 1
)

echo       Αποσυμπίεση Raylib...
powershell -Command "Expand-Archive -Path '%RAYLIB_ZIP%' -DestinationPath '%TEMP_SETUP%\raylib_extract' -Force"

:: Find extracted folder and copy files
echo       Αντιγραφή αρχείων Raylib...

:: Create target directories
if not exist "%RAYLIB_TARGET%\include" mkdir "%RAYLIB_TARGET%\include"
if not exist "%RAYLIB_TARGET%\lib" mkdir "%RAYLIB_TARGET%\lib"

:: Copy include and lib folders
powershell -Command "$src = Get-ChildItem -Path '%TEMP_SETUP%\raylib_extract' -Directory | Where-Object { $_.Name -like 'raylib*' } | Select-Object -First 1; Copy-Item -Path \"$($src.FullName)\include\*\" -Destination '%RAYLIB_TARGET%\include' -Recurse -Force; Copy-Item -Path \"$($src.FullName)\lib\*\" -Destination '%RAYLIB_TARGET%\lib' -Recurse -Force"

:: Verify installation
if not exist "%RAYLIB_HEADER%" (
    echo.
    echo  ❌ ΣΦΑΛΜΑ: Αποτυχία εγκατάστασης Raylib!
    echo.
    pause
    exit /b 1
)

echo       ✓ Raylib εγκαταστάθηκε επιτυχώς!

:: ─────────────────────────────────────────────────────────────────────
:: ΒΗΜΑ 6: Άνοιγμα στο VS Code
:: ─────────────────────────────────────────────────────────────────────
:OPEN_VSCODE
echo.
echo [6/6] Άνοιγμα στο VS Code...

:: Έλεγχος αν υπάρχει το VS Code
where code >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo.
    echo  ⚠ Το VS Code δεν βρέθηκε στο PATH.
    echo     Άνοιξε χειροκίνητα τον φάκελο:
    echo     %INSTALL_PATH%
    echo.
    explorer "%INSTALL_PATH%"
    pause
    exit /b 0
)

:: Άνοιγμα στο VS Code
code "%INSTALL_PATH%"

echo.
echo  ╔═══════════════════════════════════════════════════════════════╗
echo  ║  ✅ ΕΤΟΙΜΟ! Το παιχνίδι είναι έτοιμο!                         ║
echo  ╠═══════════════════════════════════════════════════════════════╣
echo  ║                                                               ║
echo  ║  📁 Φάκελος: %FOLDER_NAME%                                    ║
echo  ║                                                               ║
echo  ║  🎯 ΕΠΟΜΕΝΑ ΒΗΜΑΤΑ:                                           ║
echo  ║                                                               ║
echo  ║  1. Πήγαινε στον φάκελο src\student\                          ║
echo  ║                                                               ║
echo  ║  2. Ξεκίνα με το αρχείο: 01_Economy.cpp                       ║
echo  ║                                                               ║
echo  ║  3. Διάβασε τις οδηγίες και συμπλήρωσε τον κώδικα!            ║
echo  ║                                                               ║
echo  ║  4. Τρέξε: build_and_run.bat για να δεις το παιχνίδι          ║
echo  ║                                                               ║
echo  ╚═══════════════════════════════════════════════════════════════╝
echo.
echo  Καλή επιτυχία Νικόλα! 🎮
echo.
pause
