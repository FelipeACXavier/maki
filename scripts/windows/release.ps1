<#
  scripts/windows/release_windows.ps1

  Release / install script.
  - Uses the same Qt config as build_windows.ps1
  - Locates the built .exe
  - Runs windeployqt into dist/windows
  - Optionally zips the dist folder

  Run this after build_windows.ps1.
#>

param(
  [switch]$Release
)

# ------------------------------------------------------
# Load shared settings
# ------------------------------------------------------
$RepoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$SettingsPath = Join-Path $RepoRoot "scripts\windows\settings.ps1"
. $SettingsPath

$BuildType = if ($Release) { "Release" } else { "Debug" }
# Match build.ps1: CMake build tree is build/windows/<Config> (not build/windows alone)
$CMakeBuildDir = Join-Path $BuildPath $BuildType

Write-Host "==> Windows release script starting..." -ForegroundColor Cyan

LogInfo "Using Qt version: $QtVersion"
LogDebug "Repo root: $RepoRoot"
LogDebug "CMake build dir: $CMakeBuildDir"

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
if (-not (Test-Path $CMakeBuildDir)) {
    Fail "Build directory '$CMakeBuildDir' not found. Run scripts\windows\build.ps1 first."
}

$env:PATH = "$QtBin;$env:PATH"

# ------------------------------------------------------
# Locate the executable (POST_BUILD copies to <build>/<Config>/bin/)
# ------------------------------------------------------
$ExeCandidates = @(
  (Join-Path $CMakeBuildDir "bin\$ExeName")
  (Join-Path $CMakeBuildDir $ExeName)
  (Join-Path $BuildPath $ExeName)
)
$ExePath = $ExeCandidates | Where-Object { Test-Path -LiteralPath $_ } | Select-Object -First 1
if (-not $ExePath) {
    Fail "Could not find '$ExeName'. Tried: $($ExeCandidates -join ', ')."
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
  cmake --build "$CMakeBuildDir" `
        --config "$BuildType" `
        --parallel 4 `
        --target deploy-windows
} catch {
  Fail "Build failed."
}
if ($LASTEXITCODE -ne 0) {
  Fail "cmake --build deploy-windows failed (exit $LASTEXITCODE)."
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