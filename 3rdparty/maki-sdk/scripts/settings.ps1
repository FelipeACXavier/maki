<#
  scripts/windows/settings.ps1

  Contains all the shared variables needed for the setup, build and release scripts
  Run this as .\settings.ps1
#>

# ------------------------------------------------------
# Variables
$RepoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..\..")
$QtVersion = "6.8.3"

$QtArchPath       = "msvc2022_64"
$QtArch           = "win64_$QtArchPath"
$QtRoot           = "C:\Qt"
$QtBase           = Join-Path $QtRoot "$QtVersion\$QtArchPath"
$QtBin            = Join-Path $QtBase "bin"
$WindeployqtPath  = Join-Path $QtBin "windeployqt.exe"
$AQT              = Join-Path $home "\AppData\Roaming\Python\Python312\Scripts\aqt.exe"

$VcPkgPath        = "C:\vcpkg"

$PythonVersion    = "3.12"
$PythonHome       = "C:\Python312"
$Python           = "python$PythonVersion"

$BuildPath        = "$RepoRoot\build"
$InstallPath      = "$RepoRoot\release"

$BuildType        = "Debug"

# ------------------------------------------------------
# Environment
$env:PATH = "$QtBin;$env:PATH"

# Set MS tools
$vcvars = Join-Path "${env:ProgramFiles(x86)}" "Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

# 2) Import the MSVC environment from that batch file
cmd /c "`"$vcvars`" && set" |
  ForEach-Object {
      if ($_ -match "^(.*?)=(.*)$") {
          Set-Item -Path "Env:$($matches[1])" -Value $matches[2]
      }
  }

# ------------------------------------------------------
# Functions
function LogInfo($msg) {
  Write-Host "INFO: $msg" -ForegroundColor Green
}

function LogDebug($msg) {
  Write-Host "DEBUG: $msg" -ForegroundColor Cyan
}

function LogTrace($msg) {
  Write-Host "TRACE: $msg" -ForegroundColor Gray
}

function LogWarning($msg) {
  Write-Host "WARNING: $msg" -ForegroundColor Yellow
}

function Fail($msg) {
  Write-Host "ERROR: $msg" -ForegroundColor Red
  exit 1
}
