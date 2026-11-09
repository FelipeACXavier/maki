<#
  scripts/windows/setup_tools.ps1
  Ensures native build tools and MAKI's isolated Python build tools.
  Run from an elevated PowerShell.
#>
. (Join-Path $PSScriptRoot 'settings.ps1')

if ($env:OS -ne 'Windows_NT') { Fail 'This script must run on Windows.' }

# Chocolatey is only for native dependencies and an absent system Python.
if (-not (Get-Command choco.exe -ErrorAction SilentlyContinue)) {
    LogWarning 'Chocolatey not found. Installing...'
    Set-ExecutionPolicy Bypass -Scope Process -Force
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
    try {
        Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
    } catch {
        Fail "Failed to install Chocolatey: $_"
    }
    $env:PATH = "$(Join-Path $env:ProgramData 'chocolatey\bin');$env:PATH"
}
if (-not (Get-Command choco.exe -ErrorAction SilentlyContinue)) {
    Fail 'Chocolatey was not found after installation.'
}

foreach ($package in @('git', 'cmake', 'ninja')) {
    LogDebug "Ensuring $package is installed..."
    & choco.exe install $package -y --no-progress
    if ($LASTEXITCODE -notin @(0, 3010, 1641)) {
        Fail "Chocolatey failed to install $package (exit code $LASTEXITCODE)."
    }
}
# An install can change the registry PATH without updating this shell.
$env:PATH = "$(Join-Path $env:ProgramFiles 'Git\cmd');$env:PATH"

# The already-created venv can be reused; otherwise find or install base Python.
if (-not (Test-ToolsVenv)) {
    $basePython = Get-CompatiblePython
    if (-not $basePython) {
        LogWarning "No Python >= $MinimumPythonVersion found; installing Python 3.12..."
        & choco.exe install python312 -y --no-progress
        if ($LASTEXITCODE -notin @(0, 3010, 1641)) {
            Fail "Chocolatey failed to install Python 3.12 (exit code $LASTEXITCODE)."
        }
        $basePython = Get-CompatiblePython
        if (-not $basePython) {
            Fail 'Python installation completed, but no compatible interpreter was detected.'
        }
    }
    Initialize-ToolsVenv $basePython
} else {
    LogInfo "Reusing Python virtual environment: $ToolsVenv"
}

# Avoid --user: pip is now targeting the isolated environment.
LogDebug 'Installing aqtinstall into the MAKI tool environment...'
& $PythonExecutable -m pip install --upgrade aqtinstall
if ($LASTEXITCODE -ne 0) { Fail 'Unable to install aqtinstall into the virtual environment.' }
& $PythonExecutable -m aqt --version
if ($LASTEXITCODE -ne 0) { Fail 'aqtinstall could not be executed from the virtual environment.' }

$vcpkgExe = Join-Path $VcPkgPath 'vcpkg.exe'
if (-not (Test-Path -LiteralPath $vcpkgExe)) {
    if (-not (Test-Path -LiteralPath $VcPkgPath)) {
        LogDebug "Cloning vcpkg into $VcPkgPath ..."
        & git.exe clone https://github.com/microsoft/vcpkg.git $VcPkgPath
        if ($LASTEXITCODE -ne 0) { Fail 'Failed to clone vcpkg.' }
    }
    $bootstrap = Join-Path $VcPkgPath 'bootstrap-vcpkg.bat'
    if (-not (Test-Path -LiteralPath $bootstrap)) {
        Fail "Missing vcpkg bootstrap script: $bootstrap"
    }
    & $bootstrap
    if ($LASTEXITCODE -ne 0) { Fail 'Failed to bootstrap vcpkg.' }
}

LogInfo "Build tools ready. Python environment: $ToolsVenv"
exit 0
