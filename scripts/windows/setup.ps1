<#
  scripts/windows/setup_windows.ps1

  One-time (or occasional) setup script.
  - Ensures Chocolatey is installed
  - Installs Git, CMake, Ninja, Python 3
  - Checks for a C++ compiler (MSVC) and warns if missing

  Run this in an elevated PowerShell (Run as Administrator).
#>

# ------------------------------------------------------
# Load shared settings
# ------------------------------------------------------
$RepoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$SettingsPath = Join-Path $RepoRoot "scripts\windows\settings.ps1"
. $SettingsPath

LogDebug "==> Windows setup script starting..."

# ------------------------------------------------------
# Ensure script is running on Windows
# ------------------------------------------------------
if ($PSVersionTable.PSEdition -ne "Desktop" -and $env:OS -notlike "*Windows*") {
    Fail "This script must be run on Windows."
}

# ------------------------------------------------------
# Ensure aqtinstall is installed
# ------------------------------------------------------
# Add both the binary dir and Scripts dir
$env:PATH = "$PythonHome;$PythonHome\Scripts;$env:PATH"

LogDebug "==> Running with $Python"
LogDebug "==> Ensuring aqtinstall is installed..."
try {
  & $Python -m pip install --user aqtinstall
} catch {
  Fail "Failed to install aqtinstall."
}

# ------------------------------------------------------
# Install Qt via aqtinstall
# ------------------------------------------------------
LogDebug "Using Qt version: $QtVersion"

if (Test-Path $WindeployqtPath) {
    LogDebug "==> Qt already installed at: $QtBase"
} else {
    LogDebug "==> Installing Qt $QtVersion ($QtArch)..."

    if (-not (Test-Path $QtRoot)) {
        New-Item -ItemType Directory -Path $QtRoot | Out-Null
    }

    try {
      aqt install-qt windows desktop $QtVersion $QtArch `
          -m qtwebengine qtpdf qtwebchannel qtpositioning `
          -O $QtRoot `
    } catch {
      Fail "Qt installation failed."
    }

    if (-not (Test-Path $WindeployqtPath)) {
        Fail "Qt installation completed but windeployqt.exe not found. Something went wrong."
    }
}

$env:PATH = "$QtBin;$env:PATH"
LogInfo "Qt successfully installed at: $QtBase"


# ------------------------------------------------------
# Check for C++ compiler (MSVC)
# ------------------------------------------------------
LogDebug "==> Checking for MSVC (cl.exe)..."
$cl = Get-Command cl.exe -ErrorAction SilentlyContinue
if (-not $cl) {
    LogWarning @"
WARNING: 'cl.exe' (MSVC) not found on PATH.
You likely need to install:
  - Visual Studio with "Desktop development with C++"
    OR
  - Microsoft Build Tools for Visual Studio (C++)

This script does not install Visual Studio automatically.
"@
} else {
    LogDebug "MSVC compiler detected: $($cl.Source)"
}

LogInfo "==> setup.ps1 finished."
LogDebug "You can now run build.ps1 (after ensuring MSVC is installed)."
