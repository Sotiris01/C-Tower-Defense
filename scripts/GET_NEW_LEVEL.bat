@echo off
echo [SYSTEM] Connecting to Planetary Defense HQ...
echo [ACTION] Downloading new mission data...

:: Ensure we are on the student branch
git checkout student-nikolas

:: Pull changes from main (Teacher's updates)
:: --no-edit accepts the default merge message
git pull origin main --no-edit

echo.
if %errorlevel% neq 0 (
    echo [ERROR] Connection failed or conflict detected!
    echo [INFO] Ask the Chief Commander (Teacher) for help.
) else (
    echo [SUCCESS] Mission Data Updated!
    echo [INFO] You are ready to code.
)
pause
