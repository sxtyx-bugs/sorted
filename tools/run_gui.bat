@echo off
title SORTED --- Desktop File Organizer
cd /d "%~dp0\.."
echo Starting SORTED Desktop Application (Qt 6)...
python app\main.py
if errorlevel 1 pause
