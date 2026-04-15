<#
  scripts/windows/build_windows.ps1

  Build script (no packaging).
  - Ensures aqtinstall is available
  - Ensures Qt (with qtwebengine, qtpdf, qtpdfwidgets) is installed via aqt
  - Configures and builds using CMake + Ninja

  Run this in a Developer Command Prompt for VS, or a PowerShell
  where MSVC, CMake, Ninja are available on PATH.
#>

param(
  [switch]$Clean,
  [switch]$Release
)

# ------------------------------------------------------
# Load shared settings
# ------------------------------------------------------
$RepoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$SettingsPath = Join-Path $RepoRoot "scripts\windows\settings.ps1"
. $SettingsPath

$BuildType = if ($Release) { "Release" } else { "Debug" }
$BuildPath = Join-Path $BuildPath "$BuildType"

LogDebug "RepoRoot: $RepoRoot"
LogDebug "BuildPath: $BuildPath"
LogDebug "QtBase: $QtBase"
LogDebug "InstallPath: $InstallPath"

if ($Clean) {
  Write-Host "Cleaning build directory: $BuildPath"
  if (Test-Path $BuildPath) {
    Remove-Item -Recurse -Force $BuildPath
  }
  exit 0
}

LogInfo "==> Windows build script starting..."

# ------------------------------------------------------
# Configuration
# ------------------------------------------------------
LogInfo "Using Qt version: $QtVersion"
LogDebug "Repo root: $RepoRoot"

EnsureWindowsQt

# CMake / vcpkg: explicit Qt6_DIR avoids find_package missing system Qt
$Qt6DirForCmake = ((Join-Path $QtBase "lib\cmake\Qt6") -replace '\\', '/')

# Koda plugin: use its vcpkg.json so antlr4 installs to 3rdparty/plugins/koda/vcpkg_installed
$KodaVcpkgManifest = (Join-Path $RepoRoot "3rdparty\plugins\koda")
$KodaVcpkgManifestForCmake = ($KodaVcpkgManifest -replace '\\', '/')

# ------------------------------------------------------
# Configure & build with CMake
# ------------------------------------------------------
if (-not (Test-Path $RepoRoot)) {
    Fail "Repo root '$RepoRoot' does not exist."
}

LogDebug "==> Configuring CMake..."
if (-not (Test-Path $BuildPath)) {
    New-Item -ItemType Directory -Path $BuildPath | Out-Null
}

cmake -S "$RepoRoot" -B "$BuildPath" `
  -A x64 `
  -DDEPLOY_TARGET="windows" `
  -DCMAKE_PREFIX_PATH="$QtBase" `
  -DQt6_DIR="$Qt6DirForCmake" `
  -DVCPKG_MANIFEST_DIR="$KodaVcpkgManifestForCmake" `
  -DCMAKE_INSTALL_PREFIX="$InstallPath" `
  -DCMAKE_BUILD_TYPE="$BuildType" `
  -DCMAKE_TOOLCHAIN_FILE="$VcPkgPath\scripts\buildsystems\vcpkg.cmake"

if ($LASTEXITCODE -ne 0) {
  Fail "CMake configure failed."
}

LogDebug "==> Building ($BuildType)..."
if ($Release) {
  cmake --build "$BuildPath" `
        --config $BuildType `
        --parallel 4 `
        --target deploy-windows
} else {
  cmake --build "$BuildPath" `
        --config $BuildType `
        --parallel 4
}


if ($LASTEXITCODE -ne 0) {
  Fail "CMake build failed."
}

LogInfo "==> build_windows.ps1 finished successfully."
LogInfo "Build directory: $BuildPath"
exit 0
