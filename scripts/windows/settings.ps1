<#
  scripts/windows/settings.ps1
  Shared configuration and helpers for Windows setup/build scripts.
  Dot-source this file: . "$PSScriptRoot\settings.ps1"
#>

$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$QtVersion = (Get-Content (Join-Path $RepoRoot '.qt-version') -Raw).Trim()

$QtArchPath      = 'msvc2022_64'
$QtArch          = "win64_$QtArchPath"
$QtRoot          = 'C:\Qt'
$QtBase          = Join-Path $QtRoot "$QtVersion\$QtArchPath"
$QtBin           = Join-Path $QtBase 'bin'
$WindeployqtPath = Join-Path $QtBin 'windeployqt.exe'

$VcPkgPath           = 'C:\vcpkg'
$MinimumPythonVersion = [version]'3.12'
$ToolsVenv           = Join-Path $RepoRoot 'build\.venv-tools'
$PythonExecutable    = Join-Path $ToolsVenv 'Scripts\python.exe'

$BuildPath   = Join-Path $RepoRoot 'build\windows'
$InstallPath = Join-Path $RepoRoot 'release\windows'
$BuildType   = 'Debug'

function LogInfo($msg)    { Write-Host "INFO: $msg" -ForegroundColor Green }
function LogDebug($msg)   { Write-Host "DEBUG: $msg" -ForegroundColor Cyan }
function LogTrace($msg)   { Write-Host "TRACE: $msg" -ForegroundColor Gray }
function LogWarning($msg) { Write-Host "WARNING: $msg" -ForegroundColor Yellow }
function Fail($msg)       { Write-Host "ERROR: $msg" -ForegroundColor Red; exit 1 }

function Get-CompatiblePython {
    # First ask the Windows Python launcher, which knows about registered installs.
    $candidates = @()
    if (Get-Command py.exe -ErrorAction SilentlyContinue) {
        $candidates += ,@('py.exe', '-3')
    }
    foreach ($name in @('python.exe', 'python3.exe')) {
        if (Get-Command $name -ErrorAction SilentlyContinue) {
            $candidates += ,@($name)
        }
    }
    # Support standard non-PATH installations too.
    $patterns = @('C:\Python3*\python.exe')
    if ($env:LOCALAPPDATA) {
        $patterns += (Join-Path $env:LOCALAPPDATA 'Programs\Python\Python3*\python.exe')
    }
    if ($env:ProgramFiles) {
        $patterns += (Join-Path $env:ProgramFiles 'Python3*\python.exe')
    }
    foreach ($pattern in $patterns) {
        foreach ($file in @(Get-ChildItem -Path $pattern -ErrorAction SilentlyContinue)) {
            $candidates += ,@($file.FullName)
        }
    }

    $probe = 'import sys; print(sys.executable if sys.version_info >= (3, 12) else "")'
    foreach ($candidate in $candidates) {
        $cmd = $candidate[0]
        $args = @()
        if ($candidate.Count -gt 1) { $args = @($candidate[1..($candidate.Count - 1)]) }
        try {
            $found = & $cmd @args '-c' $probe 2>$null
            if ($LASTEXITCODE -eq 0 -and $found) {
                $path = "$(($found | Select-Object -Last 1))".Trim()
                if ($path -and (Test-Path -LiteralPath $path -PathType Leaf)) {
                    return $path
                }
            }
        } catch {
            # Ignore stale launcher registrations and Windows Store aliases.
        }
    }
    return $null
}

function Test-ToolsVenv {
    if (-not (Test-Path -LiteralPath $PythonExecutable -PathType Leaf)) {
        return $false
    }
    try {
        # Verify this is an isolated venv and its original Python is still available.
        $probe = 'import sys; print(int(sys.prefix != sys.base_prefix and sys.version_info >= (3, 12)))'
        $result = & $PythonExecutable -c $probe 2>$null
        return ($LASTEXITCODE -eq 0 -and "$result".Trim() -eq '1')
    } catch {
        return $false
    }
}

function Initialize-ToolsVenv([string]$BasePython) {
    if (Test-ToolsVenv) {
        LogInfo "Reusing Python virtual environment: $ToolsVenv"
        return
    }
    if (-not $BasePython -or -not (Test-Path -LiteralPath $BasePython -PathType Leaf)) {
        Fail 'No compatible system Python executable available to create the tool environment.'
    }
    if (Test-Path -LiteralPath $ToolsVenv) {
        LogWarning 'Existing tools virtual environment is unusable; recreating it.'
        Remove-Item -LiteralPath $ToolsVenv -Recurse -Force -ErrorAction Stop
    }
    $parent = Split-Path -Parent $ToolsVenv
    New-Item -ItemType Directory -Path $parent -Force | Out-Null
    LogDebug "Creating Python virtual environment with $BasePython ..."
    & $BasePython -m venv $ToolsVenv
    if ($LASTEXITCODE -ne 0 -or -not (Test-ToolsVenv)) {
        Fail 'Unable to create the Python tools virtual environment.'
    }
    LogInfo "Python tools virtual environment created: $ToolsVenv"
}

# Existing Qt may be used by build scripts; no Python activation is needed.
if (Test-Path -LiteralPath $QtBin) {
    $env:PATH = "$QtBin;$env:PATH"
}

# Import MSVC environment only if the C++ toolchain is installed.
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
if (Test-Path -LiteralPath $vswhere) {
    $vsInstall = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if ($vsInstall) {
        $vcvars = Join-Path $vsInstall 'VC\Auxiliary\Build\vcvars64.bat'
        if (Test-Path -LiteralPath $vcvars) {
            cmd.exe /c "`"$vcvars`" >nul && set" | ForEach-Object {
                if ($_ -match '^(.*?)=(.*)$') {
                    Set-Item -Path "Env:$($matches[1])" -Value $matches[2]
                }
            }
        }
    }
}
