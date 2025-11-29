<#
  scripts/windows/setup_windows.ps1

  One-time (or occasional) setup script.
  - Ensures Chocolatey is installed
  - Installs Git, CMake, Ninja, Python 3
  - Checks for a C++ compiler (MSVC) and warns if missing

  Run this in an elevated PowerShell (Run as Administrator).
#>

function Fail($msg) {
    Write-Host "ERROR: $msg" -ForegroundColor Red
    exit 1
}

Write-Host "==> Windows setup script starting..." -ForegroundColor Cyan

# ------------------------------------------------------
# Ensure script is running on Windows
# ------------------------------------------------------
if ($PSVersionTable.PSEdition -ne "Desktop" -and $env:OS -notlike "*Windows*") {
    Fail "This script must be run on Windows."
}

# ------------------------------------------------------
# Install / ensure Chocolatey
# ------------------------------------------------------
Write-Host "==> Checking for Chocolatey..." -ForegroundColor Cyan
$choco = Get-Command choco -ErrorAction SilentlyContinue
if (-not $choco) {
    Write-Host "Chocolatey not found. Installing..." -ForegroundColor Yellow
    Set-ExecutionPolicy Bypass -Scope Process -Force
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
    Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1')) `
        || Fail "Failed to install Chocolatey."
} else {
    Write-Host "Chocolatey found." -ForegroundColor Green
}

# ------------------------------------------------------
# Install tools with Chocolatey
# ------------------------------------------------------
$packages = @(
    "git",
    "cmake",
    "ninja",
    "python3"
)

Write-Host "==> Ensuring base tools are installed: $($packages -join ', ')" -ForegroundColor Cyan
foreach ($pkg in $packages) {
    Write-Host "  - $pkg" -ForegroundColor Gray
    choco install $pkg -y --no-progress | Out-Null
}

Write-Host "==> Base tools installed/updated." -ForegroundColor Green

# ------------------------------------------------------
# Ensure aqtinstall is installed
# ------------------------------------------------------
Write-Host "==> Checking for Python..." -ForegroundColor Cyan
$python = Get-Command python -ErrorAction SilentlyContinue
if (-not $python) {
    Fail "Python not found on PATH even after installation. Reopen PowerShell and retry."
}

Write-Host "==> Ensuring aqtinstall is installed..." -ForegroundColor Cyan
try {
    python -m aqt --help | Out-Null
} catch {
    Write-Host "aqtinstall not found, installing..." -ForegroundColor Yellow
    python -m pip install --user aqtinstall ` || Fail "Failed to install aqtinstall."
}

# ------------------------------------------------------
# Install Qt via aqtinstall
# ------------------------------------------------------
# Get QT version
$RepoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$QtVersionFile = Join-Path $RepoRoot ".qt-version"

$QtVersion = Get-Content $QtVersionFile -Raw
$QtVersion = $QtVersion.Trim()

Write-Host "Using Qt version: $QtVersion"

$QtArch    = "win64_msvc2022_64"
$QtRoot    = "C:\Qt"
$QtBase    = Join-Path $QtRoot "$QtVersion\$QtArch"
$QtBin  = Join-Path $QtBase "bin"
$WindeployqtPath = Join-Path $QtBin "windeployqt.exe"

if (Test-Path $WindeployqtPath) {
    Write-Host "==> Qt already installed at: $QtBase" -ForegroundColor Green
} else {
    Write-Host "==> Installing Qt $QtVersion ($QtArch)..." -ForegroundColor Cyan

    if (-not (Test-Path $QtRoot)) {
        New-Item -ItemType Directory -Path $QtRoot | Out-Null
    }

    aqt install-qt windows desktop $QtVersion $QtArch `
        -m qtwebengine qtpdf qtwebchannel qtpositioning `
        -O $QtRoot `
        || Fail "Qt installation failed."

    if (-not (Test-Path $WindeployqtPath)) {
        Fail "Qt installation completed but windeployqt.exe not found. Something went wrong."
    }
}

Write-Host "Qt successfully installed at: $QtBase" -ForegroundColor Green

$env:PATH = "$QtBin;$env:PATH"

# ------------------------------------------------------
# Check for C++ compiler (MSVC)
# ------------------------------------------------------
Write-Host "==> Checking for MSVC (cl.exe)..." -ForegroundColor Cyan
$cl = Get-Command cl.exe -ErrorAction SilentlyContinue
if (-not $cl) {
    Write-Host @"
WARNING: 'cl.exe' (MSVC) not found on PATH.
You likely need to install:
  - Visual Studio with "Desktop development with C++"
    OR
  - Microsoft Build Tools for Visual Studio (C++)

This script does not install Visual Studio automatically.
"@ -ForegroundColor Yellow
} else {
    Write-Host "MSVC compiler detected: $($cl.Source)" -ForegroundColor Green
}

Write-Host "==> setup_windows.ps1 finished." -ForegroundColor Cyan
Write-Host "You can now run build.ps1 (after ensuring MSVC is installed)." -ForegroundColor Cyan
