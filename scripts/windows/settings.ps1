<#
  scripts/windows/settings.ps1

  Contains all the shared variables needed for the setup, build and release scripts
  Run this as .\settings.ps1
#>

# ------------------------------------------------------
# Variables
$RepoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$QtVersionFile = Join-Path $RepoRoot ".qt-version"

$QtVersion = Get-Content $QtVersionFile -Raw
$QtVersion = $QtVersion.Trim()

$QtArchPath       = "msvc2022_64"
$QtArch           = "win64_$QtArchPath"
$QtRoot           = "C:\Qt"
$QtBase           = Join-Path $QtRoot "$QtVersion\$QtArchPath"
$QtBin            = Join-Path $QtBase "bin"
$WindeployqtPath  = Join-Path $QtBin "windeployqt.exe"

$VcPkgPath        = "C:\vcpkg"

$PythonVersion    = "3.12"
$PythonHome       = "C:\Python312"
$Python           = "python$PythonVersion"

# pip show "Location" is site-packages; aqt.exe lives in Scripts (system or --user).
$aqtCandidates = @(
  (Join-Path $PythonHome "Scripts\aqt.exe"),
  (Join-Path $home "AppData\Roaming\Python\Python$($PythonVersion)\Scripts\aqt.exe")
)
$AQT = ($aqtCandidates | Where-Object { Test-Path -LiteralPath $_ } | Select-Object -First 1)
if (-not $AQT) {
  $aqtCmd = Get-Command aqt -ErrorAction SilentlyContinue
  if ($aqtCmd) { $AQT = $aqtCmd.Source }
}
if (-not $AQT) {
  $AQT = Join-Path $PythonHome "Scripts\aqt.exe"
}

$BuildPath        = "$RepoRoot\build\windows"
$InstallPath      = "$RepoRoot\release\windows"

$BuildType        = "Debug"

# ------------------------------------------------------
# Environment
$env:PATH = "$QtBin;$env:PATH"

# Set MS tools (prefer BuildTools; fall back to Community when BuildTools is not installed)
$vcvars = Join-Path "${env:ProgramFiles(x86)}" "Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
if (-not (Test-Path -LiteralPath $vcvars)) {
  $vcvars = Join-Path $env:ProgramFiles "Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
}

# 2) Import the MSVC environment from that batch file
if (-not (Test-Path -LiteralPath $vcvars)) {
  Write-Host "WARNING: vcvars64.bat not found under VS 18 BuildTools or Community; MSVC may be missing from PATH." -ForegroundColor Yellow
} else {
cmd /c "`"$vcvars`" && set" |
  ForEach-Object {
      if ($_ -match "^(.*?)=(.*)$") {
          Set-Item -Path "Env:$($matches[1])" -Value $matches[2]
      }
  }
  # Shrink PATH: duplicates and bloat can make cmd fail with "The input line is too long".
  $pathParts = $env:PATH -split ';' | Where-Object { $_ } | Select-Object -Unique
  $env:PATH = $pathParts -join ';'
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

# Ensure Qt matching .qt-version is present under $QtBase (CMake needs lib/cmake/Qt6).
function EnsureWindowsQt {
  $qt6Config = Join-Path $QtBase "lib\cmake\Qt6\Qt6Config.cmake"
  if (Test-Path $qt6Config) {
    LogDebug "Qt CMake package found at $QtBase"
    return
  }

  $env:PATH = "$PythonHome;$PythonHome\Scripts;$env:PATH"

  LogWarning "Qt not found at $QtBase (missing Qt6Config.cmake). Installing via aqtinstall..."

  if (-not (Test-Path $AQT)) {
    Fail @"
aqt not found at '$AQT'.
Install aqtinstall (e.g. pip install aqtinstall) or run scripts\windows\setup_tools.ps1, then scripts\windows\setup.ps1.
"@
  }

  if (-not (Test-Path $QtRoot)) {
    New-Item -ItemType Directory -Path $QtRoot | Out-Null
  }

  & $AQT install-qt windows desktop $QtVersion $QtArch `
    -m qtpdf qtwebchannel qtpositioning qtwebsockets `
    -O $QtRoot

  if ($LASTEXITCODE -ne 0) {
    Fail "Qt installation via aqt failed."
  }

  if (-not (Test-Path $qt6Config)) {
    Fail "Qt install finished but Qt6Config.cmake is still missing at $qt6Config."
  }

  LogInfo "Qt installed at: $QtBase"
}
