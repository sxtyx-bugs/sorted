@echo off
title Publish SORTED v1.0.0 Release to GitHub
cd /d "%~dp0"

echo =======================================================
echo   Publishing SORTED v1.0.0 to GitHub Actions
echo =======================================================
echo.
echo 1. Pushing 'main' branch (registers release workflow on GitHub)...
git push origin main
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Failed to push main branch. Check GitHub authentication.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo 2. Pushing 'v1.0.0' release tag to trigger multi-platform build...
git tag -f v1.0.0
git push origin -f v1.0.0
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Failed to push release tag v1.0.0.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo =======================================================
echo   SUCCESS! GitHub Actions build has been triggered!
echo.
echo   - Track build progress:
echo     https://github.com/sxtyx-bugs/sorted/actions
echo.
echo   - View your releases when complete:
echo     https://github.com/sxtyx-bugs/sorted/releases
echo =======================================================
pause
