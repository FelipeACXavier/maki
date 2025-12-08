<#
  scripts/windows/build_windows.ps1

  Build script (no packaging).
  - Ensures aqtinstall is available
  - Ensures Qt (with qtwebengine, qtpdf, qtpdfwidgets) is installed via aqt
  - Configures and builds using CMake + Ninja

  Run this in a Developer Command Prompt for VS, or a PowerShell
  where MSVC, CMake, Ninja are available on PATH.
#>

# ------------------------------------------------------
# Load shared settings
# ------------------------------------------------------
$RepoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$SettingsPath = Join-Path $RepoRoot "scripts\windows\settings.ps1"
. $SettingsPath

LogInfo "==> Windows build script starting..."

# ------------------------------------------------------
# Configuration
# ------------------------------------------------------
LogInfo "Using Qt version: $QtVersion"
LogDebug "Repo root: $RepoRoot"

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

try {
  cmake -S "$RepoRoot" -B "$BuildPath" `
    -DDEPLOY_TARGET="windows" `
    -DCMAKE_PREFIX_PATH="$QtBase" `
    -DCMAKE_INSTALL_PREFIX="$InstallPath" `
} catch {
  Fail "CMake configure failed."
}

LogDebug "==> Building ($BuildType)..."
try {
  cmake --build "$BuildPath" `
        --config Release `
        --parallel 4
} catch {
  Fail "Build failed."
}

LogInfo "==> build_windows.ps1 finished successfully." -ForegroundColor Green
LogInfo "Build directory: $BuildPath"
