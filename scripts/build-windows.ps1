# ==============================================================================
# scripts/build-windows.ps1 — Configure, build, and test SORTED on Windows
# ==============================================================================

param (
    [string]$BuildDir = "build",
    [string]$Config = "Release",
    [switch]$Package
)

$ErrorActionPreference = "Stop"

Write-Host "=== Building SORTED on Windows ===" -ForegroundColor Cyan
Write-Host "Build Directory: $BuildDir"
Write-Host "Configuration:   $Config"

# Add MinGW toolchain to PATH if available in standard location
$mingwPath = "C:\QtTools\Tools\mingw1310_64\bin"
if (Test-Path $mingwPath) {
    if (-not ($env:PATH -like "*$mingwPath*")) {
        $env:PATH = "$mingwPath;" + $env:PATH
        Write-Host "Added MinGW toolchain to PATH: $mingwPath" -ForegroundColor Gray
    }
}

# Configure CMake
Write-Host "--> Configuring CMake..." -ForegroundColor Yellow
cmake -B $BuildDir -DCMAKE_BUILD_TYPE=$Config

# Build
Write-Host "--> Compiling targets..." -ForegroundColor Yellow
cmake --build $BuildDir --config $Config --parallel

# Run tests
Write-Host "--> Running verification tests..." -ForegroundColor Yellow
ctest --test-dir $BuildDir --output-on-failure

Write-Host "=== Build Completed Successfully ===" -ForegroundColor Green

if ($Package) {
    Write-Host "--> Triggering Windows packaging..." -ForegroundColor Cyan
    & powershell -ExecutionPolicy Bypass -File "packaging/windows/package_windows.ps1" -BuildDir $BuildDir
}
