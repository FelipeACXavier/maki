<#
  scripts/windows/release_windows.ps1

  Release / install script.
  - Uses the same Qt config as build_windows.ps1
  - Locates the built .exe
  - Runs windeployqt into dist/windows
  - Optionally zips the dist folder

  Run this after build_windows.ps1.
#>

# ------------------------------------------------------
# Load shared settings
# ------------------------------------------------------
$RepoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$SettingsPath = Join-Path $RepoRoot "scripts\windows\settings.ps1"
. $SettingsPath

Write-Host "==> Windows release script starting..." -ForegroundColor Cyan

LogInfo "Using Qt version: $QtVersion"
LogDebug "Repo root: $RepoRoot"

# ------------------------------------------------------
# Configuration
# ------------------------------------------------------
$ExeName   = "maki.exe"

# ------------------------------------------------------
# Basic checks
# ------------------------------------------------------
if (-not (Test-Path $QtBin)) {
    Fail "Qt bin folder '$QtBin' not found. Run build.ps1 first or check Qt config."
}
if (-not (Test-Path $WindeployqtPath)) {
    Fail "windeployqt not found at '$WindeployqtPath'."
}
if (-not (Test-Path $BuildPath)) {
    Fail "Build directory '$BuildPath' not found. Run build.ps1 first."
}

$env:PATH = "$QtBin;$env:PATH"

# ------------------------------------------------------
# Locate the executable
# ------------------------------------------------------
$ExePath = Join-Path $BuildPath $ExeName
if (-not (Test-Path $ExePath)) {
    # Try config-specific subdir
    $ExePathConfig = Join-Path (Join-Path $BuildPath $BuildType) $ExeName
    if (Test-Path $ExePathConfig) {
        $ExePath = $ExePathConfig
    } else {
        Fail "Could not find executable '$ExeName' in '$BuildPath' or '$BuildPath\$BuildType'."
    }
}

Write-Host "Executable: $ExePath" -ForegroundColor Green

# ------------------------------------------------------
# Run windeployqt
# ------------------------------------------------------
if (-not (Test-Path $InstallPath)) {
    New-Item -ItemType Directory -Path $InstallPath -Force | Out-Null
}

Write-Host "==> Running windeployqt into '$InstallPath'..." -ForegroundColor Cyan
try {
  cmake --build "$BuildPath" `
        --config "$BuildType" `
        --parallel 4 `
        --target deploy-windows
} catch {
  Fail "Build failed."
}

# Ensure exe itself is in the dist dir
Copy-Item $ExePath $InstallPath -Force

Write-Host "==> Deployment complete." -ForegroundColor Green
Write-Host "Distribution folder: $InstallPath"

# ------------------------------------------------------
# Create a zip archive
# ------------------------------------------------------
$ZipPath = Join-Path $RepoRoot "release\windows.zip"
Write-Host "==> Creating zip archive: $ZipPath" -ForegroundColor Cyan

if (Test-Path $ZipPath) {
    Remove-Item $ZipPath -Force
}

Add-Type -AssemblyName System.IO.Compression.FileSystem
[System.IO.Compression.ZipFile]::CreateFromDirectory($InstallPath, $ZipPath)

Write-Host "==> Release artifacts ready:" -ForegroundColor Green
Write-Host "  - Folder: $InstallPath"
Write-Host "  - Zip:    $ZipPath"
exit 0