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

LogDebug "  - python"
choco install python --version=$PythonVersion -y --no-progress | Out-Null

LogInfo "==> Base tools installed/updated."

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

exit 0