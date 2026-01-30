@echo off
echo [SYSTEM] Uploading Defense Algorithms...

:: 1. Add all changes
git add .

:: 2. Commit with timestamp
set "ts=%DATE% %TIME%"
git commit -m "Student Progress: %ts%"

:: 3. Push to student branch
git push origin student-nikolas

echo.
if %errorlevel% neq 0 (
    echo [ERROR] Upload failed! Check your internet connection.
) else (
    echo [SUCCESS] Progress Saved to Cloud!
)
pause
