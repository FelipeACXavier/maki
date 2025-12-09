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
# Install / ensure Chocolatey
# ------------------------------------------------------
LogDebug "==> Checking for Chocolatey..."
$choco = Get-Command choco -ErrorAction SilentlyContinue
if (-not $choco) {
    LogWarning "Chocolatey not found. Installing..."
    Set-ExecutionPolicy Bypass -Scope Process -Force
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
    try {
      Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
    } catch {
      Fail "Failed to install Chocolatey."
    }
} else {
    LogInfo "Chocolatey found."
}

# ------------------------------------------------------
# Install tools with Chocolatey
# ------------------------------------------------------
$packages = @(
    "cmake",
    "ninja"
)

LogDebug "==> Ensuring base tools are installed: $($packages -join ', ')"
foreach ($pkg in $packages) {
    LogDebug "  - $pkg"
    choco install $pkg -y --no-progress | Out-Null
}

LogInfo "==> Base tools installed/updated."

# ------------------------------------------------------
# Ensure aqtinstall is installed
# ------------------------------------------------------
LogDebug "==> Checking for Python $PythonVersion ..."

$PythonExists = Get-Command $Python -ErrorAction SilentlyContinue
if (-not $PythonExists) {
  LogWarning "==> Python $PythonVersion not found, installing it..."
  choco install python --version=$PythonVersion -y --no-progress | Out-Null

  if (Test-Path "$env:ChocolateyInstall\bin\refreshenv.cmd") {
    & "$env:ChocolateyInstall\bin\refreshenv.cmd"
  }
}

# Add both the binary dir and Scripts dir
$env:PATH = "$PythonHome;$PythonHome\Scripts;$env:PATH"

$PythonExists = Get-Command $Python -ErrorAction SilentlyContinue
if (-not $PythonExists) {
  Fail "Python not found on PATH even after installation. Reopen PowerShell and retry."
} else {
  LogInfo "==> Python $PythonVersion installed."
}

LogDebug "==> Ensuring aqtinstall is installed..."
LogDebug "==> Running with $Python"
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
