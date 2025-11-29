<#
  scripts/windows/release_windows.ps1

  Release / install script.
  - Uses the same Qt config as build_windows.ps1
  - Locates the built .exe
  - Runs windeployqt into dist/windows
  - Optionally zips the dist folder

  Run this after build_windows.ps1.
#>

function Fail($msg) {
    Write-Host "ERROR: $msg" -ForegroundColor Red
    exit 1
}

Write-Host "==> Windows release script starting..." -ForegroundColor Cyan

# ------------------------------------------------------
# Configuration (must match build_windows.ps1)
# ------------------------------------------------------
# Get QT version
$RepoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$QtVersionFile = Join-Path $RepoRoot ".qt-version"

$QtVersion = Get-Content $QtVersionFile -Raw
$QtVersion = $QtVersion.Trim()

Write-Host "Using Qt version: $QtVersion"

$QtArch    = "win64_msvc2019_64"
$QtRoot    = "C:\Qt"

$RepoRoot  = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$BuildDir  = Join-Path $RepoRoot "build-windows"
$BuildType = "Release"
$ExeName   = "maki.exe"              # adjust if different

$DistDir   = Join-Path $RepoRoot "release\windows"
$QtBase    = Join-Path $QtRoot "$QtVersion\$QtArch"
$QtBin     = Join-Path $QtBase "bin"
$WindeployqtPath = Join-Path $QtBin "windeployqt.exe"

# ------------------------------------------------------
# Basic checks
# ------------------------------------------------------
if (-not (Test-Path $QtBin)) {
    Fail "Qt bin folder '$QtBin' not found. Run build.ps1 first or check Qt config."
}
if (-not (Test-Path $WindeployqtPath)) {
    Fail "windeployqt not found at '$WindeployqtPath'."
}
if (-not (Test-Path $BuildDir)) {
    Fail "Build directory '$BuildDir' not found. Run build.ps1 first."
}

$env:PATH = "$QtBin;$env:PATH"

# ------------------------------------------------------
# Locate the executable
# ------------------------------------------------------
$ExePath = Join-Path $BuildDir $ExeName
if (-not (Test-Path $ExePath)) {
    # Try config-specific subdir
    $ExePathConfig = Join-Path (Join-Path $BuildDir $BuildType) $ExeName
    if (Test-Path $ExePathConfig) {
        $ExePath = $ExePathConfig
    } else {
        Fail "Could not find executable '$ExeName' in '$BuildDir' or '$BuildDir\$BuildType'."
    }
}

Write-Host "Executable: $ExePath" -ForegroundColor Green

# ------------------------------------------------------
# Run windeployqt
# ------------------------------------------------------
if (-not (Test-Path $DistDir)) {
    New-Item -ItemType Directory -Path $DistDir -Force | Out-Null
}

Write-Host "==> Running windeployqt into '$DistDir'..." -ForegroundColor Cyan
& $WindeployqtPath `
    --release `
    --dir $DistDir `
    $ExePath `
    || Fail "windeployqt failed."

# Ensure exe itself is in the dist dir
Copy-Item $ExePath $DistDir -Force

Write-Host "==> Deployment complete." -ForegroundColor Green
Write-Host "Distribution folder: $DistDir"

# ------------------------------------------------------
# Optional: create a zip archive
# ------------------------------------------------------
$ZipPath = Join-Path $RepoRoot "dist\windows.zip"
Write-Host "==> Creating zip archive: $ZipPath" -ForegroundColor Cyan

if (Test-Path $ZipPath) {
    Remove-Item $ZipPath -Force
}

Add-Type -AssemblyName System.IO.Compression.FileSystem
[System.IO.Compression.ZipFile]::CreateFromDirectory($DistDir, $ZipPath)

Write-Host "==> Release artifacts ready:" -ForegroundColor Green
Write-Host "  - Folder: $DistDir"
Write-Host "  - Zip:    $ZipPath"
