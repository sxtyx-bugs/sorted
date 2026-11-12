# ==============================================================================
# packaging/windows/package_windows.ps1 — Package SORTED for Windows distribution
# ==============================================================================

param (
    [string]$BuildDir = "build",
    [string]$OutputDir = "dist/windows"
)

$ErrorActionPreference = "Stop"

Write-Host "=== SORTED Windows Deployment & Packaging ===" -ForegroundColor Cyan

# Locate executable
$ExeCandidates = @(
    "$BuildDir/SORTED.exe",
    "$BuildDir/bin/SORTED.exe",
    "$BuildDir/Release/SORTED.exe"
)

$ExePath = $null
foreach ($c in $ExeCandidates) {
    if (Test-Path $c) {
        $ExePath = (Resolve-Path $c).Path
        break
    }
}

if (-not $ExePath) {
    Write-Warning "SORTED.exe not found in $BuildDir. Ensure the project is built before packaging."
    exit 1
}

Write-Host "Discovered executable: $ExePath" -ForegroundColor Green

# Prepare staging directory
$StageDir = "$OutputDir/SORTED"
if (Test-Path $StageDir) {
    Remove-Item -Recurse -Force $StageDir
}
New-Item -ItemType Directory -Force -Path $StageDir | Out-Null
New-Item -ItemType Directory -Force -Path "$StageDir/resources/icons" | Out-Null

# Copy binary & assets
Copy-Item $ExePath "$StageDir/SORTED.exe"
if (Test-Path "resources/icons/SORTED.ico") {
    Copy-Item "resources/icons/SORTED.ico" "$StageDir/resources/icons/SORTED.ico"
}
if (Test-Path "LICENSE") {
    Copy-Item "LICENSE" "$StageDir/LICENSE"
}

# Run windeployqt
$windeployqt = Get-Command "windeployqt.exe" -ErrorAction SilentlyContinue
if ($windeployqt) {
    Write-Host "Running windeployqt..." -ForegroundColor Yellow
    & $windeployqt.Source --no-translations --compiler-runtime "$StageDir/SORTED.exe"
} else {
    Write-Host "Notice: windeployqt not found in PATH; skipping automated Qt DLL bundling." -ForegroundColor Yellow
}

# Create Portable ZIP Package
$ZipOutput = "$OutputDir/SORTED-1.0.0-windows-x64.zip"
Write-Host "Creating portable archive: $ZipOutput" -ForegroundColor Cyan
if (Test-Path $ZipOutput) { Remove-Item -Force $ZipOutput }
Compress-Archive -Path "$StageDir/*" -DestinationPath $ZipOutput -CompressionLevel Optimal

# Check for Inno Setup compiler to create setup installer
$isccCandidates = @(
    "iscc.exe",
    "${env:ProgramFiles(x86)}\Inno Setup 6\iscc.exe",
    "${env:ProgramFiles}\Inno Setup 6\iscc.exe"
)

$isccPath = $null
foreach ($path in $isccCandidates) {
    if (Get-Command $path -ErrorAction SilentlyContinue) {
        $isccPath = (Get-Command $path).Source
        break
    } elseif (Test-Path $path) {
        $isccPath = $path
        break
    }
}

if ($isccPath) {
    Write-Host "Compiling Inno Setup installer using $isccPath..." -ForegroundColor Cyan
    & $isccPath "packaging/windows/installer.iss"
    Write-Host "Installer compilation completed successfully." -ForegroundColor Green
} else {
    Write-Host "Notice: Inno Setup compiler (iscc.exe) not found. Skipping Setup.exe compilation (portable zip created)." -ForegroundColor Yellow
}

Write-Host "=== Windows packaging complete ===" -ForegroundColor Green
Get-ChildItem $OutputDir | Select-Object Name, Length, LastWriteTime
