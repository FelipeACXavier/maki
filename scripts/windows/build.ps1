<#
  scripts/windows/build_windows.ps1

  Build script (no packaging).
  - Ensures aqtinstall is available
  - Ensures Qt (with qtwebengine, qtpdf, qtpdfwidgets) is installed via aqt
  - Configures and builds using CMake + Ninja

  Run this in a Developer Command Prompt for VS, or a PowerShell
  where MSVC, CMake, Ninja are available on PATH.
#>

function Fail($msg) {
    Write-Host "ERROR: $msg" -ForegroundColor Red
    exit 1
}

Write-Host "==> Windows build script starting..." -ForegroundColor Cyan

# ------------------------------------------------------
# Configuration
# ------------------------------------------------------

# Get QT version
$RepoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$QtVersionFile = Join-Path $RepoRoot ".qt-version"

$QtVersion = Get-Content $QtVersionFile -Raw
$QtVersion = $QtVersion.Trim()

Write-Host "Using Qt version: $QtVersion"

# Where Qt should be installed
$QtArch    = "win64_msvc2022_64"    # adjust to your MSVC / MinGW choice
$QtRoot    = "C:\Qt"

# Assume this script lives in scripts/windows, repo root is two levels up
$RepoRoot  = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$BuildDir  = Join-Path $RepoRoot "build-windows"
$BuildType = "Release"

Write-Host "Repo root: $RepoRoot" -ForegroundColor Gray

# ------------------------------------------------------
# Ensure Python + aqtinstall
# ------------------------------------------------------
Write-Host "==> Checking for Python..." -ForegroundColor Cyan
$python = Get-Command python -ErrorAction SilentlyContinue
if (-not $python) {
    Fail "Python not found on PATH. Run setup_windows.ps1 first or install Python 3."
}

Write-Host "==> Ensuring aqtinstall is available..." -ForegroundColor Cyan
try {
    python -m aqt --help | Out-Null
} catch {
    Write-Host "aqtinstall not found. Installing via pip..." -ForegroundColor Yellow
    python -m pip install --user aqtinstall `
        || Fail "pip install aqtinstall failed."
}

# ------------------------------------------------------
# Ensure Qt is installed
# ------------------------------------------------------
$QtBase = Join-Path $QtRoot "$QtVersion\$QtArch"
$QtBin  = Join-Path $QtBase "bin"
$WindeployqtPath = Join-Path $QtBin "windeployqt.exe"

if (Test-Path $WindeployqtPath) {
    Write-Host "==> Qt already installed at $QtBase" -ForegroundColor Green
} else {
    Write-Host "==> Installing Qt $QtVersion ($QtArch) with aqt..." -ForegroundColor Cyan

    if (-not (Test-Path $QtRoot)) {
        New-Item -ItemType Directory -Path $QtRoot | Out-Null
    }

    python -m aqt install-qt windows desktop $QtVersion $QtArch `
        -O $QtRoot `
        -m qtwebengine qtpdf qtpdfwidgets `
        || Fail "aqtinstall Qt installation failed."

    if (-not (Test-Path $WindeployqtPath)) {
        Fail "Qt installation finished, but windeployqt.exe not found at $WindeployqtPath."
    }
}

Write-Host "Qt base: $QtBase" -ForegroundColor Green
$env:PATH = "$QtBin;$env:PATH"

# ------------------------------------------------------
# Configure & build with CMake
# ------------------------------------------------------
if (-not (Test-Path $RepoRoot)) {
    Fail "Repo root '$RepoRoot' does not exist."
}

Write-Host "==> Configuring CMake..." -ForegroundColor Cyan
if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

cmake -S $RepoRoot -B $BuildDir -G "Ninja" `
    -DCMAKE_BUILD_TYPE=$BuildType `
    -DCMAKE_PREFIX_PATH=$QtBase `
    || Fail "CMake configure failed."

Write-Host "==> Building ($BuildType)..." -ForegroundColor Cyan
cmake --build $BuildDir --config $BuildType `
    || Fail "Build failed."

Write-Host "==> build_windows.ps1 finished successfully." -ForegroundColor Green
Write-Host "Build directory: $BuildDir"
