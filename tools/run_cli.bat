@echo off
title SORTED --- Algorithm Demonstration
cd /d "%~dp0\.."
echo Running SORTED on test workspace (tests/fixtures/SORTED_TEST)...
python -m core.sorted_cli tests\fixtures\SORTED_TEST
echo.
pause
