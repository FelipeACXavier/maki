<#
  scripts/windows/setup.ps1
  Windows setup orchestrator. Run in an elevated PowerShell.
#>
. (Join-Path $PSScriptRoot 'settings.ps1')

if ($env:OS -ne 'Windows_NT') { Fail 'This script must run on Windows.' }

# Tool setup creates build/.venv-tools and installs aqtinstall into it.
& (Join-Path $PSScriptRoot 'setup_tools.ps1')
if ($LASTEXITCODE -ne 0) { Fail 'Tool setup failed.' }

if (-not (Test-ToolsVenv)) {
    Fail "Python tools virtual environment is unavailable: $ToolsVenv"
}
LogInfo "Using isolated Python: $PythonExecutable"

if (Test-Path -LiteralPath $WindeployqtPath) {
    LogInfo "Qt already installed: $QtBase"
} else {
    LogDebug "Installing Qt $QtVersion ($QtArch)..."
    New-Item -ItemType Directory -Path $QtRoot -Force | Out-Null
    & $PythonExecutable -m aqt install-qt windows desktop $QtVersion $QtArch `
        -m qt5compat qtpdf qtwebchannel qtpositioning qtwebsockets `
        -O $QtRoot
    if ($LASTEXITCODE -ne 0) { Fail 'Qt installation failed.' }
    if (-not (Test-Path -LiteralPath $WindeployqtPath)) {
        Fail 'Qt installation completed but windeployqt.exe was not found.'
    }
}
$env:PATH = "$QtBin;$env:PATH"
LogInfo "Qt ready: $QtBase"

$compiler = Get-Command cl.exe -ErrorAction SilentlyContinue
if ($compiler) {
    LogInfo "MSVC compiler detected: $($compiler.Source)"
} else {
    LogWarning 'MSVC not detected. Install Visual Studio Build Tools with Desktop development with C++.'
}
LogInfo 'Windows setup completed.'
exit 0
